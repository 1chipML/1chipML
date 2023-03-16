
extern "C" {
#include "genetic.h"
}

#include "arduino_serial_port.h"


unsigned int coordinatesSize;




float * coordinatesY;
float * coordinatesX;
float *  bestValues ;
  
float epsilon ;
float mutationRate ;
unsigned int populationSize ;
unsigned int tourneySize;
unsigned int maxIterations;
unsigned int polynomialDegree;

void setup() {

   Serial.begin(115200);
   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
  }






float evaluationFunction(float *parameters) {

  float fitness = 0;
  float maxValues [polynomialDegree+1] = {1. ,1.,1.};
  for(unsigned int i = 0 ; i < coordinatesSize ; i++ ){

    float sum = 0;      
    sum += maxValues[0]*parameters[0];      
    for (unsigned int j = 1 ; j <= polynomialDegree ; j++){
      sum += maxValues[j]*parameters[j]*pow(coordinatesX[i],j);
    }
    sum = fabs(sum - coordinatesY[i]);     
    fitness+=sum;
  }
    
   return  fabs(fitness) ;
  
  
}

 

void loop() {
  // put your main code here, to run repeatedly:

  
  if (Serial.available()){

  
  // digitalWrite(LED_BUILTIN, HIGH);
  
  readElement(&epsilon,sizeof(epsilon));
  readElement(&mutationRate,sizeof(mutationRate));
  readElement(&populationSize, sizeof(populationSize));
 
  readElement(&tourneySize, sizeof(tourneySize));
  readElement(&maxIterations, sizeof(maxIterations));
  readElement(&coordinatesSize, sizeof(coordinatesSize));
  
  // delay(2000);  


  

  
  
  coordinatesX = realloc(coordinatesX, coordinatesSize * sizeof(*coordinatesX));
  coordinatesY = realloc(coordinatesY, coordinatesSize * sizeof(*coordinatesY));
  bestValues = realloc(bestValues, (polynomialDegree+1) * sizeof(float));

  


  readArray(coordinatesSize, coordinatesX, sizeof(float));
  readArray(coordinatesSize, coordinatesY, sizeof(float));
  
  //readElement(&polynomialDegree, sizeof(polynomialDegree));

  srand(tourneySize);

  polynomialDegree = rand()%1 + 2;

  digitalWrite(LED_BUILTIN, LOW);

  
  float value = geneticAlgorithm(bestValues, polynomialDegree+1, epsilon,
                                 mutationRate, populationSize, tourneySize,
                                 maxIterations, evaluationFunction, 0);
                                 
  digitalWrite(LED_BUILTIN, HIGH);

  writeElement(&value,sizeof(value));
  writeFloatArray((polynomialDegree+1),bestValues);

  

  
  
  }


}







