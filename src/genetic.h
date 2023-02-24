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
typedef float (*fitness_evaluation_function)(float*);

float geneticAlgorithm(float* bestFitValues, const unsigned int parameterCount,
                       float epsilon, const float mutationChance,
                       const unsigned int generationSize,
                       const unsigned int generations,
                       const unsigned int tourneySize,
                       fitness_evaluation_function function);

#endif
