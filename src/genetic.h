#ifndef GENETIC_H
#define GENETIC_H

#include "linear_congruential_random_generator.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

// We define a type for the fitness evaluation function that is passed in
// parameters to the algorithm
#ifndef genetic_real
#define genetic_real float
#endif

#ifndef genetic_real_MAX_VALUE
#define genetic_real_MAX_VALUE FLT_MAX
#endif

// We declare these constants to accelerate the execution of the algorithm

#define INT_MAX_INVERSE (1.0 / UINT16_MAX)

#ifndef INT_MAX_DIGIT_COUNT
#define INT_MAX_DIGIT_COUNT 5
#endif

#ifndef INT_MAX_DIGIT_COUNT_AS_STRING
#define INT_MAX_DIGIT_COUNT_AS_STRING_FORMATTER "%.5hu"
#endif

#ifndef genetic_int
#define genetic_int uint16_t
#endif

typedef genetic_real (*fitness_evaluation_function)(genetic_real*);

#ifdef __cplusplus
extern "C" {
#endif

genetic_real
geneticAlgorithm(genetic_real* bestFitValues, const unsigned int parameterCount,
                 const genetic_real epsilon, const genetic_real mutationChance,
                 unsigned int generationSize, unsigned int tourneySize,
                 const unsigned int maxIterations,
                 fitness_evaluation_function function,
                 unsigned numberOfEliteValues, const unsigned lowMemoryMode);

#ifdef __cplusplus
}
#endif

#endif // GENETIC_H
