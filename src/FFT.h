#ifndef FFT_H
#define FFT_H

#ifndef fft_real
#define fft_real double
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __cplusplus
extern "C" {
#endif

int FFT(const unsigned length, fft_real* realArray, fft_real* imaginaryArray,
        const int dir);

#ifdef __cplusplus
}
#endif

#endif // FFT_H
