#include "genetic.h"
#include "arduino_serial_port.h"

// Global variables that are used by the evaluation function
float* coordinatesY;
float* coordinatesX;
float* limits;  
  
unsigned int coordinatesSize;
unsigned int polynomialDegree;



void setup() {

   Serial.begin(9600);
   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
}

/**
 * @brief This function is used to evaluate each solution of coordinates
 * 
 * @param parameters the solution of coefficients that needs to be evaluated
 * @return the fitness of the solution
 */
float evaluationFunction(const float *parameters) {

  float fitness = 0;
  for(unsigned int i = 0 ; i < coordinatesSize ; i++ ){

    float sum = 0;      
    for (unsigned int j = 0 ; j <= polynomialDegree ; j++){
      sum += limits[j] * ((parameters[j]- 0.5) * 2) * pow(coordinatesX[i],j);
    }
    sum = fabs(sum - coordinatesY[i]);     
    fitness += sum;
  }
   return  fabs(fitness) ;
}

 

void loop() {


  //Variable definition
  float epsilon;
  float mutationRate;
  unsigned int populationSize;
  unsigned int tourneySize;
  unsigned int maxIterations;

  // Read all of the variables from the serial connection
  readElement(&epsilon,sizeof(epsilon));
  readElement(&mutationRate,sizeof(mutationRate));
  readElement(&populationSize, sizeof(populationSize));
  readElement(&tourneySize, sizeof(tourneySize));
  readElement(&maxIterations, sizeof(maxIterations));
  readElement(&polynomialDegree, sizeof(polynomialDegree));
  readElement(&coordinatesSize, sizeof(coordinatesSize));

  unsigned int dimensions = polynomialDegree+1;

  // Initiliaze the global arrays 
  coordinatesX = realloc(coordinatesX ,coordinatesSize * sizeof(*coordinatesX));
  coordinatesY = realloc(coordinatesY,coordinatesSize * sizeof(*coordinatesY));
  limits = realloc(limits, sizeof(*limits)* dimensions);

  // We make sure each of the limits are positive
  for (unsigned int i = 0 ; i < dimensions; i++){
    limits[i] = fabs(limits[i]);
  }

  // Define the array that stores the best coefficients
  float bestValues[dimensions];

  // Read all of the arrays from the serial connection
  readArray(coordinatesSize, coordinatesX, sizeof(*coordinatesX));
  readArray(coordinatesSize, coordinatesY, sizeof(*coordinatesY));
  readArray(dimensions, limits, sizeof(*limits));

  digitalWrite(LED_BUILTIN, HIGH);


  float value = geneticAlgorithm(bestValues, dimensions, epsilon,
                                 mutationRate, populationSize, tourneySize,
                                 maxIterations, evaluationFunction, 0);

  digitalWrite(LED_BUILTIN, LOW);

  // Send answers through the serial connection
  writeElement(&value,sizeof(value));
  writeFloatArray((dimensions),bestValues);
  
}
