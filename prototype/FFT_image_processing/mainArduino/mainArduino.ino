
#define fft_real float

#include "FFT.h"

void setup() {
  // initialize serial:
  Serial.begin(115200, SERIAL_8N1);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Tell we are ready
  const unsigned messageLength = 6;
  const char readyBuffer[messageLength] = "Ready";
  while(Serial.availableForWrite() < messageLength); // wait for write
  Serial.write(readyBuffer, messageLength);
}

unsigned FFTLength = 0;
int dir = 0;
int fftCode = 0;

void loop() {
  if (Serial.available() && fftCode == 0) {

    digitalWrite(LED_BUILTIN, HIGH);

    // FFT over serial

    // Reading
    // read length
    readElement(&FFTLength, sizeof(FFTLength));

    // init arrays
    fft_real FFTreals[FFTLength];
    fft_real FFTimgs[FFTLength];
    memset(FFTreals, 0, FFTLength * sizeof(fft_real));
    memset(FFTimgs, 0, FFTLength * sizeof(fft_real));


    // read reals
    readArray(FFTLength, FFTreals, sizeof(fft_real));
    // read imaginaries
    readArray(FFTLength, FFTimgs, sizeof(fft_real));
    // read direction
    readElement(&dir, sizeof(dir));

    //Execute FFT
    fftCode = FFT(FFTLength, FFTreals, FFTimgs, dir);
    if (fftCode != 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      return;
    }

    // Writing
    // write reals
    writeArray(FFTLength, FFTreals, sizeof(fft_real));
    // write imaginaries
    writeArray(FFTLength, FFTimgs, sizeof(fft_real));

    //uint32_t size = 0;
    //float* array = NULL;
    //readElement(&size, sizeof(size));
    //if (size > 0) {
    //  array = malloc(size * sizeof(float));
    //}
    //readArray(size, array, sizeof(float));
    //writeArray(size, array, sizeof(float));
    //free(array);
    //array = NULL;

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
    
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  
}

int readArray(const uint32_t arraySize, void* outArray, const uint32_t sizeOfElement) {
  readElement(outArray, arraySize * sizeOfElement);
}

void writeArray(const uint32_t arraySize, float* array, const uint32_t sizeOfElement) {
  for(uint32_t i = 0; i < arraySize; ++i) {
    writeElement(&array[i], sizeOfElement);
  }
}

void readElement(void* element, const uint32_t sizeOfElement) {

  unsigned char* readElement = (unsigned char*) element;
  for(uint32_t i = 0; i < sizeOfElement; ++i) {
    while(Serial.available() < 1); // Wait for element
    Serial.readBytes(&readElement[i], sizeof(unsigned char));
  }
  
}

void writeElement(void* element, const uint32_t sizeOfElement) {
  while(Serial.availableForWrite() < sizeOfElement); // wait for write
  Serial.write((unsigned char*) element, sizeOfElement);
  Serial.flush(); // wait until data is sent
}
