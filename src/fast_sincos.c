#include <stdio.h>
#include <math.h>
#include "fast_sincos.h"


static fast_sincos_real fastSinChebyshev(const fast_sincos_real angleRadians, const int sinDegree);
static fast_sincos_real fastCosChebyshev(const fast_sincos_real angleRadians, const int sinDegree);
static fast_sincos_real lookupCosInterpolate(const fast_sincos_real angleRadians);
static fast_sincos_real lookupSinInterpolate(const fast_sincos_real angleRadians);
static fast_sincos_real lookupCos(const fast_sincos_real angleRadians);
static fast_sincos_real lookupSin(const fast_sincos_real angleRadians);

static inline double scaleValueToRadians16(const uint16_t value);
static inline double scaleValueToRadians32(const uint32_t value);


void test() {

    fast_sincos_real angle = -20;
    
    printf("SIN\n");
    printf("actual = %.20f\n", sin(angle));
    printf("approx = %.20f\n", fastSin(angle, 3));
    printf("look = %.20f\n", lookupSinInterpolate(angle));
    printf("look = %.20f\n", lookupSin(angle));
    printf("\n");

    printf("COS\n");
    printf("actual = %.20f\n", cos(angle));
    printf("approx = %.20f\n", fastCos(angle, 3));
    printf("look = %.20f\n", lookupCosInterpolate(angle));
    printf("look = %.20f\n", lookupCos(angle));

}

/**
 * @brief Fast sine computation.
 * With degrees over 2, the Chebyshev approximation is used to compute the sine of the angle.
 * With degree = 2, the sine is computed with a lookup sine table and interpolation.
 * With degree = 1, the sine is computed with a lookup sine table only.
 * @param angleRadians The angle, in radians.
 * @param degree The degree of the approximation. 
 * Higher is more accurate, but slower.
 * @return A sine approximation of the angle.
*/
fast_sincos_real fastSin(const fast_sincos_real angleRadians, const int degree) {

    // Check for lower degrees first
    switch (degree) {
      case 2:
        return lookupSinInterpolate(angleRadians);
      case 1:
        return lookupSin(angleRadians);
      default:
        break;
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

    // Chose the appropriate approximation depending on the
    // accepted range.
    fast_sincos_real returnedValue;
    if(clampedAngle < FAST_PI_DIV_4) {
        returnedValue = fastSinChebyshev(clampedAngle, degree);
    } else {
        returnedValue = fastCosChebyshev(FAST_PI_DIV_2 - clampedAngle, degree);
    }

    return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * @brief Fast cosine computation.
 * With degrees over 2, the Chebyshev approximation is used to compute the cosine of the angle.
 * With degree = 2, the cosine is computed with a lookup sine table and interpolation.
 * With degree = 1, the cosine is computed with a lookup sine table only.
 * @param angleRadians The angle, in radians
 * @param degree The degree of the approximation. 
 * Higher is more accurate, but slower.
 * @return A cosine approximation of the angle.
*/
fast_sincos_real fastCos(const fast_sincos_real angleRadians, const int degree) {
    
    // Check for lower degrees first
    switch (degree) {
      case 2:
        return lookupCosInterpolate(angleRadians);
      case 1:
        return lookupCos(angleRadians);
      default:
        break;
    }

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

    // Chose the appropriate approximation depending on the
    // accepted range.
    fast_sincos_real returnedValue;
    if(clampedAngle < FAST_PI_DIV_4) {
        returnedValue = fastCosChebyshev(clampedAngle, degree);
    } else {
        returnedValue = fastSinChebyshev(FAST_PI_DIV_2 - clampedAngle, degree);
    }

    return negativeFactor ? -returnedValue : returnedValue;
}


/**
 * The Chebyshev approximation is used to compute the sine of the angle.
 * Coefficients were obtained manually with the Remez algorithm.
 * Horner's method (nested multiplication) is then used for faster calculations.
 * The range for sin is [-PI / 4, PI / 4].
 * This range was chosen as some of the coefficients become negligible,
 * reducing the total number of multiplications.
 * @param angleRadians The angle, in radians, within the range [-PI / 4, PI / 4].
 * @param sinDegree The degree of the Chebyshev approximation for sin. 
 * Higher is more accurate, but slower.
 * @return A sine approximation of the angle.
*/
static fast_sincos_real fastSinChebyshev(const fast_sincos_real angleRadians, const int sinDegree) {
    fast_sincos_real angleRadiansSquared = angleRadians * angleRadians;
    switch (sinDegree) {
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
 * The Chebyshev approximation is used to compute the cosine of the angle.
 * Coefficients were obtained manually with the Remez algorithm.
 * Horner's method (nested multiplication) is then used for faster calculations.
 * The range for cos is [-PI / 4, PI / 4].
 * This range was chosen as some of the coefficients become negligible,
 * reducing the total number of multiplications.
 * @param angleRadians The angle, in radians, within the range [-PI / 4, PI / 4].
 * @param sinDegree The degree of the Chebyshev approximation for sin.
 * Because this approximation is for cos, the actual degree of the approximation
 * will be "sinDegree + 1".
 * Higher is more accurate, but slower.
 * @return A cosine approximation of the angle.
*/
static fast_sincos_real fastCosChebyshev(const fast_sincos_real angleRadians, const int sinDegree) {
    fast_sincos_real angleRadiansSquared = angleRadians * angleRadians;
    switch (sinDegree) {
        case 7: // Actually computes for degree 8
            return (((
                2.4379929375956876e-5
                *angleRadiansSquared-1.3886619210252882e-3)
                *angleRadiansSquared+4.1666616739207635e-2)
                *angleRadiansSquared-4.9999999615433476e-1)
                *angleRadiansSquared+9.9999999995260044e-1;
        case 5: // Actually computes for degree 6
        default: 
            return ((
                -1.3585908510113299e-3
                *angleRadiansSquared+4.1655026884251524e-2)
                *angleRadiansSquared-4.9999856695848848e-1)
                *angleRadiansSquared+9.9999997242332292e-1;
        
        case 3: // Actually computes for degree 4
            return (
                4.0398535966168857e-2
                *angleRadiansSquared-4.9970814035466399e-1)
                *angleRadiansSquared+9.9999003495519596e-1;
    }
}

/**
 * @brief Fast sine computation with lookup table, using interpolation
 * @param angleRadians The angle, in radians.
 * @return A sine approximation of the angle.
*/
static fast_sincos_real lookupSinInterpolate(const fast_sincos_real angleRadians) {
  
  fast_sincos_real scaledAngle = angleRadians * LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    negativeFactor = 1;
    scaledAngle = -scaledAngle;
  }

  long roundedAngle = scaledAngle;
  // Get the remainder in fixed point, for faster calculations
  // The function below allows for fast floating point multiplication by power of 2
  uint16_t remainder = ldexpf(scaledAngle - roundedAngle, LOOKUP_REMAINDER_BITS); 
  uint16_t index = roundedAngle & LOOKUP_INDEX_MASK;  // This & replaces modulus

  // Isolate the angle in the first quandrant
  if (index >= QUADRANT_SIZE_2) {
    index -= QUADRANT_SIZE_2;
    negativeFactor ^= 1;
  }

  if (index >= QUADRANT_SIZE) {
    index = QUADRANT_SIZE_2 - index;

    if (remainder) {
      remainder = LOOKUP_REMAINDER_SIZE - remainder;
      --index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint32_t currentValue = sineTable[index]; 
  if (remainder > 0) {
    currentValue = currentValue + (((sineTable[index + 1] - currentValue) * remainder) >> LOOKUP_REMAINDER_BITS);
  }

  fast_sincos_real returnedValue = scaleValueToRadians32(currentValue);//ldexpf(currentValue, LOOKUP_ELEMENTS_BITS_NEGATIVE);//currentValue * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * @brief Fast cosine computation with lookup table, using interpolation
 * @param angleRadians The angle, in radians.
 * @return A cosine approximation of the angle.
*/
static fast_sincos_real lookupCosInterpolate(const fast_sincos_real angleRadians) {
  fast_sincos_real scaledAngle = angleRadians * LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    scaledAngle = -scaledAngle;
  }

  long roundedAngle = scaledAngle;
  // Get the remainder in fixed point, for faster calculations
  // The function below allows for fast floating point multiplication by power of 2
  uint16_t remainder = ldexpf(scaledAngle - roundedAngle, LOOKUP_REMAINDER_BITS); 
  uint16_t index = roundedAngle & LOOKUP_INDEX_MASK;  // This & replaces modulus

  // Isolate the angle in the first quandrant
  if (index >= QUADRANT_SIZE_2) {
    index -= QUADRANT_SIZE_2;
    negativeFactor ^= 1;
  }

  if (index >= QUADRANT_SIZE) {
    index = QUADRANT_SIZE_2 - index;
    negativeFactor ^= 1;
    if (remainder) {
      remainder = -remainder;
      --index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint32_t currentValue;
  if (remainder > 0) 
  {
    currentValue = sineTable[QUADRANT_SIZE_MINUS_1 - index]; 
    remainder = LOOKUP_REMAINDER_SIZE - remainder;
    currentValue = currentValue + (((sineTable[QUADRANT_SIZE - index] - currentValue) * remainder) >> LOOKUP_REMAINDER_BITS);
  } else {
    currentValue = sineTable[QUADRANT_SIZE - index]; 
  }
  
  fast_sincos_real returnedValue =  scaleValueToRadians32(currentValue);//ldexpf(currentValue, LOOKUP_ELEMENTS_BITS_NEGATIVE);//currentValue * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}

static fast_sincos_real lookupSin(const fast_sincos_real angleRadians) {
  
  fast_sincos_real scaledAngle = angleRadians * LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    negativeFactor = 1;
    scaledAngle = -scaledAngle;
  }

  // basic rounding
  long roundedAngle = lroundf(scaledAngle);
  uint16_t index = roundedAngle & (LOOKUP_INDEX_MASK);  // This & replaces fmod

  // Isolate the angle in the first quandrant
  if (index >= QUADRANT_SIZE_2) {
    index -= QUADRANT_SIZE_2;
    negativeFactor ^= 1;
  }

  if (index >= QUADRANT_SIZE) {
    index = QUADRANT_SIZE_2 - index;
  }

  fast_sincos_real returnedValue = scaleValueToRadians16(sineTable[index]); //ldexpf(sineTable[index], LOOKUP_ELEMENTS_BITS_NEGATIVE);//sineTable[index] * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}

static fast_sincos_real lookupCos(const fast_sincos_real angleRadians) {
  fast_sincos_real scaledAngle = angleRadians * LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    scaledAngle = -scaledAngle;
  }

  // basic rounding
  long roundedAngle = lroundf(scaledAngle);
  uint16_t index = roundedAngle & (LOOKUP_INDEX_MASK);  // This & replaces fmod

  // Isolate the angle in the first quandrant
  if (index >= QUADRANT_SIZE_2) {
    index -= QUADRANT_SIZE_2;
    negativeFactor ^= 1;
  }

  if (index >= QUADRANT_SIZE) {
    index = QUADRANT_SIZE_2 - index;
    negativeFactor ^= 1;
  }

  fast_sincos_real returnedValue = scaleValueToRadians16(sineTable[QUADRANT_SIZE - index]); //ldexpf(sineTable[QUADRANT_SIZE - index] , LOOKUP_ELEMENTS_BITS_NEGATIVE);//sineTable[QUADRANT_SIZE - index] * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}

/** 
 * @brief Converts a value obtained from a lookup table to radians
 * Divide the current value by the maximum value of the elements
 * in the lookup table, plus 1,
 * which is (2^LOOKUP_ELEMENTS_BITS_NEGATIVE).
 * The Actual division should be by the real maximum value of the 
 * elements in the lookup table, but it is faster to
 * divide by a power of 2. 
 * The error is slightly increased because of this choice
 * e.g.: Dividing by 65536 instead of 65535
 * @param value The value to scale.
 * @return The scaled value, as a real number.
 */ 
static inline fast_sincos_real scaleValueToRadians16(const uint16_t value) {
  return ldexpf(value, LOOKUP_ELEMENTS_BITS_NEGATIVE);
}

static inline fast_sincos_real scaleValueToRadians32(const uint32_t value) {
  return ldexpf(value, LOOKUP_ELEMENTS_BITS_NEGATIVE);
}
