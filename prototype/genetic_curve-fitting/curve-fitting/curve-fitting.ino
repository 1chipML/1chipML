
#include "genetic.h"

void setup() {

   Serial.begin(115200);
   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
  }



  unsigned int coordinatesSize = 0 ;




  float * coordinatesY;
  float * coordinatesX;
  float *  bestValues ;
  
  float epsilon = 0.0;
  float mutationRate = 0.1;
  unsigned int populationSize =  50;
  unsigned int tourneySize = 5;
  unsigned int maxIterations = 500;

  unsigned int polynomialDegree = 2;




 

void loop() {
  // put your main code here, to run repeatedly:

  
  if (Serial.available()){

  
  digitalWrite(LED_BUILTIN, HIGH);
  
  readElement(&epsilon,sizeof(epsilon));
  readElement(&mutationRate,sizeof(mutationRate));
  readElement(&populationSize, sizeof(populationSize));
 
  readElement(&tourneySize, sizeof(tourneySize));
  readElement(&maxIterations, sizeof(maxIterations));
  readElement(&coordinatesSize, sizeof(coordinatesSize));
  
  // delay(2000);  
  // readElement(&polynomialDegree, sizeof(polynomialDegree));
  

  

  
  
  coordinatesX = realloc(coordinatesX, coordinatesSize * sizeof(*coordinatesX));
  coordinatesY = realloc(coordinatesY, coordinatesSize * sizeof(*coordinatesY));
  bestValues = realloc(bestValues, (polynomialDegree+1) * sizeof(float));

  


  readArray(coordinatesSize, coordinatesX, sizeof(float));
  readArray(coordinatesSize, coordinatesY, sizeof(float));


  digitalWrite(LED_BUILTIN, LOW);

  
  float value = geneticAlgorithm(bestValues, polynomialDegree+1, epsilon,
                                 mutationRate, populationSize, tourneySize,
                                 maxIterations, evaluationFunction, 0);
                                 
  digitalWrite(LED_BUILTIN, HIGH);

  writeElement(&value,sizeof(value));
  writeArray((polynomialDegree+1),bestValues,sizeof(float));

  

  
  
  }



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


/**
 * @brief Read an array of length arraySize, where each element is of size
 * sizeOfElement. This function will block untill all elements are read.
 * @param arraySize The size of the array to read.
 * @param outArray The array in which to store the result of the read data.
 * @param sizeOfElement The size of each element in the array.
 */
int readArray(const uint32_t arraySize, void* outArray,
              const uint32_t sizeOfElement) {
  readElement(outArray, arraySize * sizeOfElement);
}

/**
 * @brief Write an array of length arraySize, where each element is of size
 * sizeOfElement. This function will block untill all elements are written.
 * @param arraySize The size of the array to read.
 * @param array The array containing the data to be written.
 * @param sizeOfElement The size of each element in the array.
 */
void writeArray(const uint32_t arraySize, float* array,
                const uint32_t sizeOfElement) {
  for (uint32_t i = 0; i < arraySize; ++i) {
    writeElement(&array[i], sizeOfElement);
  }
}

/**
 * @brief Read an element of an arbitrary size
 * One byte is read at a time
 * This function will block until all elements are read.
 * @param element The element to read.
 * @param sizeOfElement The size of the element to read.
 */
void readElement(void* element, const uint16_t sizeOfElement) {
  unsigned char* readElement = (unsigned char*)element;
  for (uint16_t i = 0; i < sizeOfElement; ++i) {
    while (Serial.available() < 1); // Wait for element
    Serial.readBytes(&readElement[i], sizeof(unsigned char));
    
  }
}

/**
 * @brief Write an element of an arbitrary size.
 * This function will block until the element is written.
 * @param element The element to write.
 * @param sizeOfElement The size of the element to write.
 */
void writeElement(void* element, const uint32_t sizeOfElement) {
  while (Serial.availableForWrite() < sizeOfElement); // wait for write
  Serial.write((unsigned char*)element, sizeOfElement);
  Serial.flush(); // wait until data is sent
}
