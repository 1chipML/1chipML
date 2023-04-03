#ifndef MATRIX_H
#define MATRIX_H

typedef unsigned int matrix_size;
typedef double matrix_real_number;

#ifdef __cplusplus
extern "C" {
#endif

matrix_size coordToIndex(const matrix_size row, const matrix_size col,
                         const matrix_size size);
matrix_size isDiagonal(const matrix_size index, const matrix_size size);
void matrixMultiply(const matrix_real_number* firstMatrix,
                    const matrix_real_number* secondMatrix,
                    const matrix_size size[3], matrix_real_number* output,
                    const matrix_size transposeFirstMatrix);
void createIdentityMatrix(const matrix_size size, matrix_real_number* output);
void vectorScale(matrix_real_number* vector, matrix_size nbElements,
                 matrix_real_number scale);
void vectorSubstract(matrix_real_number* vector1, matrix_real_number* vector2,
                     matrix_size dim);
void makeUnitVector(matrix_real_number* vector, matrix_size nbElements);
matrix_real_number computeNorm(matrix_real_number* vector, matrix_size length);
void matrixTranspose(matrix_real_number* input, matrix_real_number* output,
                     matrix_size* dims);

#ifdef __cplusplus
}
#endif

#endif
