#ifndef ARDUINO_SERIAL_PORT_H
#define ARDUINO_SERIAL_PORT_H

#include <Arduino.h>

void setupSerial(const unsigned long baudRate);

void readArray(const unsigned arraySize, void* outArray,
              const unsigned sizeOfElement);

void writeArray(const unsigned arraySize, void* array,
                const unsigned sizeOfElement);

void readElement(void* element, const unsigned sizeOfElement);

void writeElement(void* element, const unsigned sizeOfElement);

#endif // ARDUINO_SERIAL_PORT_H
