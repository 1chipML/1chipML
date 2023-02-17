#include <linear_congruential_random_generator.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

// This variable decides the maxixmum amount of generations that can be created
unsigned int numberOfGenerations;

// This controls the quantity of solutions per generation. This value greatly
// controls the memory used by this algorithm
unsigned int populationSize;

// This value controls the chance that a child will have a mutation
float mutationRate;

// The amount of parameters that each solution need
unsigned int dimensions;

// The amount of solutions chosen for each parent-selection tourney
unsigned int tournamentSelectionsSize;




float geneticAlgorithm(float* bestFitValues, const unsigned int parameterCount,
                       float epsilon, const float mutationChance,
                       const unsigned int generationSize,
                       const unsigned int generations,
                       const unsigned int tourneySize,
                       float (*evaluationFunction)(float*));
