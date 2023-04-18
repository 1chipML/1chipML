#include "lu_decomposition.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

int compareMatrix(float* matrix1, float* matrix2, int size) {
  for (int i = 0; i < size * size; i++) {
    if (fabs(matrix1[i] - matrix2[i]) > 0.0001) {
      printf("Error: %f != %f", matrix1[i], matrix2[i]);
      return 1;
    }
  }
  return 0;
}

int testLUDecomposition(float* initialMatrix, float* expectedL,
                        float* expectedU, int size) {
  float lMatrix[size * size];
  float uMatrix[size * size];
  memset(lMatrix, 0, size * size * sizeof(float));
  memset(uMatrix, 0, size * size * sizeof(float));

  LUDecomposition(initialMatrix, lMatrix, uMatrix, size);
  int returnCode = 0;
  returnCode |= compareMatrix(expectedL, lMatrix, size);
  returnCode |= compareMatrix(expectedU, uMatrix, size);

  if (returnCode == 0) {
    printf("Success %s()\n", __func__);
  }
  return returnCode;
}

int main() {
  const int size = 4;
  float initialMatrix[] = {2, 3,  5,  5,  6, 13, 5,  19,
                           2, 19, 10, 23, 4, 10, 11, 31};

  float expectedL[] = {1, 0, 0, 0, 3, 1, 0, 0, 1, 4, 1, 0, 2, 1, 0.24444444, 1};
  float expectedU[] = {2, 3, 5,  5, 0, 4, -10, 4,
                       0, 0, 45, 2, 0, 0, 0,   16.511111};

  return testLUDecomposition(initialMatrix, expectedL, expectedU, size);
}
