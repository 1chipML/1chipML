#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double myFunction(double x);
void monteCarloEstimateSTD(double lowBound, double upBound, int iterations,
                           double mcStats[]);

int main() {

  double lowerBound, upperBound;
  int iterations;

  lowerBound = 1;
  upperBound = 5;

  double mcStats[2]; // position 0 holds the estimate, position 1 holds the STD

  for (int i = 1; i < 6; i++) {

    iterations = 2 * pow(4, i);

    monteCarloEstimateSTD(lowerBound, upperBound, iterations, mcStats);

    printf("Estimate for %.1f -> %.1f is %.3f, STD = %.4f, (%i iterations)\n",
           lowerBound, upperBound, mcStats[0], mcStats[1], iterations);
  }

  return 0;
}

double myFunction(double x)
// Function to integrate
{
  return pow(x, 4) * exp(-x);
}

void monteCarloEstimateSTD(double lowBound, double upBound, int iterations,
                           double statsArray[])
// Function to execute Monte Carlo integration on predefined function
{

  double totalSum = 0;
  double totalSumSquared = 0;

  int iter = 0;

  while (iter < iterations - 1) {

    double randNum =
        lowBound + (float)rand() / RAND_MAX * (upBound - lowBound);

    double functionVal = myFunction(randNum);

    totalSum += functionVal;
    totalSumSquared += pow(functionVal, 2);

    iter++;
  }

  double estimate = (upBound - lowBound) * totalSum / iterations;
  double expected = totalSum / iterations;

  double expectedSquare = totalSumSquared / iterations;

  double std = (upBound - lowBound) *
               pow((expectedSquare - pow(expected, 2)) / (iterations - 1), 0.5);

  statsArray[0] = estimate;
  statsArray[1] = std;
}
