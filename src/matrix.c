#include "matrix.h"
#include <math.h>

/**
 * @brief Converts a 2d coordinate in a matrix to an index in a 2d array
 * @param row the row number
 * @param col the column number
 * @param size the number of columns in the matrix
 */
inline matrix_size coordToIndex(const matrix_size row, const matrix_size col,
                                const matrix_size size) {
  return row * size + col;
}

/**
 * @brief Check if the given index is part of the diagonal
 * @param index the index to check
 * @param size the number of columns in the matrix
 */
inline matrix_size isDiagonal(const matrix_size index, const matrix_size size) {
  return ((index / size) == (index % size));
}

/**
 * @brief Multiplies 2 matrices together using this equation : output =
 * firstMatrix x secondMatrix
 * @param firstMatrix first matrix to multiply of size m * n
 * @param secondMatrix second matrix to multiply of size n * p
 * @param size array of size 3 containing m, n and p
 * @param output matrix resulting of the matrix multiplication of the given
 * matrices
 * @param transposeFirstMatrix whether to transpose the first matrix when
 * multiplying the matrices
 */
void matrixMultiply(const matrix_real_number* firstMatrix,
                    const matrix_real_number* secondMatrix,
                    const matrix_size size[3], matrix_real_number* output,
                    const matrix_size transposeFirstMatrix) {
  const matrix_size m = size[0];
  const matrix_size n = size[1];
  const matrix_size p = size[2];
  for (matrix_size i = 0; i < m; ++i) {
    for (matrix_size j = 0; j < p; ++j) {
      matrix_real_number sum = 0.0;
      for (matrix_size k = 0; k < n; ++k) {
        sum += (transposeFirstMatrix ? firstMatrix[coordToIndex(k, i, n)]
                                     : firstMatrix[coordToIndex(i, k, n)]) *
               secondMatrix[coordToIndex(k, j, p)];
      }
      output[coordToIndex(i, j, p)] = sum;
    }
  }
}

/**
 * @brief Creates the identity matrix (1 on the diagonal and 0 on other
 * coordinates)
 * @param size size of the matrix
 * @param output output matrix
 */
void createIdentityMatrix(const matrix_size size, matrix_real_number* output) {
  const matrix_size sizeSquared = size * size;
  for (matrix_size i = 0; i < sizeSquared; ++i) {
    if (isDiagonal(i, size)) {
      output[i] = 1;
    } else {
      output[i] = 0;
    }
  }
}

/**
 * @brief Transpose a matrix
 * @param input the matrix to transpose
 * @param output an array to store the transposed matrix
 * @param dims an array of size 3 with the number of rows of the first matrix,
 * the number of rows of the second matrix and the number of columns of the
 * second matrix
 */
void matrixTranspose(matrix_real_number* input, matrix_real_number* output,
                     matrix_size* dims) {
  for (matrix_size i = 0; i < dims[0]; ++i) {
    for (matrix_size j = 0; j < dims[1]; ++j) {
      output[j * dims[0] + i] = input[i * dims[1] + j];
    }
  }
}

/**
 * @brief Apply a multiplication between a vector and a scalar
 * @param vector The vector to scale
 * @param nbElements The number of elements in the vector
 * @param scale The scalar
 */
void vectorScale(matrix_real_number* vector, matrix_size nbElements,
                 matrix_real_number scale) {
  for (matrix_size i = 0; i < nbElements; ++i) {
    vector[i] *= scale;
  }
}

/**
 * @brief Substracts 2 vectors together
 * @param vector1 The first vector
 * @param vector2 The second vector
 * @param dim the number of elements in the vector
 */
void vectorSubstract(matrix_real_number* vector1, matrix_real_number* vector2,
                     matrix_size dim) {
  for (matrix_size i = 0; i < dim; ++i) {
    vector1[i] = vector1[i] - vector2[i];
  }
}

/**
 * @brief Makes a unit vector
 * @param vector The vector to transform
 * @param nbElements The number of elements in the vector
 */
void makeUnitVector(matrix_real_number* vector, matrix_size nbElements) {
  matrix_real_number norm = computeNorm(vector, nbElements);
  vectorScale(vector, nbElements, 1.0 / norm);
}

/**
 * @brief Compute the norm of the vector
 * @param vector the vector to use
 * @param length the number of elements in the vector
 */
matrix_real_number computeNorm(matrix_real_number* vector, matrix_size length) {
  matrix_real_number tmp = 0;
  for (matrix_size i = 0; i < length; ++i) {
    tmp += vector[i] * vector[i];
  }
  return sqrt(tmp);
}
