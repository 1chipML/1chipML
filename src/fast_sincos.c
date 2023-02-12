#include <stdio.h>
#include <math.h>
#include "fast_sincos.h"
//#include <time.h>

static fast_sincos_real fastSinChebyshev(const fast_sincos_real angleRadians, const int degree);
static fast_sincos_real fastCosChebyshev(const fast_sincos_real angleRadians, const int degree);

void test() {

    //clock_t begin, end;
    //begin = clock();
    //end = clock();
    //printf("time approx = %.10f\n", (double)(end - begin) / CLOCKS_PER_SEC);
    //begin = clock();
    //end = clock();
    //printf("time actual = %.10f\n", (double)(end - begin) / CLOCKS_PER_SEC);

    fast_sincos_real approx = fastSin(0.5, 3);
    fast_sincos_real actual = sin(0.5);
    printf("approx = %.20f\n", approx);
    printf("actual = %.20f\n", actual);

}

/**
 * The Chebyshev approximation is used to compute the sine of the angle
 * @param angleRadians The angle, in radians
 * @param degree The degree of the Chebyshev approximation. 
 * Higher is more accurate, but slower
 * @return A sine approximation of the angle
*/
fast_sincos_real fastSin(const fast_sincos_real angleRadians, const int degree) {

    // Isolate the angle in the first quandrant
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

    fast_sincos_real returnedValue;
    if(clampedAngle < FAST_PI_DIV_4) {
        returnedValue = fastSinChebyshev(clampedAngle, degree);
    } else {
        returnedValue = fastCosChebyshev(FAST_PI_DIV_2 - clampedAngle, degree);
    }

    return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * The Chebyshev approximation is used to compute the cosine of the angle
 * @param angleRadians The angle, in radians
 * @param degree The degree of the Chebyshev approximation. 
 * Higher is more accurate, but slower
 * @return A cosine approximation of the angle
*/
fast_sincos_real fastCos(const fast_sincos_real angleRadians, const int degree) {
    
    // Isolate the angle in the first quandrant
    int negativeFactor = 0;
    fast_sincos_real clampedAngle = angleRadians;
    if (clampedAngle < 0) {
        clampedAngle = -clampedAngle;
    }
    
    clampedAngle = fmod(clampedAngle, FAST_PI_TIMES_2);
    
    if (clampedAngle >= FAST_PI) {
        clampedAngle -= FAST_PI;
        negativeFactor ^= 1;
    }

    if(clampedAngle >= FAST_PI_DIV_2) {
        clampedAngle = FAST_PI - clampedAngle;
        negativeFactor ^= 1;
    }

    //printf("clamped = %.5f\n", (double)negativeFactor * clampedAngle);

    fast_sincos_real returnedValue;
    if(clampedAngle < FAST_PI_DIV_4) {
        returnedValue = fastCosChebyshev(clampedAngle, degree);
    } else {
        returnedValue = fastSinChebyshev(FAST_PI_DIV_2 - clampedAngle, degree);
    }

    return negativeFactor ? -returnedValue : returnedValue;
}


/**
 * The Chebyshev approximation is used to compute the sine of the angle
 * Coefficients were obtained manually with the Remez algorithm
 * Horner's method (nested multiplication) is then used for faster calculations
 * The range for sin is [-PI / 4, PI / 4]
 * @param angleRadians The angle, in radians, within the range [-PI / 4, PI / 4]
 * @param degree The degree of the Chebyshev approximation. 
 * Higher is more accurate, but slower
 * @return A sine approximation of the angle
*/
static fast_sincos_real fastSinChebyshev(const fast_sincos_real angleRadians, const int degree) {
    //printf("sin Internal\n");
    fast_sincos_real angleRadiansSquared = angleRadians * angleRadians;
    switch (degree) {
        case 7:
            return (((
                -1.9462116998273101e-4
                * angleRadiansSquared +8.3315846064878458e-3)
                * angleRadiansSquared -1.6666636754299513e-1)
                * angleRadiansSquared +9.9999998617934201e-1)
                * angleRadians;
        case 5:
        default: // 5
            return ((
                8.1215579245991201e-3
                *angleRadiansSquared-1.6660161988228715e-1)
                *angleRadiansSquared+9.9999499756161918e-1)
                *angleRadians;
        case 3:
            return (
                -1.6034401672287444e-1
                *angleRadiansSquared+9.9903142291243359e-1)
                *angleRadians-1.2156602888538742e-99;
    }
}

/**
 * The Chebyshev approximation is used to compute the cosine of the angle
 * Coefficients were obtained manually with the Remez algorithm
 * Horner's method (nested multiplication) is then used for faster calculations
 * The range for cos is [-PI / 4, PI / 4]
 * @param angleRadians The angle, in radians, within the range [-PI / 4, PI / 4]
 * @param degree The degree of the Chebyshev approximation. 
 * Higher is more accurate, but slower
 * @return A cosine approximation of the angle
*/
static fast_sincos_real fastCosChebyshev(const fast_sincos_real angleRadians, const int degree) {
    //printf("cos Internal\n");
    fast_sincos_real angleRadiansSquared = angleRadians * angleRadians;
    switch (degree) {
        case 7: // is 8 in reality
            return (((
                2.4379929375956876e-5
                *angleRadiansSquared-1.3886619210252882e-3)
                *angleRadiansSquared+4.1666616739207635e-2)
                *angleRadiansSquared-4.9999999615433476e-1)
                *angleRadiansSquared+9.9999999995260044e-1;
        case 5:
        default: // 5, is 6 in reality
            return ((
                -1.3585908510113299e-3
                *angleRadiansSquared+4.1655026884251524e-2)
                *angleRadiansSquared-4.9999856695848848e-1)
                *angleRadiansSquared+9.9999997242332292e-1;
        
        case 3: // is 4 in reality
            return (
                4.0398535966168857e-2
                *angleRadiansSquared-4.9970814035466399e-1)
                *angleRadiansSquared+9.9999003495519596e-1;
    }
}
