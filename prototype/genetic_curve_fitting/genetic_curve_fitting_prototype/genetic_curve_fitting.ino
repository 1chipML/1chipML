extern "C" {
#include "genetic.h"
}

#include "arduino_serial_port.h"

float* coordinatesY;
float*  coordinatesX;
float*  bestValues; 
float*  limits;  
  
float epsilon;
float mutationRate;
unsigned int populationSize;
unsigned int tourneySize;
unsigned int maxIterations;
unsigned int coordinatesSize;
unsigned int polynomialDegree;

void setup() {

   Serial.begin(9600);
   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
}

float evaluationFunction(float *parameters) {

  float fitness = 0;
  for(unsigned int i = 0 ; i < coordinatesSize ; i++ ){

    float sum = 0;      
    sum += limits[0]*parameters[0];      
    for (unsigned int j = 1 ; j <= polynomialDegree ; j++){
      sum += limits[j]*parameters[j]*pow(coordinatesX[i],j);
    }
    sum = fabs(sum - coordinatesY[i]);     
    fitness+=sum;
  }
   return  fabs(fitness) ;
}

 

void loop() {
  
  readElement(&epsilon,sizeof(epsilon));
  readElement(&mutationRate,sizeof(mutationRate));
  readElement(&populationSize, sizeof(populationSize));
  readElement(&tourneySize, sizeof(tourneySize));
  readElement(&maxIterations, sizeof(maxIterations));
  readElement(&polynomialDegree, sizeof(polynomialDegree));


  readElement(&coordinatesSize, sizeof(coordinatesSize));

  coordinatesX = realloc(coordinatesX ,coordinatesSize * sizeof(*coordinatesX));
  coordinatesY = realloc(coordinatesY,coordinatesSize * sizeof(*coordinatesY));
  bestValues = realloc(bestValues, sizeof(*bestValues)* (polynomialDegree+1));
  limits = realloc(limits, sizeof(*limits)* (polynomialDegree+1));

  if (coordinatesX == NULL || coordinatesY == NULL || bestValues == NULL){
    abort();
  }

  readArray(coordinatesSize, coordinatesX, sizeof(*coordinatesX));
  readArray(coordinatesSize, coordinatesY, sizeof(*coordinatesY));
  readArray(polynomialDegree+1, limits, sizeof(*limits));

  float value = geneticAlgorithm(bestValues, polynomialDegree+1, epsilon,
                                 mutationRate, populationSize, tourneySize,
                                 maxIterations, evaluationFunction, 0);

  digitalWrite(LED_BUILTIN, LOW);


  writeElement(&value,sizeof(value));
  writeFloatArray((polynomialDegree+1),bestValues);
  
}







