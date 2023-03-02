#ifndef GENETIC_H
#define GENETIC_H

#include <float.h>
#include <limits.h>
#include <linear_congruential_random_generator.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

// We define a type for the fitness evaluation function that is passed in
// parameters to the algorithm

#define genetic_real float
#define genetic_real_MAX_VALUE FLT_MAX

// We declare these constants to accelerate the execution of the algorithm

#define INT_MAX_INVERSE (1.0 / UINT16_MAX)
#define INT_MAX_DIGIT_COUNT 5

#define genetic_int uint16_t

typedef genetic_real (*fitness_evaluation_function)(genetic_real *);

genetic_real
geneticAlgorithm(genetic_real *bestFitValues, const unsigned int parameterCount,
                 genetic_real epsilon, const genetic_real mutationChance,
                 unsigned int generationSize, unsigned int tourneySize,
                 const unsigned int maxIterations,
                 fitness_evaluation_function function,
                 const unsigned lowMemoryMode);

#endif
