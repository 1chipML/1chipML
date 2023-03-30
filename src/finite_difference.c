#include "finite_difference.h"
#include <stdio.h>
#include <string.h>

/// @brief Approximates the gradient of a function using the finite difference
/// method

/// @param func The function for which to approximate the gradient
/// @param point The point at which to appriximate the gradient
/// @param grad Return parameter containing the approximated gradient
/// @param n Number of dimensions of the function
/// @param eps Size of the step taken away from point for the approximation.
/// Lower eps should lead to better approximations. Should not be smaller than
/// the machine floating point precision.
/// @param type Type of finite difference approximation (Forward, Backward or
/// Central)
void gradientApproximation(function func, real point[], real grad[], int n,
                           real eps, approximationType type) {
  // It is imperative to choose h so that x and x + h differ by an exactly
  // representable number
  // see:
  // http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c5-7.pdf
  real h_next[n];
  real h_prev[n];
  for (int i = 0; i < n; i++) {
    volatile real temp = point[i] + eps;
    h_next[i] = temp - point[i];

    temp = point[i] - eps;
    h_prev[i] = point[i] - temp;
  }

  if (type == Forward) {
    real next[n];
    memcpy(next, point, n * sizeof(real));

    for (int i = 0; i < n; i++) {
      next[i] += h_next[i];
      grad[i] = (func(next) - func(point)) / h_next[i];
      next[i] -= h_next[i];
    }
  } else if (type == Backward) {
    real prev[n];
    memcpy(prev, point, n * sizeof(real));

    for (int i = 0; i < n; i++) {
      prev[i] -= h_prev[i];
      grad[i] = (func(point) - func(prev)) / h_prev[i];
      prev[i] += h_prev[i];
    }
  } else {
    real next[n];
    real prev[n];
    memcpy(next, point, n * sizeof(real));
    memcpy(prev, point, n * sizeof(real));

    for (int i = 0; i < n; i++) {
      next[i] += h_next[i];
      prev[i] -= h_prev[i];
      grad[i] = (func(next) - func(prev)) / (h_prev[i] + h_next[i]);
      prev[i] += h_prev[i];
      next[i] -= h_next[i];
    }
  }
}