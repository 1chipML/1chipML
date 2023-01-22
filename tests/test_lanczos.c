#include <lanczos.h>
#include <stdio.h>
#include <string.h>

#define size 3
#define nbIter 2

static void printMatrix(double* matrix, int row, int column) {
  for (int i = 0; i < row; ++i) {
    for (int j = 0; j < column; ++j) {
      printf("%f ", matrix[i * column + j]);
    }
    printf("\n");
  }
  printf("\n");
}

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
  memcpy(&tCopy[0][0], tMatrix, (nbIter) * (nbIter) * sizeof(double));
  vectorScale(&identity[0][0], nbIter * nbIter, eigenValue);
  vectorSubstract(&tCopy[0][0], &identity[0][0], nbIter * nbIter);
  double determinant = computeDeterminant2X2(&tCopy[0][0]);
  printf("With eigenValue of %f ", eigenValue);
  printf("determinant is %f (Should be 0)\n", determinant);
  if (isAlmostZero(&determinant) != 0) {
    return 1;
  }
  return 0;
}

int testLanczos() {
  double initialMatrix[size][size] = {
      {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}};
  double tMatrix[nbIter][nbIter] = {{0.0, 0.0}, {0.0, 0.0}};
  double vMatrix[nbIter][size] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  printf("Testing matrix with %d iterations:\n", nbIter);
  printMatrix(&initialMatrix[0][0], size, size);

  lanczos(initialMatrix[0], size, tMatrix[0], vMatrix[0], nbIter);

  puts("After Lanczos Matrix T is");
  printMatrix(&tMatrix[0][0], nbIter, nbIter);
  puts("Matrix V is:");
  printMatrix(&vMatrix[0][0], size, nbIter);

  // Eigenvalues were manually calculated for this test.
  validateEigenValue(&tMatrix[0][0], 3.0);

  // Now that we have validated the eigenvalues let's test the eigenVector.
  // We will test the eigen vector (1, 1, 1) associated with the eigenvalue 3
  // We know that when the eigenValue is 4 the corresponding eigen vector from
  // the matrix T is (3.137852623, 1) So to get the associated eigen vector of
  // the matrix A we simply compute V times the eigen vector
  {
    puts("\nComputing eigenVector");
    // With eigenvector (1, 1, 1)
    double eigenVector[] = {3.13786, 1};
    double output[3];
    uint_least8_t dims[] = {size, nbIter, 1};
    matrixMultiply(&vMatrix[0][0], eigenVector, dims, output);
    printMatrix(output, 1, size);
    // Since the vector is (1, 1, 1) it's easy to validate
    // simply make sure that all the elements are the same
    for (int i = 0; i < size - 1; ++i) {
      // Using diff here since we have floating point values
      double diff = output[i] - output[i + 1];
      if (!isAlmostZero(&diff)) {
        return 1;
      }
    }
  }
  return 0;
}

int main() { return testLanczos(); }
