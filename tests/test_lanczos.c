#include "matrix.h"
#include <lanczos.h>
#include <stdio.h>
#include <string.h>

#define size 3
#define nbIter 2

static double computeDeterminant2X2(double* matrix) {
  return matrix[0] * matrix[3] - matrix[1] * matrix[2];
}

static int isAlmostZero(const double* value) {
  if (*value > -0.0001 && *value < 0.0001) {
    return 0;
  }

  return 1;
}

static int validateEigenValue(double* tMatrix, double eigenValue) {
  double identity[nbIter][nbIter] = {{1.0, 0.0}, {0.0, 1.0}};
  double tCopy[nbIter][nbIter];
  memcpy(tCopy[0], tMatrix, (nbIter) * (nbIter) * sizeof(double));

  vectorScale(&identity[0][0], nbIter * nbIter, eigenValue);
  vectorSubstract(&tCopy[0][0], &identity[0][0], nbIter * nbIter);
  double determinant = computeDeterminant2X2(&tCopy[0][0]);
  if (isAlmostZero(&determinant) != 0) {
    printf("Fail : %s(), expected determinant to be 0 but got %f", __func__,
           determinant);
    return 1;
  }
  return 0;
}

int testLanczos(double* initialMatrix, double* tMatrix, double* vMatrix,
                double* initialVector, double expectedEigenValue) {
  lanczos(initialMatrix, size, nbIter, initialVector, tMatrix, vMatrix);

  // Eigenvalues were manually calculated for this test.
  if (validateEigenValue(tMatrix, expectedEigenValue) != 0) {
    return 1;
  }

  // Now that we have validated the eigenvalues let's test the eigenVector.
  // We will test the eigen vector (1, 1, 1) associated with the eigenvalue 3
  // We know that when the eigenValue is 4 the corresponding eigen vector from
  // the matrix T is (3.137852623, 1) So to get the associated eigen vector of
  // the matrix A we simply compute V times the eigen vector
  {
    // With eigenvector (1, 1, 1)
    double eigenVector[] = {2.44949, 1};
    double output[3];
    matrix_size dims[] = {size, nbIter, 1};
    matrixMultiply(vMatrix, eigenVector, dims, output, 0);
    // Since the vector is (1, 1, 1) it's easy to validate
    // simply make sure that all the elements are the same
    for (int i = 0; i < size - 1; ++i) {
      // Using diff here since we have floating point values
      double diff = output[i] - output[i + 1];
      if (isAlmostZero(&diff) != 0) {
        printf("Fail : %s(), expected %f == %f\n", __func__, output[i],
               output[i + 1]);
        return 1;
      }
    }
  }
  printf("Success : %s()\n", __func__);
  return 0;
}

int main() {
  double initialMatrix[size][size] = {
      {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}};
  double tMatrix[nbIter][nbIter] = {{0.0, 0.0}, {0.0, 0.0}};
  double vMatrix[nbIter][size] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  double initialVector[size] = {1.0, 2.0, 3.0};
  const double expectedEigenValue = 3.0;

  return testLanczos(initialMatrix[0], tMatrix[0], vMatrix[0], initialVector,
                     expectedEigenValue);
}
