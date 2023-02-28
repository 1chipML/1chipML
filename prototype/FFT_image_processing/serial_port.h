#include <stdint.h>

void closeSerialPort();
int openSerialPort(char* portName);

int writeFloatArray(const uint32_t size, float* array);
int writeElement(void* element, const unsigned sizeOfElement);

int readFloatArray(const uint32_t readSize, float* outArray);
int readUnkownFloatArray(uint32_t* outSize, float** outArray);
int readElement(void* element, const unsigned sizeOfElement);
