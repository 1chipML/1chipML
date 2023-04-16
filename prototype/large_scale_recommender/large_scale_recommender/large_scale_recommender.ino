#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "lu_decomposition.h"
#include "finite_difference.h"
#include "gradient_descent.h"
#include "matrix.h"
#include "arduino_serial_port.hpp"

#define LATENT_SIZE 2
#define NUMBER_OF_MATRICES 2 
#define EPS 0.00001
uint8_t feedbackSize = 0;
double *feedbackMatrix;

double* gradientMatrix;

void extractUserMatrix(const double* lMatrix, double* outputMatrix)
{
  for(uint8_t i = 0; i < feedbackSize; ++i)
  {
    memcpy(&outputMatrix[i * LATENT_SIZE], &lMatrix[i * feedbackSize], LATENT_SIZE * sizeof(double));
  }
}

void extractProductMatrix(const double* uMatrix, double* outputMatrix)
{
  memcpy(outputMatrix, uMatrix, LATENT_SIZE * feedbackSize * sizeof(double));
}

static gradient_real func(gradient_real* parameters)
{
  double sum = 0;
  const int OFFSET = feedbackSize * LATENT_SIZE;
  const double weight = 0.01;
  for(uint8_t i = 0; i < feedbackSize; ++i)
  {
    for(uint8_t j = 0; j < feedbackSize; ++j)
    {
      double currentSum = 0;
      for(uint8_t k = 0; k < LATENT_SIZE; ++k)
      {
        currentSum += parameters[coordToIndex(i, k, LATENT_SIZE)] * parameters[coordToIndex(j, k, LATENT_SIZE) + OFFSET];
      }

      if(feedbackMatrix[coordToIndex(i, j, feedbackSize)] != -1)
      {
        const double value = feedbackMatrix[coordToIndex(i, j, feedbackSize)] - currentSum;
        sum += value * value;
      }
      else
      {
        sum += weight * currentSum * currentSum;
      }
    }
  }
  return fabs(sum);
}

static void dfunc(gradient_real* parameters, gradient_real* gradient)
{
  gradientApproximation(func, parameters, gradient, feedbackSize * LATENT_SIZE * NUMBER_OF_MATRICES, EPS, Central);
} 

void setup()
{
  setupSerial(9600);
}

void loop()
{
  readElement(&feedbackSize, sizeof(feedbackSize));
  const uint16_t totalSize = feedbackSize * feedbackSize;

  feedbackMatrix = malloc(totalSize * sizeof(double));

  readArray(totalSize, feedbackMatrix, sizeof(double));

  {
    double lMatrix[totalSize];
    double uMatrix[totalSize];

    memset(lMatrix, 0, totalSize * sizeof(double));
    memset(uMatrix, 0, totalSize * sizeof(double));

    LUDecomposition(feedbackMatrix, lMatrix, uMatrix, feedbackSize);

    gradientMatrix = calloc((feedbackSize * LATENT_SIZE * 2), sizeof(double));

    extractUserMatrix(lMatrix, gradientMatrix);
    extractProductMatrix(uMatrix, &gradientMatrix[feedbackSize * LATENT_SIZE]);
  }

  const unsigned int nbParameters = feedbackSize * LATENT_SIZE * 2;
  double fitness;
  int iterations = 100;
  int status = gradient_descent(func, dfunc, &fitness, gradientMatrix, nbParameters, 0.001, &iterations);

  double transposeProductMatrix[feedbackSize * LATENT_SIZE];
  unsigned int dims[] = {feedbackSize, LATENT_SIZE, feedbackSize};
  matrixTranspose(&gradientMatrix[feedbackSize * LATENT_SIZE], transposeProductMatrix, dims);

  double resultMatrix[totalSize];
  matrixMultiply(gradientMatrix, transposeProductMatrix, dims, resultMatrix, 0);

  writeArray(totalSize, resultMatrix, sizeof(double));

  free(feedbackMatrix);
  free(gradientMatrix);
}
