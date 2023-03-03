#include "genetic.h"

int coordinates[] =  {0 ,1, 4, 9 , 16, 25, 36 , 49 ,64 , 81 , 100};

int currentIndex = 11;

float evaluationFunction(float * parameters){

    float fitness = 0;
    
    for (unsigned int i = 0 ; i < currentIndex ; i++) {
    
        fitness += fabs(parameters[0]*i * 1 + parameters[1]*1*pow(i,2) + parameters[2] * pow(i,3) - coordinates[i] );

    }
    return fitness;




}

int main(){

    
    float bestValues [3];
    unsigned int parameterCount = 3;
    float epsilon = 0.0001;
    float mutationRate = 0.01;
    unsigned int populationSize = 100;
    unsigned int tourneySize = 10;
    unsigned int maxIterations = 200;

    


    float value = geneticAlgorithm(bestValues, parameterCount, epsilon, mutationRate, populationSize, tourneySize, maxIterations, evaluationFunction,0);
    printf("Value %f \n", value);

    printf("Parameters %f %f %f  \n",bestValues[0],bestValues[1],bestValues[2]);
}

// geneticAlgorithm();