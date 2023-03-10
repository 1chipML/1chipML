#include <stdint.h>

typedef double lanczos_real;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @input Matrix is the matrix for which you can to calculate the eigen values
 * and eigen vectors
 * @input dim is the size of the hermitian matrix
 * @input nbIter is the number of iteration of the algorithm
 * @input beginVector is a vector used for the first iteration of the algorithm.
 * This parameter can be NULL
 * @output tMatrix is the matrix T which can be used to compute eigen values and
 * eigen vectors
 * @output vMatrix is the matrix V which can be used to transforme eigen vectors
 * from space of T to space or matrix
 *
 */
void lanczos(lanczos_real* matrix, uint_least8_t dim, uint_least8_t nbIter,
             lanczos_real* initialVector, lanczos_real* tMatrix,
             lanczos_real* vMatrix);
void vectorScale(lanczos_real* vector, uint_least8_t nbElements,
                 lanczos_real scale);
void vectorSubstract(lanczos_real* vector1, lanczos_real* vector2,
                     uint_least8_t dim);
void matrixMultiply(lanczos_real* firstMatrix, lanczos_real* secondMatrix,
                    uint_least8_t size[3], lanczos_real* output);
void makeUnitVector(lanczos_real* vector, uint_least8_t nbElements);
void transpose(lanczos_real* input, lanczos_real* output,
                      uint_least8_t* dims);
#ifdef __cplusplus
}
#endif
