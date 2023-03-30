#ifndef ARDUINO_SERIAL_PORT_H
#define ARDUINO_SERIAL_PORT_H

#include <Arduino.h>

void setupSerial(const unsigned long baudRate);

int readArray(const uint32_t arraySize, void* outArray,
              const uint32_t sizeOfElement);

void writeFloatArray(const uint32_t arraySize, float* array);

void readElement(void* element, const uint32_t sizeOfElement);

void writeElement(void* element, const uint32_t sizeOfElement);

<<<<<<< HEAD
#endif // ARDUINO_SERIAL_PORT_H
=======
#endif // ARDUINO_SERIAL_PORT_H
>>>>>>> c6c0a1f... Anomaly detection prototype
