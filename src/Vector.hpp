// Copyright 2024 Marco Calderón Guevara <marco.calderonguevara@ucr.ac.cr>
///               Andy Cen Wu <andy.cen@ucr.ac.cr>
///               Arianna Leitón Quesada <arianna.leiton@ucr.ac.cr>
///               Juliana Rodríguez Mora <juliana.rodriguez@ucr.ac.cr>

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <stdexcept>

class Vector {
 private:
  double components[3] = {0.0};  // Initialize all elements to 0.0

 public:
  /**
   * @brief Default constructor. Initializes the vector to (0, 0, 0).
   */
  Vector() = default;  // Default constructor

  /**
   * @brief Constructs a vector with the specified components.
   * 
   * @param x The x-component of the vector.
   * @param y The y-component of the vector.
   * @param z The z-component of the vector.
   */
  Vector(double x, double y, double z) : components{x, y, z} {}

  /**
   * @brief Accesses a vector component by index.
   * 
   * @param index The index of the component (0 for x, 1 for y, 2 for z).
   * @return A reference to the component, allowing modification.
   * @throws std::out_of_range If the index is not in the range [0, 2].
   */
  double& operator[](size_t index) {
    if (index >= 3) throw std::out_of_range("Index out of range");
    return components[index];
  }

  /**
   * @brief Accesses a vector component by index (const version).
   * 
   * @param index The index of the component (0 for x, 1 for y, 2 for z).
   * @return A constant reference to the component.
   * @throws std::out_of_range If the index is not in the range [0, 2].
   */
  const double& operator[](size_t index) const {
    if (index >= 3) throw std::out_of_range("Index out of range");
    return components[index];
  }

  /**
   * @brief Adds two vectors.
   * 
   * @param other The vector to add.
   * @return A new vector that is the sum of this vector and the other.
   */
  Vector operator+(const Vector& other) const {
    return {components[0] + other[0], components[1] + other[1], components[2] +
        other[2]};
  }

  /**
   * @brief Subtracts one vector from another.
   * 
   * @param other The vector to subtract.
   * @return A new vector that is the result of the subtraction.
   */
  Vector operator-(const Vector& other) const {
    return {components[0] - other[0], components[1] - other[1], components[2] -
        other[2]};
  }

  /**
   * @brief Calculates the dot product of two vectors.
   * 
   * @param other The vector to calculate the dot product with.
   * @return The scalar dot product of the two vectors.
   */
  double operator*(const Vector& other) const {
    return components[0] * other[0] + components[1] * other[1] + components[2] *
        other[2];
  }

  /**
   * @brief Multiplies the vector by a scalar.
   * 
   * @param scalar The scalar value to multiply with.
   * @return A new vector that is the result of the scalar multiplication.
   */
  Vector operator*(double scalar) const {
    return {components[0] * scalar, components[1] * scalar, components[2] *
        scalar};
  }

  /**
   * @brief Divides the vector by a scalar.
   * 
   * @param scalar The scalar value to divide by.
   * @return A new vector that is the result of the scalar division.
   * @throws std::invalid_argument If the scalar is zero.
   */
  Vector operator/(double scalar) const {
    if (scalar == 0.0) throw std::invalid_argument("Cannot divide by zero");
    return {components[0] / scalar, components[1] / scalar, components[2] /
        scalar};
  }

  /**
   * @brief Calculates the norm (magnitude) of the vector.
   * 
   * The norm is computed as the square root of the sum of the squares of the components.
   * @return The norm of the vector.
   */
  double norm() const {
    return std::sqrt((*this) * (*this));
  }
};
#endif
