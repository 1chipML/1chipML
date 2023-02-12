#ifndef FAST_SINCOS_H
#define FAST_SINCOS_H

#ifndef fast_sincos_real
    #define fast_sincos_real double
#endif

#define FAST_PI 3.14159265358979323846 // pi
#define FAST_PI_DIV_2 1.57079632679489661923 // pi / 2
#define FAST_PI_DIV_4 0.78539816339744830962 // pi / 4
#define FAST_PI_TIMES_2 6.28318530717958647692 // pi * 2

#ifdef __cplusplus
extern "C" {
#endif

void test();
fast_sincos_real fastSin(const fast_sincos_real angleRadians, const int degree);
fast_sincos_real fastCos(const fast_sincos_real angleRadians, const int degree);

#ifdef __cplusplus
}
#endif

#endif // FAST_SINCOS
