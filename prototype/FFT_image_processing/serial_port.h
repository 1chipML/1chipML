#include <stdint.h>

void closeSerialPort();
int openSerialPort(char* portName);
int writeFloatArray(const uint32_t size, float* array);
int readFloatArray(const uint32_t readSize, float* outArray);
int readUnkownFloatArray(uint32_t* outSize, float** outArray);
