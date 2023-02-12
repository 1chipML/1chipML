#include <stdio.h>
#include <math.h>
#include "fast_sincos.h"
//#include <time.h>

static fast_sincos_real fastSinInternal(fast_sincos_real angleRadians);
static fast_sincos_real fastCosInternal(fast_sincos_real angleRadians);

void test() {

    //clock_t begin, end;
    //begin = clock();
    //end = clock();
    //printf("time approx = %.10f\n", (double)(end - begin) / CLOCKS_PER_SEC);
    //begin = clock();
    //end = clock();
    //printf("time actual = %.10f\n", (double)(end - begin) / CLOCKS_PER_SEC);

    fast_sincos_real approx = fastSin(20);
    fast_sincos_real actual = sin(20);
    printf("approx = %.20f\n", approx);
    printf("actual = %.20f\n", actual);

}

/**
 * The Chebyshev approximation is used to compute the sine of the angle
 * @param angleRadians The angle, in radians
 * @return A sine approximation of the angle
*/
fast_sincos_real fastSin(fast_sincos_real angleRadians) {

    int negativeFactor = 0;
    fast_sincos_real clampedAngle = angleRadians;
    if (clampedAngle < 0) {
        negativeFactor = 1;
        clampedAngle = -clampedAngle;
    }
    
    clampedAngle = fmod(clampedAngle, FAST_PI_TIMES_2);
    
    if (clampedAngle >= FAST_PI) {
        clampedAngle -= FAST_PI;
        negativeFactor ^= 1;
    }

    if(clampedAngle >= FAST_PI_DIV_2) {
        clampedAngle = FAST_PI - clampedAngle;
    }

    //printf("clamped = %.5f\n", (double)negativeFactor * clampedAngle);

    fast_sincos_real returnedValue;
    if(clampedAngle < FAST_PI_DIV_4) {
        returnedValue = fastSinInternal(clampedAngle);
    } else {
        returnedValue = fastCosInternal(FAST_PI_DIV_2 - clampedAngle);
    }

    return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * The Chebyshev approximation is used to compute the cosine of the angle
 * @param angleRadians The angle, in radians
 * @return A cosine approximation of the angle
*/
fast_sincos_real fastCos(fast_sincos_real angleRadians) {
    return fastSin(FAST_PI_DIV_2 - angleRadians);
}


/**
 * The Chebyshev approximation is used to compute the sine of the angle
 * Coefficients were obtained manually with the Remez algorithm
 * Horner's method (nested multiplication) is then used for faster calculations
 * The range for sin is [-PI / 4, PI / 4]
 * @param angleRadians The angle, in radians, within the range [-PI / 4, PI / 4]
 * @return A sine approximation of the angle
*/
static fast_sincos_real fastSinInternal(fast_sincos_real angleRadians) {
    //printf("sin Internal\n");
    fast_sincos_real angleRadiansSquared = angleRadians * angleRadians;
    return (((
        -1.9462116998273101e-4
        *angleRadiansSquared+8.3315846064878458e-3)
        *angleRadiansSquared-1.6666636754299513e-1)
        *angleRadiansSquared+9.9999998617934201e-1)
        *angleRadians;
}

/**
 * The Chebyshev approximation is used to compute the cosine of the angle
 * Coefficients were obtained manually with the Remez algorithm
 * Horner's method (nested multiplication) is then used for faster calculations
 * The range for cos is [-PI / 4, PI / 4]
 * @param angleRadians The angle, in radians, within the range [-PI / 4, PI / 4]
 * @return A cosine approximation of the angle
*/
static fast_sincos_real fastCosInternal(fast_sincos_real angleRadians) {
    //printf("cos Internal\n");
    fast_sincos_real angleRadiansSquared = angleRadians * angleRadians;
    return (((
        2.4379929375956876e-5
        *angleRadians-1.3886619210252882e-3)
        *angleRadiansSquared+4.1666616739207635e-2)
        *angleRadiansSquared-4.9999999615433476e-1)
        *angleRadiansSquared+9.9999999995260044e-1;
}
