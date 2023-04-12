#include <genetic.h>
#include <stdio.h>

// This is a function that we want to optimize
static float evaluateStrength(float* population) {

  // We want to minimize this function
  return fabs(4 * population[0] - exp(2 * population[1]) + 2);
}

static int verifyCoordinates(float* firstSet, float* secondSet,
                             int parameterCount) {

  for (unsigned int i = 0; i < parameterCount; i++) {
    if (firstSet[i] != secondSet[i])
      return 0;
  }

  return 1;
}

int main() {

  float epsilon = 0.0001;
  float mutationRate = 0.01;
  float bestFitValues[2];
  float lowMemoryBestFitValues[2];
  const unsigned int dimensions = 2;
  unsigned int numberOfEliteValues = 2;
  const unsigned int maxIterations = 10000;
  unsigned int populationSize = 100;
  unsigned int tournamentSize = 20;

  set_linear_congruential_generator_seed(16);

  float result = geneticAlgorithm(
      bestFitValues, dimensions, epsilon, mutationRate, populationSize,
      tournamentSize, maxIterations, evaluateStrength, numberOfEliteValues, 0);

  // We reset the seed
  set_linear_congruential_generator_seed(16);

  float lowMemoryResult = geneticAlgorithm(
      lowMemoryBestFitValues, dimensions, epsilon, mutationRate, populationSize,
      tournamentSize, maxIterations, evaluateStrength, numberOfEliteValues, 1);

  float fitness = evaluateStrength(bestFitValues);

  if (fitness > epsilon) {

    printf("Genetic test failed! Fitness %f expected smaller than %f\n",
           fitness, epsilon);
    return 1;
  }
  // We permit an approximation because the values are floats
  else if (fabs(lowMemoryResult - result) >= 0.0001) {
    printf("Genetic test failed! Fitness %f found by algorithm is not the same "
           "one found by the low memory version %f \n",
           fitness, epsilon);
    return 1;
  }

  else if (verifyCoordinates(bestFitValues, lowMemoryBestFitValues,
                             dimensions) == 0) {
    printf("Genetic test failed! Coordinates found by the algorithm are not "
           "the same as the ones found by the low memory version \n");
    return 1;
  }

  else {
    printf("Genetic test success! Fitness %f expected smaller than %f\n",
           fitness, epsilon);
    return 0;
  }
}
