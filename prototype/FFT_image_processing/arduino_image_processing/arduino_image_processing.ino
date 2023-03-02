
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
  while (Serial.availableForWrite() < messageLength); // wait for write
  Serial.write(readyBuffer, messageLength);
  Serial.flush();
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

    // Execute FFT
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

  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
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
void readElement(void* element, const uint32_t sizeOfElement) {
  unsigned char* readElement = (unsigned char*)element;
  for (uint32_t i = 0; i < sizeOfElement; ++i) {
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
