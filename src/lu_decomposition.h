#ifndef LU_DECOMPOSITION_H
#define LU_DECOMPOSITION_H

typedef float lu_real;

#ifdef _cpluplus
extern "C" {
#endif

void LUDecomposition(const lu_real* initialMatrix, lu_real* lMatrix,
                     lu_real* uMatrix, const int size);

#ifdef _cpluplus
}
#endif

#endif
