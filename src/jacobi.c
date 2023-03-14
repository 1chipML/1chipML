#include "jacobi.h"

/**
 * Converts a 2d coordinate in a square matrix to an index in a 2d array
*/
static inline vec_size coordToIndex(vec_size row, vec_size col, vec_size size) {
    return row * size + col;
}

static inline vec_size isDiagonal(vec_size index, vec_size size) {
    return ((index / size) == (index % size));
}

/**
 * Finds the index of the element with the highest absolute value. Matrix must be at least 2x2 since below that size, the concept of off-diagonal does not exist
 * @param matrix must be a square matrix
 * @param size size of the matrix
 * @param maxX pointer where the x coordinate of the maximum element will be stored
 * @param maxY pointer where the y coordinate of the maximum element will be stored 
*/
void jacobiMaxIndex(real_number* matrix, vec_size size, vec_size* maxRow, vec_size* maxCol) {

    real_number maxElement = matrix[1]; // Cannot be first element since it's on the diagonal
    vec_size currentMaxRow = 0;
    vec_size currentMaxCol = 1;

    // Multiplication could overflow if vec_size is not defined properly
    vec_size sizeSquared = size * size;

    // Iteration starts at 2 since we assume that the first element off diagonal is the max at that point
    for (vec_size i = 2; i < sizeSquared; ++i) {
        real_number elem = fabs(matrix[i]);

        if (elem > maxElement) {
            vec_size currentRow = i / size;
            vec_size currentCol = i % size;
            if (currentRow != currentCol) {
                currentMaxRow = currentRow;
                currentMaxCol = currentCol;
                maxElement = elem;
            }
        }
    }

    *maxRow = currentMaxRow;
    *maxCol = currentMaxCol;
}


/**
 * Multiplies 2 matrices together using this equation : output = firstMatrix x secondMatrix
 * @param firstMatrix first matrix to multiply of size m * n
 * @param secondMatrix second matrix to multiply of size n * p
 * @param size array of size 3 containing m, n and p
 * @param output matrix resulting of the matrix multiplication of the given matrices
 * @param transposeFirstMatrix whether to transpose the first matrix when multiplying the matrices
 */
void jacobiMatrixMultiply(real_number* firstMatrix, real_number* secondMatrix, vec_size size[3], real_number* output, vec_size transposeFirstMatrix) {
    
    vec_size m = size[0];
    vec_size n = size[1];
    vec_size p = size[2];
    for (vec_size i = 0; i < m; ++i) {
        for (vec_size j = 0; j < p; ++j) {
            real_number sum = 0.0;
            for (register vec_size k = 0; k < n; ++k) {
                sum += (transposeFirstMatrix ? firstMatrix[coordToIndex(k, i, n)] : firstMatrix[coordToIndex(i, k, n)]) * secondMatrix[coordToIndex(k, j, n)];
            }
            output[coordToIndex(i, j, p)] = sum;
        }
    }
}

/**
 * Creates a jacobi rotation matrix (1 on the diagonal and the value of cos(angle) and sin(angle) at row p and q)
 * @param row row where to put the c and s values
 * @param col column where to put the c and s values
 * @param output output matrix (must be square)
 * @param size size of the output matrix
*/
void jacobiCreateRotationMatrix(real_number* input, vec_size row, vec_size col, real_number* output, vec_size size) {

    real_number aqq = input[coordToIndex(col, col, size)];
    real_number app = input[coordToIndex(row, row, size)];
    real_number apq = input[coordToIndex(row, col, size)];

    real_number tau = (aqq - app) / (2.0 * apq);
    real_number t1 = -tau - sqrt(1 + tau * tau);
    real_number t2 = -tau + sqrt(1 + tau * tau);
    real_number t = fabs(t1) > fabs(t2) ? t1 : t2;

    real_number c = 1.0 / (sqrt(1 + t * t));
    real_number s = c * t;

    vec_size cIndex1 = coordToIndex(row, row, size);
    vec_size cIndex2 = coordToIndex(col, col, size);
    vec_size sIndex1 = coordToIndex(row, col, size);
    vec_size sIndex2 = coordToIndex(col, row, size);
    vec_size sizeSquared = size * size;

    for (vec_size i = 0; i < sizeSquared; ++i) {
        if (i == cIndex1 || i == cIndex2) {  
            output[i] = c;
        } else if (i == sIndex1) {
            output[i] = s;
        } else if (i == sIndex2) {
            output[i] = -s;
        } else if (isDiagonal(i, size)) {
            output[i] = 1;
        } else {
            output[i] = 0; // Off diagonal
        }
    }
}


/**
 * Creates the identity matrix (1 on the diagonal and 0 on other coordinates)
 * @param size size of the matrix
 * @param output output matrix
*/
void jacobiCreateIdentityMatrix(vec_size size, real_number* output) {
    vec_size sizeSquared = size * size;
    for (vec_size i = 0; i < sizeSquared; ++i) {
        if (isDiagonal(i, size)) {
            output[i] = 1;
        } else {
            output[i] = 0;
        }
    }
}

/**
 * Computes the off diagonal absolute sum of a matrix (sums every element not in the diagonal)
 * @param matrix matrix to compute the sum on
 * @param size size of matrix
*/
real_number jacobiComputeOffDiagonalSum(real_number* matrix, vec_size size) {
    real_number sum = 0;
    vec_size sizeSquared = size * size;
    
    for (vec_size i = 0; i < sizeSquared; ++i) {
        if (!isDiagonal(i, size)) {
            sum += fabs(matrix[i]);
        }
    }
    return sum;
}

/**
 * Computes the jacobi method to find the eigenvalues and eigenvectors of the input matrix
 * @param inputMatrix matrix whose eigenvalues and eigenvectors we are looking for (will contain the eigenvalues after this function has been called)
 * @param size size of the matrix (matrix must be square)
 * @param outputMatrix matrix containing every eigenvector of the input matrix (same size as the input matrix)
 * @param max_iterations max number of iterations the algorithm can do (set to -1 to allow any amount of iterations)
 * @param cyclic whether to use the cyclic jacobi method or the classical version
*/
void jacobi(real_number* inputMatrix, vec_size size, real_number* outputMatrix, vec_size max_iterations, vec_size cyclic) {

    vec_size sizes3d[3] = {size, size, size};
    vec_size sweepSize = (size * (size - 1) >> 1); // A sweep is defined as a n * (n - 1) / 2 jacobi rotations
    real_number rotationMatrix[size * size];
    real_number vBuffer[size * size];
    real_number aBuffer[size * size];
    vec_size maxRow, maxCol, indexToMinimize = 0;
    vec_size sizeSquared = size * size;

    jacobiCreateIdentityMatrix(size, outputMatrix);
    real_number currentOffDiagonalSum = jacobiComputeOffDiagonalSum(inputMatrix, size);
    vec_size allowInfiniteIterations = max_iterations == -1;
    for (vec_size i = 0; (allowInfiniteIterations || i < max_iterations) && currentOffDiagonalSum > EPSILON; ++i) {
        if (cyclic) {
              if (indexToMinimize == (sizeSquared - 2)) {
                  indexToMinimize = 0;
              }

              indexToMinimize = (indexToMinimize + 1) % (sizeSquared);
              if (isDiagonal(indexToMinimize, size)) {
                  indexToMinimize = (indexToMinimize + 1) % (sizeSquared);
              }

              maxRow = indexToMinimize / size;
              maxCol = indexToMinimize % size;

        } else {
            jacobiMaxIndex(inputMatrix, size, &maxRow, &maxCol);
        }

        if (i < 3 * sweepSize && i != 0) { // The number 3 is proposed in the book Numerical Recipes
            // In the first three sweeps, we keep the rotation matrix if |a_pq| > epsilon
            // The constant 0.20 is proposed in the book Numerical Recipes
            real_number epsilon_sweep = 0.20 * (currentOffDiagonalSum) / ((real_number) (sizeSquared));

            if (fabs(inputMatrix[coordToIndex(maxRow, maxCol, size)]) > epsilon_sweep) {
                jacobiCreateRotationMatrix(inputMatrix, maxRow, maxCol, rotationMatrix, size);
            }
        } else {

            real_number apq = fabs(inputMatrix[coordToIndex(maxRow, maxCol, size)]);
            real_number app = inputMatrix[coordToIndex(maxRow, maxRow, size)];
            real_number aqq = inputMatrix[coordToIndex(maxCol, maxCol, size)];

            // If |apq| << |app| and |apq << |aqq|, set the element to 0 and continue with other element
            real_number precision = pow(10, -(DIGITS_PRECISION + 2));
            if (apq < precision * app && apq < precision * aqq) {
                inputMatrix[coordToIndex(maxRow, maxCol, size)] = 0;
                i--;
                continue;
            }

            jacobiCreateRotationMatrix(inputMatrix, maxRow, maxCol, rotationMatrix, size);
        }

        jacobiMatrixMultiply(rotationMatrix, inputMatrix, sizes3d, aBuffer, 1);
        jacobiMatrixMultiply(aBuffer, rotationMatrix, sizes3d, inputMatrix, 0);
        jacobiMatrixMultiply(outputMatrix, rotationMatrix, sizes3d, vBuffer, 0);
        memcpy(outputMatrix, vBuffer, sizeof(real_number) * sizeSquared);
        currentOffDiagonalSum = jacobiComputeOffDiagonalSum(inputMatrix, size);
    }

}
