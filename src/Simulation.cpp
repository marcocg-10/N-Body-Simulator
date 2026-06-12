// Copyright 2024 Marco Calderón Guevara <marco.calderonguevara@ucr.ac.cr>
///               Andy Cen Wu <andy.cen@ucr.ac.cr>
///               Arianna Leitón Quesada <arianna.leiton@ucr.ac.cr>
///               Juliana Rodríguez Mora <juliana.rodriguez@ucr.ac.cr>

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <vector>

#include "omp.h"

#include "Simulation.hpp"
#include "Vector.hpp"

void Simulation::startSimulation(char* argv[]) {
  // File Route
  std::string universeFilePath = (std::filesystem::path(argv[1]).parent_path() /
      this->universeFilename).string();

  // Determine file format with the extension
  std::string extension = this->universeFilename.substr
      (this->universeFilename.find_last_of(".") + 1);

  Body body;

  if (extension == "tsv") {
    // Read tsv file
    std::ifstream universeFile(universeFilePath);
    if (!universeFile) {
      throw std::ios_base::failure("Error: could not open: " +
          this->universeFilename);
    }
    // Read each line of the file and initialize the bodies in the simulation
    while (universeFile >> body.mass >> body.radius >> body.position[0] >>
        body.position[1] >> body.position[2] >> body.velocity[0] >>
        body.velocity[1] >> body.velocity[2]) {
      this->bodies.push_back(body);
    }
  } else if (extension == "bin") {
    // Read bin file
    std::ifstream universeFile(universeFilePath, std::ios::binary);
    if (!universeFile) {
      throw std::ios_base::failure("Error: could not open: " +
          this->universeFilename);
    }
    while (universeFile.read(reinterpret_cast<char*>(&body), sizeof(Body))) {
      this->bodies.push_back(body);
    }
  } else {
    throw std::runtime_error("Error: unsupported file format: " + extension);
  }

  if (this->bodies.empty()) {
    throw std::runtime_error("Error: no valid bodies found in the file");
  }

  double timePassed = 0; int absorbedCount = 0;
  while (timePassed < this->finalTime && absorbedCount + 1 <
      static_cast<int>(this->bodies.size())) {
    this->checkCollisions(absorbedCount);
    this->updateVelocity();
    this->updatePosition();
    timePassed += this->deltaT;
  }
  writeUniverse(argv[1], timePassed);
}

void Simulation::updateVelocity() {
  #pragma omp parallel for num_threads(threadCount) default(none) \
      shared(bodies, deltaT, finalTime) schedule(dynamic)
  for (size_t body = 0; body < bodies.size(); ++body) {
    if (bodies[body].mass == -1) continue;  // Skip absorbed bodies

    // To store the resulting acceleration
    Vector acceleration;
    for (size_t otherBody = 0; otherBody < bodies.size(); ++otherBody) {
      if (bodies[otherBody].mass == -1 || body == otherBody) continue;
      // Calculate the distance vector between bodies
      Vector distance = bodies[body].position - bodies[otherBody].position;

      acceleration = acceleration + (distance * (bodies[otherBody].mass /
          std::pow(distance.norm(), 3)));
    }
    // Apply the gravitational constant and update velocity
    bodies[body].velocity = bodies[body].velocity + (acceleration *
        -GRAVITATIONAL_CONSTANT * deltaT);
  }
}

void Simulation::updatePosition() {
  #pragma omp parallel for num_threads(threadCount) default(none) \
      shared(bodies, deltaT, finalTime) schedule(dynamic)
  for (size_t body = 0; body < bodies.size(); ++body) {
    if (bodies[body].mass != -1)  // Skip absorbed bodies
      // Update the position based on the velocity and delta time
      bodies[body].position = bodies[body].position + (bodies[body].velocity *
          deltaT);
  }
}

void Simulation::checkCollisions(int& absorbedCount) {
  #pragma omp parallel for num_threads(threadCount) default(none) \
      shared(bodies, deltaT, finalTime, absorbedCount) schedule(dynamic)
  for (size_t body = 0; body < bodies.size(); ++body) {
    if (bodies[body].mass == -1) continue;  // Skip absorbed bodies

    // Check for collisions with every other body
    for (size_t otherBody = 0; otherBody < bodies.size(); ++otherBody) {
      if (bodies[otherBody].mass == -1 || body == otherBody) continue;

      // Calculate the distance vector between bodies
      Vector distance = bodies[body].position - bodies[otherBody].position;
      // Check if the bodies have collided
      if (bodies[body].radius + bodies[otherBody].radius > distance.norm()) {
        #pragma omp critical
        {
          ++absorbedCount;
          // Absorb the smaller body into the larger one
          if (bodies[body].mass < bodies[otherBody].mass) {
            absorb(otherBody, body);
          } else if (bodies[body].mass > bodies[otherBody].mass) {
            absorb(body, otherBody);
          } else {
            if (body < otherBody) {
              absorb(body, otherBody);
            } else {
              absorb(otherBody, body);
            }
          }
        }
      }
    }
  }
}

void Simulation::absorb(int bodyToUpdate, int bodyAbsorbed) {
  if (bodies[bodyToUpdate].mass == -1 || bodies[bodyAbsorbed].mass == -1) return;
  this->bodies[bodyToUpdate].velocity = (this->bodies[bodyToUpdate].velocity *
      this->bodies[bodyToUpdate].mass + this->bodies[bodyAbsorbed].velocity *
      this->bodies[bodyAbsorbed].mass) / (this->bodies[bodyToUpdate].mass +
      this->bodies[bodyAbsorbed].mass);
  this->bodies[bodyToUpdate].radius = cbrt(std::pow(this->bodies[bodyToUpdate].
      radius, 3) + std::pow(this->bodies[bodyAbsorbed].radius, 3));
  this->bodies[bodyToUpdate].mass += this->bodies[bodyAbsorbed].mass;
  this->bodies[bodyAbsorbed].mass = -1;  // Mark as absorbed
}

void Simulation::writeUniverse(const char* jobFileName, double timePassed) {
  char timeStr[50];
  snprintf(timeStr, sizeof(timeStr), "%lg", timePassed);  // %lg adjust format

  std::string outputFileName = (std::filesystem::path(jobFileName).parent_path()
      / (this->universeFilename.substr(0,
      this->universeFilename.find_last_of(".")) + "-" + timeStr)).string();

  std::string extension =
    this->universeFilename.substr(this->universeFilename.find_last_of(".") + 1);

  if (extension == "tsv") {
    // Write tsv file
    std::ofstream outputFile(outputFileName + ".tsv");
    if (!outputFile) {
      throw std::ios_base::failure("Error: could not open universe to write");
    }
    for (const auto& body : bodies) {
      outputFile << body.mass << "\t" << body.radius << "\t" << body.position[0]
          << "\t" << body.position[1] << "\t" << body.position[2] << "\t" <<
          body.velocity[0] << "\t" << body.velocity[1] << "\t" <<
          body.velocity[2] << std::endl;
    }
  } else if (extension == "bin") {
    // Write bin file
    std::ofstream outputFile(outputFileName + ".bin", std::ios::binary);
    if (!outputFile) {
      throw std::ios_base::failure("Error: could not open universe to write");
    }
    for (const auto& body : bodies) {
      outputFile.write(reinterpret_cast<const char*>(&body), sizeof(Body));
    }
  } else {
    throw std::runtime_error("Error: unsupported file format for writing");
  }
}

