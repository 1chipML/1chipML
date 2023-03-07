#include "arduino_serial_port.h"


/**
 * @brief Initialize serial
 * @param baudRate The desired baudrate
 * @param config The serial configuration
*/
void setupSerial(const unsigned long baudRate) {
  Serial.begin(baudRate, SERIAL_8N1);
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
 * @brief Write an array of length arraySize. 
 * This function will block until all elements are written.
 * @param arraySize The size of the array to read.
 * @param array The array containing the data to be written.
 */
void writeFloatArray(const uint32_t arraySize, float* array) {
  for (uint32_t i = 0; i < arraySize; ++i) {
    writeElement(&array[i], sizeof(float));
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
