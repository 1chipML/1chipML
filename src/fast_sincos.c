#include <stdio.h>
#include <math.h>
#include "fast_sincos.h"
//#include <time.h>
#include <inttypes.h>

static fast_sincos_real fastSinChebyshev(const fast_sincos_real angleRadians, const int degree);
static fast_sincos_real fastCosChebyshev(const fast_sincos_real angleRadians, const int degree);


const int8_t lut1[128] = {
   0,   6,  12,  19,  25,  31,  37,  43,  49,  54,
  60,  65,  71,  76,  81,  85,  90,  94,  98, 102,
 106, 109, 112, 115, 117, 120, 122, 123, 125, 126,
 126, 127, 127, 127, 126, 126, 125, 123, 122, 120,
 117, 115, 112, 109, 106, 102,  98,  94,  90,  85,
  81,  76,  71,  65,  60,  54,  49,  43,  37,  31,
  25,  19,  12,   6,   0,  -6, -12, -19, -25, -31,
 -37, -43, -49, -54, -60, -65, -71, -76, -81, -85,
 -90, -94, -98,-102,-106,-109,-112,-115,-117,-120,
-122,-123,-125,-126,-126,-127,-127,-127,-126,-126,
-125,-123,-122,-120,-117,-115,-112,-109,-106,-102,
 -98, -94, -90, -85, -81, -76, -71, -65, -60, -54,
 -49, -43, -37, -31, -25, -19, -12,  -6 };

const uint8_t lut2[128] = {
  0,  3,  6,  9, 13, 16, 19, 22, 25, 28, 31, 34, 37,
 41, 44, 47, 50, 53, 56, 59, 62, 65, 68, 71, 74, 77,
 80, 83, 86, 89, 92, 95, 98,100,103,106,109,112,115,
117,120,123,126,128,131,134,136,139,142,144,147,149,
152,154,157,159,162,164,167,169,171,174,176,178,180,
183,185,187,189,191,193,195,197,199,201,203,205,207,
208,210,212,214,215,217,219,220,222,223,225,226,228,
229,231,232,233,234,236,237,238,239,240,241,242,243,
244,245,246,247,247,248,249,249,250,251,251,252,252,
253,253,253,254,254,254,255,255,255,255,255 };

const uint16_t lut[] = {
    0,  804, 1608, 2412, 3216, 4019, 4821, 5623,
 6424, 7223, 8022, 8820, 9616,10411,11204,11996,
12785,13573,14359,15142,15924,16703,17479,18253,
19024,19792,20557,21319,22078,22834,23586,24334,
25079,25820,26557,27291,28020,28745,29465,30181,
30893,31600,32302,32999,33692,34379,35061,35738,
36409,37075,37736,38390,39039,39682,40319,40950,
41575,42194,42806,43411,44011,44603,45189,45768,
46340,46905,47464,48014,48558,49095,49624,50145,
50659,51166,51664,52155,52638,53113,53580,54039,
54490,54933,55367,55794,56211,56620,57021,57413,
57797,58171,58537,58895,59243,59582,59913,60234,
60546,60850,61144,61429,61704,61970,62227,62475,
62713,62942,63161,63371,63571,63762,63943,64114,
64276,64428,64570,64703,64826,64939,65042,65136,
65219,65293,65357,65412,65456,65491,65515,65530,
65535
};


// 20.371832715762602978 for 128 / 2pi
// 40.74366543152520595683424342 for 256 / 2pi
static double lookupSin(const double x, const long shift) {
  
  double scaledAngle = x * 20.371832715762602978 * 4;
  int negative = 0;
  if (x < 0) {
    negative = 1;
    scaledAngle = -scaledAngle;
  }

  long roundedAngle = scaledAngle;  // This should be the _only_ line of FP code
  roundedAngle += shift; // we are doing cosine
  uint16_t remainder = (scaledAngle - roundedAngle) * 65536.0;

  // All following code is integer code.
  //scaled_x += number_of_entries/4 ; // If we are doing cosine

  unsigned index = roundedAngle & (128 * 4 - 1);  // This & replaces fmod

  if (index >= 256) {
    index -= 256;
    negative ^= 1;
  }

  if (index >= 128) {
    index = 256 - index;

    if (remainder) {
      remainder = ~remainder; // equivalent to 65536 - remainder
      --index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint32_t currentValue = lut[index]; 
  if (remainder > 0) 
  {
    currentValue = currentValue + (((lut[index + 1] - currentValue) * remainder) >> 16);
  }

  double returnedValue = currentValue * 1.525902189669642175e-5; // divide by 65535
  return negative ? -returnedValue : returnedValue;
}

static double lookupCos(const double x) {
  
  const long shift = 128;
  return lookupSin(x, shift);
}

double lookup_sin_alt1(const double x) {
  double scaledAngle = x * 20.371832715762602978;
  long roundedAngle = lround(scaledAngle);  // This should be the _only_ line of FP code
  // All following code is integer code.
  //scaled_x += number_of_entries/4 ; // If we are doing cosine
  //scaled_x += 64 + 32;
  //printf("scaled_x = %ld\n", scaled_x);
  unsigned index = roundedAngle & (128 - 1);  // Equivalent of "% 128" 

  double result = lut1[index];
  return result * 0.0078125;
}

void test() {

    //clock_t begin, end;
    //begin = clock();
    //end = clock();
    //printf("time approx = %.10f\n", (double)(end - begin) / CLOCKS_PER_SEC);
    //begin = clock();
    //end = clock();
    //printf("time actual = %.10f\n", (double)(end - begin) / CLOCKS_PER_SEC);

    double angle = -20;
    
    printf("SIN\n");
    printf("approx = %.20f\n", fastSin(angle, 3));
    printf("look = %.20f\n", lookupSin(angle, 0));
    printf("actual = %.20f\n", sin(angle));
    printf("\n");

    printf("COS\n");
    printf("approx = %.20f\n", fastCos(angle, 3));
    printf("look = %.20f\n", lookupCos(angle));
    printf("actual = %.20f\n", cos(angle));

}

/**
 * With degrees over 2, the Chebyshev approximation is used to compute the sine of the angle
 * @param angleRadians The angle, in radians
 * @param degree The degree of the Chebyshev approximation. 
 * Higher is more accurate, but slower
 * @return A sine approximation of the angle
*/
fast_sincos_real fastSin(const fast_sincos_real angleRadians, const int degree) {

    if (degree == 0) {
      return lookupSin(angleRadians, 0);
    }

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
