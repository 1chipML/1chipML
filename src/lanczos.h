#include <stdint.h>

typedef double lanczos_real;

#ifdef __cplusplus
extern "C" {
#endif

void lanczos(lanczos_real* matrix, uint_least8_t dim, lanczos_real* tMatrix,
             lanczos_real* vMatrix, uint_least8_t nbIter);
void vectorScale(lanczos_real* vector, uint_least8_t nbElements,
                 lanczos_real scale);
void vectorSubstract(lanczos_real* vector1, lanczos_real* vector2,
                     uint_least8_t dim);
void matrixMultiply(lanczos_real* firstMatrix, lanczos_real* secondMatrix,
                    uint_least8_t size[3], lanczos_real* output);
void makeUnitVector(lanczos_real* vector, uint_least8_t nbElements);
#ifdef __cplusplus
}
#endif
