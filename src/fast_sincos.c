#include <stdio.h>
#include <math.h>
#include "fast_sincos.h"


static fast_sincos_real fastSinChebyshev(const fast_sincos_real angleRadians, const int sinDegree);
static fast_sincos_real fastCosChebyshev(const fast_sincos_real angleRadians, const int sinDegree);
static fast_sincos_real lookupCosInterpolate(const fast_sincos_real angleRadians);
static fast_sincos_real lookupSinInterpolate(const fast_sincos_real angleRadians);
static fast_sincos_real lookupCos(const fast_sincos_real angleRadians);
static fast_sincos_real lookupSin(const fast_sincos_real angleRadians);


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
        case 5:
        default: // Actually computes for degree 5
            return ((
                -1.3585908510113299e-3
                *angleRadiansSquared+4.1655026884251524e-2)
                *angleRadiansSquared-4.9999856695848848e-1)
                *angleRadiansSquared+9.9999997242332292e-1;
        
        case 3: // Actually computes for degree 3
            return (
                4.0398535966168857e-2
                *angleRadiansSquared-4.9970814035466399e-1)
                *angleRadiansSquared+9.9999003495519596e-1;
    }
}

// 20.371832715762602978 for 128 / 2pi
// 40.74366543152520595683424342 for 256 / 2pi
static fast_sincos_real lookupSinInterpolate(const fast_sincos_real angleRadians) {
  
  fast_sincos_real scaledAngle = angleRadians * FAST_LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    negativeFactor = 1;
    scaledAngle = -scaledAngle;
  }

  long roundedAngle = scaledAngle;  // This should be the _only_ line of FP code
  uint16_t remainder = (scaledAngle - roundedAngle) * 65536.0;

  // All following code is integer code.
  //scaled_x += number_of_entries/4 ; // If we are doing cosine

  unsigned index = roundedAngle & (128 * 4 - 1);  // This & replaces fmod

  if (index >= 256) {
    index -= 256;
    negativeFactor ^= 1;
  }

  if (index >= 128) {
    index = 256 - index;

    if (remainder) {
      remainder = 65536 - remainder; // equivalent to 65536 - remainder
      --index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint32_t currentValue = sineTable[index]; 
  if (remainder > 0) 
  {
    currentValue = currentValue + (((sineTable[index + 1] - currentValue) * remainder) >> 16);
  }

  fast_sincos_real returnedValue = currentValue * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}

static fast_sincos_real lookupCosInterpolate(const fast_sincos_real angleRadians) {
  //return lookupSinInterpolate(FAST_PI_DIV_2 - x);
  fast_sincos_real scaledAngle = angleRadians * FAST_LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    scaledAngle = -scaledAngle;
  }

  long roundedAngle = scaledAngle;  // This should be the _only_ line of FP code
  uint16_t remainder = (scaledAngle - roundedAngle) * 65536.0;

  unsigned index = roundedAngle & (128 * 4 - 1);  // This & replaces fmod

  if (index >= 256) {
    index -= 256;
    negativeFactor ^= 1;
  }

  if (index >= 128) {
    index = 256 - index;
    negativeFactor ^= 1;
    if (remainder) {
      remainder = -remainder; // equivalent to 65536 - remainder
      --index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint32_t currentValue;
  if (remainder > 0) 
  {
    currentValue = sineTable[127 - index]; 
    remainder = 65536 - remainder;
    currentValue = currentValue + (((sineTable[128 - index] - currentValue) * remainder) >> 16);
  } else {
    currentValue = sineTable[128 - index]; 
  }

  fast_sincos_real returnedValue = currentValue * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}

static fast_sincos_real lookupSin(const fast_sincos_real angleRadians) {
  
  fast_sincos_real scaledAngle = angleRadians * FAST_LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    negativeFactor = 1;
    scaledAngle = -scaledAngle;
  }

  long roundedAngle = lroundf(scaledAngle);  // This should be the _only_ line of FP code
  unsigned index = roundedAngle & (128 * 4 - 1);  // This & replaces fmod

  // All following code is integer code.
  //scaled_x += number_of_entries/4 ; // If we are doing cosine

  if (index >= 256) {
    index -= 256;
    negativeFactor ^= 1;
  }

  if (index >= 128) {
    index = 256 - index;
  }

  fast_sincos_real returnedValue = sineTable[index] * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}

static fast_sincos_real lookupCos(const fast_sincos_real angleRadians) {
  //return lookupSinInterpolate(FAST_PI_DIV_2 - x);
  fast_sincos_real scaledAngle = angleRadians * FAST_LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    scaledAngle = -scaledAngle;
  }

  long roundedAngle = lroundf(scaledAngle);  // This should be the _only_ line of FP code
  unsigned index = roundedAngle & (128 * 4 - 1);  // This & replaces fmod

  if (index >= 256) {
    index -= 256;
    negativeFactor ^= 1;
  }

  if (index >= 128) {
    index = 256 - index;
    negativeFactor ^= 1;
  }

  // extended for the multiplication that is about to occur and keep the precision
  fast_sincos_real returnedValue = sineTable[128 - index] * 1.525902189669642175e-5; // divide by 65535
  return negativeFactor ? -returnedValue : returnedValue;
}
