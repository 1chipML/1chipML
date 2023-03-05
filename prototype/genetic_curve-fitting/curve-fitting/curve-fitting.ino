
#include "genetic.h"


void setup() {

   Serial.begin(9600);
  }

int coordinates[] = {169, 144, 121, 100, 81, 64,  49,  36,  25,
                     16,  9,   4,   1,   0,  1,   4,   9,   16,
                     25,  36,  49,  64,  81, 100, 121, 144, 169};

    // int coordinates[] = {0 , 2.5, 6.5, 12.5, 20.5, 30.5, 42.5, 56.5, 72.5, 90.5,
    // 110.5, 132.5, 156.5, 182.5};

int currentIndex = 13;
  
float evaluationFunction(float *parameters) {

float fitness = 0;

for (int i = -currentIndex; i < currentIndex - 1; i++) {


    fitness += fabs(parameters[0] * i + parameters[1] * pow(i, 2) +
                    parameters[2] * pow(i, 3) - coordinates[i + currentIndex] +
                    parameters[3]);
  }
  return fitness;
}


  const unsigned int parameterCount = 4;

  float bestValues[parameterCount];
  float epsilon = 0.0001;
  float mutationRate = 1;
  unsigned int populationSize =  50;
  unsigned int tourneySize = 10;
  unsigned int maxIterations = 1000;



 

void loop() {
  // put your main code here, to run repeatedly:
 

  
  float value = geneticAlgorithm(bestValues, parameterCount, epsilon,
                                 mutationRate, populationSize, tourneySize,
                                 maxIterations, evaluationFunction, 0);
  
  Serial.println(value);

  Serial.println("Values ");
  Serial.println(bestValues[0]);
  Serial.println(bestValues[1]);
  Serial.println(bestValues[2]);
  Serial.println(bestValues[3]);
  
 

}
