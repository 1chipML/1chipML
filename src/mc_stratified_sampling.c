#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double myFunction(double x);
void monteCarloEstimateSTD(double lowBound, double upBound, int iterations,
                           double mcStats[]);
void monteCarloEstimateStrat(double lowBound, double upBound, int iterations,
                             double mcStats[], int subdomains);

int main() {

  double lowerBound, upperBound;
  int iterations;

  lowerBound = 0;
  upperBound = 20;

  double mcStats[2]; // position 0 holds the estimate, position 1 holds the STD

  printf("Normal Monte Carlo Integration\n");
  for (int i = 1; i < 6; i++) {

    iterations = 128 * pow(4, i);

    monteCarloEstimateSTD(lowerBound, upperBound, iterations, mcStats);

    printf("Estimate for %.1f -> %.1f is %.3f, STD = %.4f, (%i iterations)\n",
           lowerBound, upperBound, mcStats[0], mcStats[1], iterations);
  }

  printf("Stratified Sampling Monte Carlo Integration\n");
  for (int i = 1; i < 6; i++) {

    iterations = 128 * pow(4, i);

    monteCarloEstimateStrat(lowerBound, upperBound, iterations, mcStats, 4);

    printf("Estimate for %.1f -> %.1f is %.3f, STD = %.4f, (%i iterations)\n",
           lowerBound, upperBound, mcStats[0], mcStats[1], iterations);
  }
  return 0;
}

double myFunction(double x)
// Function to integrate
{
  return exp(-1 * pow(x - 6, 4)) + +exp(-1 * pow(x - 14, 4));
}

void monteCarloEstimateSTD(double lowBound, double upBound, int iterations,
                           double statsArray[])
// Function to execute Monte Carlo integration on predefined function,
// calculates STD
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

  double estimate =
      (upBound - lowBound) * totalSum / iterations; // For normal solve

  double expected = totalSum / iterations;

  double expectedSquare = totalSumSquared / iterations;

  double std = (upBound - lowBound) *
               pow((expectedSquare - pow(expected, 2)) / (iterations - 1), 0.5);

  statsArray[0] = estimate;
  statsArray[1] = std;
}

void monteCarloEstimateStrat(double lowBound, double upBound, int iterations,
                             double statsArray[], int subdomains)
// Function to execute Monte Carlo integration on predefined function, uses
// stratified sampling of equally sized subdomains
{

  double totalSum[subdomains];
  double totalSumSquared[subdomains];
  int iter;

  // Divide the local iterations amoung the subdomains
  iterations = (int)((float)iterations / subdomains);

  for (int i = 0; i < subdomains; i++) {
    totalSum[i] = 0;
    totalSumSquared[i] = 0;
  }

  // Amount of change the range by each time
  double increment = (upBound - lowBound) / (float)subdomains;

  for (int seg = 0; seg < subdomains; seg++) {

    iter = 0;

    double randNum;
    double functionVal;

    double startRange = lowBound + seg * increment;

    while (iter < iterations - 1) {

      randNum = startRange + (float)rand() / RAND_MAX * increment;
      functionVal = myFunction(randNum);

      totalSum[seg] += functionVal;
      totalSumSquared[seg] += pow(functionVal, 2);

      iter++;
    }
  }

  double estimates[subdomains];
  double expecteds[subdomains];
  double expectedSquares[subdomains];
  double STDs[subdomains];

  for (int i = 0; i < subdomains; i++) {

    estimates[i] = increment * totalSum[i] / iterations; // For normal solve
    expecteds[i] = totalSum[i] / iterations;
    expectedSquares[i] = totalSumSquared[i] / iterations;

    STDs[i] = increment * pow((expectedSquares[i] - pow(expecteds[i], 2)) /
                                  (iterations - 1),
                              0.5);
  }

  double estimate = 0;
  double std = 0;

  for (int i = 0; i < subdomains; i++) {

    estimate += estimates[i];

    std += pow(increment, 2) * pow(STDs[i], 2) / iterations;
  }

  statsArray[0] = estimate;
  statsArray[1] = pow(std, 0.5);
}
