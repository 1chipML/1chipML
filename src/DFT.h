#ifndef DFT_H
#define DFT_H

#ifndef dft_real
#define dft_real double
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __cplusplus
extern "C" {
#endif

void DFT(const unsigned length, dft_real* realArray, dft_real* imaginaryArray,
         const int dir);

#ifdef __cplusplus
}
#endif

#endif // DFT_H
