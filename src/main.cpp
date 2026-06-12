// Copyright 2024 Marco Calderón Guevara <marco.calderonguevara@ucr.ac.cr>
///               Andy Cen Wu <andy.cen@ucr.ac.cr>
///               Arianna Leitón Quesada <arianna.leiton@ucr.ac.cr>
///               Juliana Rodríguez Mora <juliana.rodriguez@ucr.ac.cr>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "Simulation.hpp"

#include "Mpi.hpp"

/**
 * @brief Loads simulations into a vector of `Simulation` objects from a job file,
 * and then returns the vector of `Simulation` objects.
 * 
 * @param filename Name of the job file to process.
 * @param threadCount Number of threads to use.
 * @return std::vector<Simulation> Vector of `Simulation` objects created with
 * information from the job file.
 */
std::vector<Simulation> loadSimulations(const std::string& filename,
    int threadCount) {
  std::vector<Simulation> simulations;
  std::ifstream jobs_file(filename);
  if (!jobs_file) {  // Checks if file opened to read
    throw std::ios_base::failure("error: could not open job file");
  }

  // Read each line and extract data
  std::string universeFilename;
  double deltaT = 0, finalTime = 0;
  while (jobs_file >> universeFilename >> deltaT >> finalTime) {
    Simulation simulation(universeFilename, deltaT, finalTime, threadCount);
    simulations.push_back(simulation);  // Add simulation to the vector
  }

  if (simulations.empty()) {
    throw std::runtime_error("error: no valid simulations found");
  }

  return simulations;
}

/**
 * @brief Method that executes the Master process (or process 0). It distributes
 * simulations to the processes using dynamic mapping if there is more than one
 * process. If there is only one process, it performs the work itself. Once all 
 * simulations are completed, it sends termination signals to the other
 * processes (if any).
 * 
 * @param mpi MPI object.
 * @param simulations Vector containing the simulations.
 * @param argv Input argument vector.
 */
void masterProcess(Mpi& mpi, const std::vector<Simulation>& simulations,
    char* argv[]) {
  if (mpi.size() > 1) {
    int process = 0;
    for (int simulationIndex = 0; simulationIndex
        < static_cast<int>(simulations.size()); ++simulationIndex) {
      // Waits for a process to be ready
      mpi.receive(process, MPI_ANY_SOURCE);
      // Sends the index of the simulation to the available process
      mpi.send(simulationIndex, process);
    }

    int stopSignal = -1;
    for (int stopCount = 1; stopCount < mpi.size(); ++stopCount) {
      mpi.receive(process, MPI_ANY_SOURCE); mpi.send(stopSignal, process);
    }
  } else {
    for (size_t simulationIndex = 0; simulationIndex < simulations.size();
        ++simulationIndex) {
      const_cast<Simulation&>(simulations[simulationIndex]).
          startSimulation(argv);
    }
  }
}

/**
 * @brief Method executed by slave processes (those that are not process 0).
 * Each process receives an index indicating the simulation it should work on. 
 * Once finished, it sends its own process number to request another simulation. 
 * When it receives the termination signal from the master process, it stops.
 * 
 * @param mpi MPI object.
 * @param simulations Vector containing the simulations.
 * @param argv Input argument vector.
 */
void slaveProcess(Mpi& mpi, const std::vector<Simulation>& simulations,
    char* argv[]) {
  int process = mpi.rank();  // Get the rank of this process
  int simulationIndex = 0;

  while (true) {
    // Notify master process that this process is ready
    mpi.send(process, 0);
    // Receive the simulation index from master
    mpi.receive(simulationIndex, 0);
    // If simulationIndex is -1, stop working
    if (simulationIndex == -1) break;
    const_cast<Simulation&>(simulations[simulationIndex]).startSimulation(argv);
  }
}

/**
 * @brief Main method that loads simulations, distributes them among processes, 
 * and ensures each process performs its assigned simulation.
 * 
 * @param argc Number of input arguments.
 * @param argv Argument vector.
 * @return int EXIT_SUCCESS if executed successfully, EXIT_FAILURE otherwise.
 */
int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);

    // Check if enough arguments are provided
    if (argc < 2) {
      throw std::invalid_argument("usage: <program> <jobs_file> <threads>");
    }

    int threadCount = (argc == 3) ? std::atoi(argv[2]) :
        std::thread::hardware_concurrency();
    if (threadCount <= 0) {
      throw std::invalid_argument("error: invalid thread count");
    }

    // Load the simulations from the provided file
    std::vector<Simulation> simulations = loadSimulations(argv[1], threadCount);

    if (mpi.rank() == 0) {
      masterProcess(mpi, simulations, argv);
    } else {
      slaveProcess(mpi, simulations, argv);
    }
  } catch (const Mpi::Error& error) {
    // Handle MPI-specific errors
    std::cerr << "MPI error: " << error.what() << std::endl;
  } catch (const std::exception& exception) {
    // Handle other general errors
    std::cerr << exception.what() << std::endl;
    return EXIT_FAILURE;  // Return failure if an error occurred
  }
  return EXIT_SUCCESS;  // Return success if the program completes successfully
}
