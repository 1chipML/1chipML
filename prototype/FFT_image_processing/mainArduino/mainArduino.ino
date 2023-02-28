
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
    readElement(&size, sizeof(size));

    if (size > 0) {
      array = malloc(size * sizeof(float));
    }

    readArray(size, array, sizeof(float));
    writeArray(size, array, sizeof(float));

    free(array);
    array = NULL;

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

int readArray(const uint32_t arraySize, void* outArray, const uint32_t sizeOfElement) {
  readElement(outArray, arraySize * sizeOfElement);
}

void writeArray(const uint32_t arraySize, void* array, const uint32_t sizeOfElement) {
  writeElement(array, arraySize * sizeOfElement);
}

void readElement(void* element, const uint32_t sizeOfElement) {

  unsigned char* readElement = (unsigned char*) element;
  for(uint32_t i = 0; i < sizeOfElement; ++i) {
    while(Serial.available() < 1); // Wait for element
    Serial.readBytes(&readElement[i], sizeof(unsigned char));
  }
  
}

void writeElement(void* element, const uint32_t sizeOfElement) {
  while(!Serial.availableForWrite()); // wait for write
  Serial.write((unsigned char*) element, sizeOfElement);
}
