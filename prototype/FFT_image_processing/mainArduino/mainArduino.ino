
#define fft_real float

#include "FFT.h"

void setup() {
  // put your setup code here, to run once:
  // initialize serial:
  Serial.begin(9600, SERIAL_8N1);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {

    uint32_t size = 0;
    float* array = NULL;
    readFloatArray(&size, &array);
    
    writeFloatArray(size, array);

    free(array);

    //char buffer[] = "     ";
    //Serial.readBytes(buffer, 6);
    //String buffer = Serial.readStringUntil('\0');

    // read float from bytes
    //float receivedFloat; // @@@@ = 0x40404040 = 3.00392
    //Serial.readBytes((unsigned char*) &receivedFloat, sizeof(receivedFloat));
    //
    //if (receivedFloat - 3.00392 < 1e-20)
    //  digitalWrite(LED_BUILTIN, HIGH);
    //delay(2000);
    //digitalWrite(LED_BUILTIN, LOW);
    //// Serial.println(buffer);
    //// Reminder: Float has a 6 decimal preicision display
    //Serial.println(receivedFloat, 6);
    
  }
  
}

void writeFloatArray(const uint32_t size, float* array) {
  // Send array size first
  while(!Serial.availableForWrite()); // wait for write
  Serial.write((unsigned char*) &size, sizeof(size));

  // Send individual floats
  for (uint32_t i = 0; i < size; ++i) {
    while(!Serial.availableForWrite()); // wait for write
    Serial.write((unsigned char*) &array[i], sizeof(array[i]));
  }

}

// blocking serial read
void readFloatArray(uint32_t* outSize, float** outArray) {

  uint32_t arraySize = 0; 
  float* readArray = NULL;

  // Read array size first
  while(Serial.available() < 4); // Wait for element
  
  Serial.readBytes((unsigned char*) &arraySize, sizeof(arraySize));

  // Allocate memory for reading
  if (arraySize == 0) {
    return;
  }

  readArray = malloc(arraySize * sizeof(float));

  // Read individual floats
  for (uint32_t i = 0; i < arraySize; ++i) {
    while(Serial.available() < 4); // Wait for element
    Serial.readBytes((unsigned char*) &readArray[i], sizeof(readArray[i]));
  }

  *outSize = arraySize;
  *outArray = readArray;
}

