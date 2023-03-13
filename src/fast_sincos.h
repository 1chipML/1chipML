#ifndef FAST_SINCOS_H
#define FAST_SINCOS_H

#include <stdint.h>

#ifndef fast_sincos_real
    #define fast_sincos_real double
#endif

/** 
 * The scaling factor to use the lookup table.
 * The scaling factor should be adapted to the lookup table and the incoming angle.
 * The scaling factor is computed by taking the total size of all 
 * quadrants, divided by the expected input.
 * e.g 1:, if the sine lookup table is of size 128, and only represents the
 * first quandrant, then the size of all quandrants is "128 * 4".
 * If the input is expected to be in radians, then the scaling factor would
 * be "128 * 4 / (2 * pi)"
 * e.g 2:, if the sine lookup table is of size 128, and represents all four
 * quadrants, then the size of all quandrants is "128".
 * If the input is expected to be in radians, then the scaling factor would
 * be "128 / (2 * pi)"
*/
#define LOOKUP_SCALE_FACTOR 81.4873308630504119136684 // 128 * 4 / (2 * pi)

// Lookup sine table constants
#define QUADRANT_SIZE 128 // The size of a quadrant.
#define QUADRANT_SIZE_MINUS_1 127 // QUADRANT_SIZE - 1.
#define QUADRANT_SIZE_2 256 // QUADRANT_SIZE * 2.
#define LOOKUP_INDEX_MASK 511 // QUADRANT_SIZE * 4 - 1. Used for a modulus of the calculated index
#define LOOKUP_REMAINDER_SIZE 65536 // Max size + 1 of the remainder used for interpolation.
#define LOOKUP_REMAINDER_BITS 16 // The number of bits allocated for the remainder
#define LOOKUP_ELEMENTS_BITS_NEGATIVE -16 // Number of bits allocated for each element in the lookup table, times -1


#ifdef __cplusplus
extern "C" {
#endif

fast_sincos_real fastSin(const fast_sincos_real angleRadians, const int degree);
fast_sincos_real fastCos(const fast_sincos_real angleRadians, const int degree);

#ifdef __cplusplus
}
#endif

#endif // FAST_SINCOS
