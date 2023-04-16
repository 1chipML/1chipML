#ifndef LU_DECOMPOSITION_H
#define LU_DECOMPOSITION_H

typedef double lu_real;

#ifdef __cplusplus
extern "C" {
#endif

void LUDecomposition(const lu_real* initialMatrix, lu_real* lMatrix,
                     lu_real* uMatrix, const int size);

#ifdef __cplusplus
}
#endif

#endif
