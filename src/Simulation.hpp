// Copyright 2024 Marco Calderón Guevara <marco.calderonguevara@ucr.ac.cr>
///               Andy Cen Wu <andy.cen@ucr.ac.cr>
///               Arianna Leitón Quesada <arianna.leiton@ucr.ac.cr>
///               Juliana Rodríguez Mora <juliana.rodriguez@ucr.ac.cr>

#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <string>
#include <vector>
#include <utility>

#include "Vector.hpp"

/// Universal gravitational constant in m³/(kg·s²)
#define GRAVITATIONAL_CONSTANT 6.67430e-11

class Simulation {
 private:
  /**
   * @brief Represents a celestial body in the universe.
   *
   * Contains information about the mass, radius, position, and
   * velocity of the body.
   */
  class Body{
   public:
    double mass = 0.0, radius = 0.0;
    Vector position, velocity;

    /**
     * @brief Default constructor.
     * 
     */
    Body() = default;
  };

  std::string universeFilename;
  double deltaT = 0.0, finalTime = 0.0;
  int threadCount;
  std::vector<Body> bodies;

 public:
  /**
   * @brief Constructs a new Simulation object with the provided parameters.
   * 
   * @param universeFilename Universe file name.
   * @param deltaT Duration of each body simulation step.
   * @param finalTime Final duration of the body simulation.
   * @param threadCount Number of threads for running the simulation.
   */
  Simulation(std::string& universeFilename, double deltaT,
      double finalTime, int threadCount) : universeFilename(universeFilename),
      deltaT(deltaT), finalTime(finalTime), threadCount(threadCount) {}

  /**
   * @brief Default constructor.
   * 
   */
  Simulation() = default;

  /**
   * @brief Runs the simulation of gravitational interaction between celestial
   * bodies.
   *
   * @param argv Additional command-line arguments.
   */
  void startSimulation(char* argv[]);

  /**
   * @brief Updates the velocities of the bodies in the universe.
   *
   */
  void updateVelocity();

  /**
   * @brief Updates the positions of the bodies in the universe.
   *
   */
  void updatePosition();

  /**
   * @brief Checks and handles collisions between celestial bodies.
   *
   * @param absorbedCount Number of bodies absorbed due to collisions.
   */
  void checkCollisions(int& absorbedCount);

  /**
   * @brief Updates the parameters of a body after a collision.
   *
   * @param bodyToUpdate Index of the body to be updated.
   * @param bodyAbsorbed Index of the absorbed body.
   */
  void absorb(int bodyToUpdate, int bodyAbsorbed);

  /**
   * @brief Writes the updated state of the universe to a file.
   *
   * @param jobFileName Name of the job file.
   * @param timePassed Time elapsed since the start of the simulation
   * (in seconds).
   */
  void writeUniverse(const char* jobFileName, double timePassed);
};

#endif

