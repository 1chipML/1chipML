#ifndef GENETIC_H
#define GENETIC_H

#include <float.h>
#include <limits.h>
#include <linear_congruential_random_generator.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

float geneticAlgorithm(float* bestFitValues, const unsigned int parameterCount,
                       float epsilon, const float mutationChance,
                       const unsigned int generationSize,
                       const unsigned int generations,
                       const unsigned int tourneySize,
                       float (*evaluationFunction)(float*));

#endif
