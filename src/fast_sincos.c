#include <stdio.h>
#include <math.h>
#include "fast_sincos.h"
#include "utils.h"

static fast_sincos_real fastSinChebyshev(const fast_sincos_real angleRadians, const int sinDegree);
static fast_sincos_real fastCosChebyshev(const fast_sincos_real angleRadians, const int sinDegree);
static fast_sincos_real lookupCosInterpolate(const fast_sincos_real angleRadians);
static fast_sincos_real lookupSinInterpolate(const fast_sincos_real angleRadians);
static fast_sincos_real lookupCos(const fast_sincos_real angleRadians);
static fast_sincos_real lookupSin(const fast_sincos_real angleRadians);
static inline fast_sincos_real scaleValueToRadians(const fast_sincos_real value);


// Configure sine table management
#if defined  __has_attribute
  // Define table storage and access mode
  #if __has_attribute (__progmem__)
    #include <avr/pgmspace.h>
    #define ACCESS_TABLE(index) pgm_read_word_near(sineTable + index)
  #else
    #define PROGMEM
    #define ACCESS_TABLE(index) sineTable[index]
  #endif
#else
  #define PROGMEM
  #define ACCESS_TABLE(index) sineTable[index]
#endif

/**
 * Sine table used for sine and cosine approximation
 * This sine table only contains the first quadrant
 * The size of this table is "QUADRANT_SIZE + 1"
 * The "+ 1" allows for interpolation
 * 
 * It is recommended to put this table in the
 * program memory (flash) instead of the SRAM, as
 * it can take a lot of space.
 * With Arduino, this can be done by using the PROGMEM keyword
 * Using PROGMEM requires special methods to access the data,
 * such as pgm_read_word_near
 * 
 * Below is the definition of the table to instantiate
 * in the program memory
*/
static const uint16_t sineTable[129] PROGMEM = {
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

/**
 * @brief Fast sine computation.
 * With degrees of 3, 5 and 7, the Chebyshev approximation is used to compute the sine of the angle.
 * With degree = 2, the sine is computed with a lookup sine table and interpolation.
 * With degree = 1, the sine is computed with a lookup sine table only.
 * Invalid degrees will default to 7.
 * @param angleRadians The angle, in radians.
 * @param degree The degree of the approximation. 
 * Higher is more accurate, but slower.
 * @return A sine approximation of the angle.
*/
fast_sincos_real fastSin(const fast_sincos_real angleRadians, const int degree) {

    // Check for lower degrees first
    switch (degree) {
      case 1:
        return lookupSin(angleRadians);
      case 2:
        return lookupSinInterpolate(angleRadians);
    }

    // Isolate the angle in the first quandrant
    int negativeFactor = 0;
    fast_sincos_real clampedAngle = angleRadians;
    if (clampedAngle < 0) {
        negativeFactor = 1;
        clampedAngle = -clampedAngle;
    }
    
    clampedAngle = fmod(clampedAngle, PI_TIMES_2);
    
    if (clampedAngle >= M_PI) {
        clampedAngle -= M_PI;
        negativeFactor ^= 1;
    }

    if(clampedAngle >= M_PI_2) {
        clampedAngle = M_PI - clampedAngle;
    }

    // Chose the appropriate approximation depending on the
    // accepted range.
    fast_sincos_real returnedValue;
    if(clampedAngle < M_PI_4) {
        returnedValue = fastSinChebyshev(clampedAngle, degree);
    } else {
        returnedValue = fastCosChebyshev(M_PI_2 - clampedAngle, degree);
    }

    return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * @brief Fast cosine computation.
 * With degrees of 3, 5 and 7, the Chebyshev approximation is used to compute the cosine of the angle.
 * With degree = 2, the cosine is computed with a lookup sine table and interpolation.
 * With degree = 1, the cosine is computed with a lookup sine table only.
 * Invalid degrees will default to 7.
 * @param angleRadians The angle, in radians
 * @param degree The degree of the approximation. 
 * Higher is more accurate, but slower.
 * @return A cosine approximation of the angle.
*/
fast_sincos_real fastCos(const fast_sincos_real angleRadians, const int degree) {
    
    // Check for lower degrees first
    switch (degree) {
      case 1:
        return lookupCos(angleRadians);
      case 2:
        return lookupCosInterpolate(angleRadians);
    }

    // Isolate the angle in the first quandrant
    int negativeFactor = 0;
    fast_sincos_real clampedAngle = angleRadians;
    if (clampedAngle < 0) {
        clampedAngle = -clampedAngle;
    }
    
    clampedAngle = fmod(clampedAngle, PI_TIMES_2);
    
    if (clampedAngle >= M_PI) {
        clampedAngle -= M_PI;
        negativeFactor ^= 1;
    }

    if(clampedAngle >= M_PI_2) {
        clampedAngle = M_PI - clampedAngle;
        negativeFactor ^= 1;
    }

    // Chose the appropriate approximation depending on the
    // accepted range.
    fast_sincos_real returnedValue;
    if(clampedAngle < M_PI_4) {
        returnedValue = fastCosChebyshev(clampedAngle, degree);
    } else {
        returnedValue = fastSinChebyshev(M_PI_2 - clampedAngle, degree);
    }

    return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * 0 is 0
 * 2Pi is 2^25 (arbitrary choice for easier initial calculations wihtout overflow)
*/
fast_sincos_real fastFixedSin(const uint32_t fixedAngle) {
  //const uint32_t limit = 2147483648;//(1 << 31); // 0x7FFF FFFF
  //fixedAngle &= ~limit; // only get 31 bits
  //const uint32_t limit = 0x7FFFFFFF;
  //uint32_t inputAngle = fixedAngle; // & limit;

  // 1<<25 = 33 554 432

  // 22 + 9 = 31
  const uint32_t RemainderMask = 0xFFFF; // 16 lsb bits
  const uint32_t indexMask = 0x1FF; // 9 last bits

  //uint16_t remainder = (fixedAngle >> 6) & RemainderMask; // 22 lsb bits
  //uint16_t index = (((uint16_t*)&fixedAngle)[1] >> 6) & indexMask;  // 9 last bits
  uint16_t remainder = fixedAngle & RemainderMask; // 16 lsb bits
  uint16_t index = (((uint16_t*)&fixedAngle)[1]) & indexMask;  // 9 last bits

  // Isolate the angle in the first quandrant
  int negativeFactor = 0;
  if (index >= QUADRANT_SIZE_2) {
    index -= QUADRANT_SIZE_2;
    negativeFactor = 1;
  }

  if (index >= QUADRANT_SIZE) {
    if (remainder) {
      index = QUADRANT_SIZE_2_MINUS_1 - index;
      remainder = LOOKUP_REMAINDER_SIZE - remainder;
    }
    else {
      index = QUADRANT_SIZE_2 - index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint16_t currentValue = ACCESS_TABLE(index); 
  if (remainder) {
    uint32_t temporaryResult = ((uint32_t)(ACCESS_TABLE(index + 1) - currentValue)) * remainder;
    //currentValue = currentValue +  (((ACCESS_TABLE(index + 1) - currentValue) * remainder) >> LOOKUP_REMAINDER_BITS);
    currentValue += ((uint16_t*)&temporaryResult)[1];
  }

  fast_sincos_real returnedValue = scaleValueToRadians((fast_sincos_real)currentValue);
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
        case 3:
            return (
                -1.6034401672287444e-1
                *angleRadiansSquared+9.9903142291243359e-1)
                *angleRadians-1.2156602888538742e-99;
        case 5:
            return ((
                8.1215579245991201e-3
                *angleRadiansSquared-1.6660161988228715e-1)
                *angleRadiansSquared+9.9999499756161918e-1)
                *angleRadians;
        default:
        case 7:
            return (((
                -1.9462116998273101e-4
                * angleRadiansSquared +8.3315846064878458e-3)
                * angleRadiansSquared -1.6666636754299513e-1)
                * angleRadiansSquared +9.9999998617934201e-1)
                * angleRadians;
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
        case 3: // Actually computes for degree 4
            return (
                4.0398535966168857e-2
                *angleRadiansSquared-4.9970814035466399e-1)
                *angleRadiansSquared+9.9999003495519596e-1;
        case 5: // Actually computes for degree 6
            return ((
                -1.3585908510113299e-3
                *angleRadiansSquared+4.1655026884251524e-2)
                *angleRadiansSquared-4.9999856695848848e-1)
                *angleRadiansSquared+9.9999997242332292e-1;
        default:
        case 7: // Actually computes for degree 8
            return (((
                2.4379929375956876e-5
                *angleRadiansSquared-1.3886619210252882e-3)
                *angleRadiansSquared+4.1666616739207635e-2)
                *angleRadiansSquared-4.9999999615433476e-1)
                *angleRadiansSquared+9.9999999995260044e-1;
        
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
    if (remainder) {
      index = QUADRANT_SIZE_2_MINUS_1 - index;
      remainder = LOOKUP_REMAINDER_SIZE - remainder;
    }
    else {
      index = QUADRANT_SIZE_2 - index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint16_t currentValue = ACCESS_TABLE(index); 
  if (remainder > 0) {
    uint32_t temporaryResult = ((uint32_t)(ACCESS_TABLE(index + 1) - currentValue)) * remainder;
    currentValue += ((uint16_t*)&temporaryResult)[1];
  }

  fast_sincos_real returnedValue = scaleValueToRadians((fast_sincos_real)currentValue);
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
    negativeFactor ^= 1;
    if (remainder) {
      index = QUADRANT_SIZE_2_MINUS_1 - index;
      remainder = -remainder;
    }
    else {
      index = QUADRANT_SIZE_2 - index;
    }
  }

  // extended for the multiplication that is about to occur and keep the precision
  uint16_t currentValue;
  if (remainder > 0) 
  {
    currentValue = ACCESS_TABLE(QUADRANT_SIZE_MINUS_1 - index); 
    remainder = LOOKUP_REMAINDER_SIZE - remainder;

    uint32_t temporaryResult = ((uint32_t)(ACCESS_TABLE(QUADRANT_SIZE - index) - currentValue)) * remainder;
    currentValue += ((uint16_t*)&temporaryResult)[1];
  } else {
    currentValue = ACCESS_TABLE(QUADRANT_SIZE - index); 
  }
  
  fast_sincos_real returnedValue = scaleValueToRadians((fast_sincos_real)currentValue);
  return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * @brief Fast sine computation with lookup table only
 * @param angleRadians The angle, in radians.
 * @return A sine approximation of the angle.
*/
static fast_sincos_real lookupSin(const fast_sincos_real angleRadians) {
  
  fast_sincos_real scaledAngle = angleRadians * LOOKUP_SCALE_FACTOR;
  int negativeFactor = 0;
  if (scaledAngle < 0) {
    negativeFactor = 1;
    scaledAngle = -scaledAngle;
  }

  // basic rounding
  uint32_t roundedAngle = lroundf(scaledAngle);
  uint16_t index = roundedAngle & LOOKUP_INDEX_MASK;  // This & replaces fmod

  // Isolate the angle in the first quandrant
  if (index >= QUADRANT_SIZE_2) {
    index -= QUADRANT_SIZE_2;
    negativeFactor ^= 1;
  }

  if (index >= QUADRANT_SIZE) {
    index = QUADRANT_SIZE_2 - index;
  }

  fast_sincos_real returnedValue = scaleValueToRadians((fast_sincos_real)ACCESS_TABLE(index));
  return negativeFactor ? -returnedValue : returnedValue;
}

/**
 * @brief Fast cosine computation with lookup table only.
 * @param angleRadians The angle, in radians.
 * @return A cosine approximation of the angle.
*/
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

  fast_sincos_real returnedValue = scaleValueToRadians((fast_sincos_real)ACCESS_TABLE(QUADRANT_SIZE - index));
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
static inline fast_sincos_real scaleValueToRadians(const fast_sincos_real value) {
  return ldexpf(value, LOOKUP_ELEMENTS_BITS_NEGATIVE);
}
