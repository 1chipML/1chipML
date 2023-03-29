#include "arduino_serial_port.hpp"


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
void readArray(const unsigned arraySize, void* outArray,
              const unsigned sizeOfElement) {
  unsigned char* elementIndex = (unsigned char*)outArray;
  for (unsigned i = 0; i < arraySize; ++i) {
    readElement(elementIndex, sizeOfElement);
    elementIndex += sizeOfElement;
  }
}

/**
 * @brief Write an array of length arraySize. 
 * This function will block until all elements are written.
 * @param arraySize The size of the array to read.
 * @param array The array containing the data to be written.
 * @param sizeOfElement The size of each element in the array.
 */
void writeArray(const unsigned arraySize, 
                              void* array, 
                              const unsigned sizeOfElement) {
  unsigned char* elementIndex = (unsigned char*)array;
  for (unsigned i = 0; i < arraySize; ++i) {
    writeElement(elementIndex, sizeOfElement);
    elementIndex += sizeOfElement;
  }
}

/**
 * @brief Read an element of an arbitrary size
 * This function will block until the element is read.
 * @param element The element to read.
 * @param sizeOfElement The size of the element to read.
 */
void readElement(void* element, const unsigned sizeOfElement) {
  while (Serial.available() < sizeOfElement); // Wait for element
  
  // read
  unsigned char* readElement = (unsigned char*)element;
  Serial.readBytes(readElement, sizeOfElement);
}

/**
 * @brief Write an element of an arbitrary size.
 * This function will block until the element is written.
 * @param element The element to write.
 * @param sizeOfElement The size of the element to write.
 */
void writeElement(void* element, const unsigned sizeOfElement) {
  while (Serial.availableForWrite() < sizeOfElement); // wait for write
  Serial.write((unsigned char*)element, sizeOfElement);
  Serial.flush(); // wait until data is sent
}
