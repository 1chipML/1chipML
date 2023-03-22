#include "lanczos.h"
#include "linear_congruential_random_generator.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
 * Multiply 2 matrices together
 * using this equation : output = firstMatrix x secondMatrix
 * The input parameter size is composed of 3 values :
 *  The number of rows of the first matrix
 *  The number of column of the first matrix
 *  The number of column of the second matrix
 */
void matrixMultiply(lanczos_real* firstMatrix, lanczos_real* secondMatrix,
                    uint_least8_t size[3], lanczos_real* output) {
  for (uint_least8_t i = 0; i < size[0]; ++i) {
    for (uint_least8_t j = 0; j < size[2]; ++j) {
      output[i * size[2] + j] = 0;
      for (uint_least8_t k = 0; k < size[1]; ++k) {
        output[i * size[2] + j] +=
            firstMatrix[i * size[1] + k] * secondMatrix[k * size[2] + j];
      }
    }
  }
}

static void transpose(lanczos_real* input, lanczos_real* output,
                      uint_least8_t* dims) {
  for (uint_least8_t i = 0; i < dims[0]; ++i) {
    for (uint_least8_t j = 0; j < dims[1]; ++j) {
      output[j * dims[0] + i] = input[i * dims[1] + j];
    }
  }
}

static lanczos_real computeNorm(lanczos_real* vector, uint_least8_t length) {
  lanczos_real tmp = 0;
  for (uint_least8_t i = 0; i < length; ++i) {
    tmp += vector[i] * vector[i];
  }
  return sqrt(tmp);
}

void vectorScale(lanczos_real* vector, uint_least8_t nbElements,
                 lanczos_real scale) {
  for (uint_least8_t i = 0; i < nbElements; ++i) {
    vector[i] *= scale;
  }
}

void vectorSubstract(lanczos_real* vector1, lanczos_real* vector2,
                     uint_least8_t dim) {
  for (uint_least8_t i = 0; i < dim; ++i) {
    vector1[i] = vector1[i] - vector2[i];
  }
}

void makeUnitVector(lanczos_real* vector, uint_least8_t nbElements) {
  lanczos_real norm = computeNorm(vector, nbElements);
  vectorScale(vector, nbElements, 1.0 / norm);
}

/**
 * Assume all vectors in vectorList
 * are already orthonormal and unit vectors
 * so by using gram-Schmidt we are basically
 * orthogonalising the new vector with all the previous ones.
 *
 * Gram-Schmidt is also prone to numerical instability
 * but this issue mostly arise when vectors are linearly dependant
 * In that case the loss of orthogonality is due to numerical instability
 * so vectors won't be linearly dependant. Instead they should already be
 * almost orthogonal.
 */
static void gramSchmidt(lanczos_real* vectorList, uint_least8_t nbVectors,
                        uint_least8_t vectorLength,
                        lanczos_real* vectorToChange) {
  for (uint_least8_t i = 0; i < nbVectors; ++i) {
    lanczos_real dotProduct;
    uint_least8_t dims[] = {1, vectorLength, 1};
    lanczos_real tmpVector[vectorLength];

    memcpy(tmpVector, &vectorList[i * vectorLength],
           vectorLength * sizeof(lanczos_real));
    matrixMultiply(tmpVector, vectorToChange, dims, &dotProduct);
    vectorScale(tmpVector, vectorLength, dotProduct);
    vectorSubstract(vectorToChange, tmpVector, vectorLength);
  }
  lanczos_real norm = computeNorm(vectorToChange, vectorLength);
  vectorScale(vectorToChange, vectorLength, 1.0 / norm);
}

static void getRandomUnitVector(lanczos_real* vector, uint_least8_t dim) {
  for (uint_least8_t i = 0; i < dim; ++i) {
    vector[i] = linear_congruential_random_generator();
  }

  makeUnitVector(vector, dim);
}

void lanczos(lanczos_real* matrix, uint_least8_t dim, uint_least8_t nbIter,
             lanczos_real* initialVector, lanczos_real* tMatrix,
             lanczos_real* vMatrix) {
  lanczos_real q0[dim];
  lanczos_real q1[dim];

  lanczos_real beta = 0;
  // Temporary vector to store the transpose of the V matrix.
  lanczos_real vTranspose[nbIter][dim];

  if (initialVector == NULL) {
    // q1 is a random unit vector and q0 is a vector filled with 0
    getRandomUnitVector(q1, dim);
  } else {
    makeUnitVector(initialVector, dim);
    memcpy(q1, initialVector, dim * sizeof(lanczos_real));
  }
  memset(q0, 0, dim * sizeof(lanczos_real));

  for (uint_least8_t i = 0; i < nbIter; ++i) {
    // Store the current vector v as the ith row of the vTranspose matrix
    memcpy(vTranspose[i], q1, dim * sizeof(lanczos_real));

    // Compute the value of v from the equation
    // v = A*qn
    lanczos_real v[dim];
    uint_least8_t dims[3] = {dim, dim, 1};
    matrixMultiply(matrix, q1, dims, v);

    // Compute the value of alpha using the equation
    // alpha = transpose(q) * v
    // also, since q is a vector we do not really need to transpose it
    lanczos_real alpha;
    uint_least8_t dims2[3] = {1, dim, 1};
    matrixMultiply(q1, v, dims2, &alpha);

    // Recompute a new v using the equation
    // v = v - beta(n-1) * q(n-1) - alpha * q(n)
    lanczos_real alphaDotQ[dim];
    memcpy(alphaDotQ, q1, dim * sizeof(lanczos_real));
    vectorScale(alphaDotQ, dim, alpha);

    lanczos_real betaDotQ0[dim];
    memcpy(betaDotQ0, q0, dim * sizeof(lanczos_real));
    vectorScale(betaDotQ0, dim, beta);

    vectorSubstract(v, betaDotQ0, dim);
    vectorSubstract(v, alphaDotQ, dim);

    // Compute the value of beta
    // Where beta is the norm of v
    beta = computeNorm(v, dim);

    // Store q0 now becomes q1 and q1 is computed using the equation
    // q1 = v / beta
    //
    // Special case where beta is 0
    // In that case q1 should be a new randomly generated vector
    // orthogonal to all previous vectors in vTranspose
    memcpy(q0, q1, dim * sizeof(lanczos_real));
    memcpy(q1, v, dim * sizeof(lanczos_real));
    if (beta >= -0.00001 && beta <= 0.00001) {
      getRandomUnitVector(q1, dim);
    } else {
      vectorScale(q1, dim, 1.0 / beta);
    }

    // Reorthogonalize the vector q1 using the modified Gram-Schmidth algorithm
    gramSchmidt(vTranspose[0], i + 1, dim, q1);

    // Fill the matrix T with the current alpha and beta values
    tMatrix[i * nbIter + i] = alpha;
    if (i != nbIter - 1) {
      tMatrix[(i + 1) * nbIter + i] = beta;
      tMatrix[i * nbIter + i + 1] = beta;
    }
  }

  uint_least8_t vMatrixSize[] = {nbIter, dim};
  transpose(vTranspose[0], vMatrix, vMatrixSize);
}
