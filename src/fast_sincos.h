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
extern const uint16_t sineTable[129];
//  = {
//     0,  804, 1608, 2412, 3216, 4019, 4821, 5623,
//  6424, 7223, 8022, 8820, 9616,10411,11204,11996,
// 12785,13573,14359,15142,15924,16703,17479,18253,
// 19024,19792,20557,21319,22078,22834,23586,24334,
// 25079,25820,26557,27291,28020,28745,29465,30181,
// 30893,31600,32302,32999,33692,34379,35061,35738,
// 36409,37075,37736,38390,39039,39682,40319,40950,
// 41575,42194,42806,43411,44011,44603,45189,45768,
// 46340,46905,47464,48014,48558,49095,49624,50145,
// 50659,51166,51664,52155,52638,53113,53580,54039,
// 54490,54933,55367,55794,56211,56620,57021,57413,
// 57797,58171,58537,58895,59243,59582,59913,60234,
// 60546,60850,61144,61429,61704,61970,62227,62475,
// 62713,62942,63161,63371,63571,63762,63943,64114,
// 64276,64428,64570,64703,64826,64939,65042,65136,
// 65219,65293,65357,65412,65456,65491,65515,65530,
// 65535
// };

#ifdef __cplusplus
extern "C" {
#endif

typedef const uint16_t (*tableAccessType)(const uint16_t*);
void setTableAccessFunction(tableAccessType tableAccess);
fast_sincos_real fastSin(const fast_sincos_real angleRadians, const int degree);
fast_sincos_real fastCos(const fast_sincos_real angleRadians, const int degree);

#ifdef __cplusplus
}
#endif

#endif // FAST_SINCOS
