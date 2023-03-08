#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "genetic.h"


float epsilon = 0.0001;
float mutationRate = 0.1;
uint16_t populationSize =  25;
uint16_t tourneySize = 3;
uint32_t maxIterations = 1000000;

uint16_t testSize = 9;
float testCoords[]  =  {0,1,4,9,16,25,36,49, 64};

float evaluationFunction(float *parameters) {

float fitness = 0;

for (int i = 0; i < testSize; i++) {


    fitness += fabs(parameters[0] * i + parameters[1] * pow(i, 2) +
                    parameters[2] * pow(i, 3) - testCoords[i] +
                    parameters[3]* sin(i));
  }
  return fitness;
}

const int parameterCount = 4;

int main(){

float bestValues[parameterCount];


float value = geneticAlgorithm(bestValues,parameterCount,epsilon,mutationRate,
populationSize,tourneySize,maxIterations,evaluationFunction,0);

printf("Fitness %f \n",value);
printf("Best Coordinates %f %f %f %f \n",bestValues[0],bestValues[1],bestValues[2],bestValues[3]);


}





