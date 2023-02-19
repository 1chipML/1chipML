#include <genetic.h>
#include <stdio.h>

// This is a function that we want to optimize
static float evaluateStrength(float* population) {

  // We want to minimize this function
  return fabs(4 * population[0] - exp(2 * population[1]) + 2);
}

int main() {

  float epsilon = 0.0001;
  float mutationRate = 0.01;
  float bestFitValues[2];

  geneticAlgorithm(bestFitValues, 2, epsilon, mutationRate, 100, 10000, 20,
                   evaluateStrength);
  

  float fitness = evaluateStrength(bestFitValues);

  if (fitness > epsilon) {

    printf("Genetic test failed! Fitness %f expected smaller than %f\n",
           fitness, epsilon);
    return 1;
  } else {
    printf("Genetic test success! Fitness %f expected smaller than %f\n",
           fitness, epsilon);
    return 0;
  }
}
