#include "lu_decomposition.h"
#include "matrix.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Perform the LU decomposition algorithm on the matrix initialMatrix
 *
 * @param initialMatrix The matrix to decompose
 * @param lMatrix Output matrix L
 * @param uMatrix Output matrix U
 * @param size Size of the matrix
 */
void LUDecomposition(const lu_real* initialMatrix, lu_real* lMatrix,
                     lu_real* uMatrix, const int size) {
  const int sizeSquared = size * size;
  lu_real tmpMatrix[sizeSquared];
  memcpy(tmpMatrix, initialMatrix, sizeSquared * sizeof(lu_real));

  for (int k = 0; k < size - 1; ++k) {
    for (int i = k + 1; i < size; ++i) {
      lu_real factor = tmpMatrix[coordToIndex(i, k, size)] /
                       tmpMatrix[coordToIndex(k, k, size)];
      tmpMatrix[coordToIndex(i, k, size)] = factor;
      for (int j = k + 1; j < size; ++j) {
        tmpMatrix[coordToIndex(i, j, size)] =
            tmpMatrix[coordToIndex(i, j, size)] -
            factor * tmpMatrix[coordToIndex(k, j, size)];
      }
    }
  }

  // Copy values to corresponding matrices (l and u)
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size - i; ++j) {
      int col = i + j;
      uMatrix[coordToIndex(j, col, size)] =
          tmpMatrix[coordToIndex(j, col, size)];
    }
    for (int j = 0; j < i + 1; ++j) {
      if (i == j) {
        lMatrix[coordToIndex(i, j, size)] = 1;
      } else {
        lMatrix[coordToIndex(i, j, size)] =
            tmpMatrix[(coordToIndex(i, j, size))];
      }
    }
  }
}
