void closeSerialPort();
int openSerialPort(char* portName, 
                   const unsigned serialBufferSize,
                   const unsigned baudRate);

int readArray(const unsigned arraySize, 
              void* outArray,
              const unsigned sizeOfElement);
int readElement(void* element, const unsigned sizeOfElement);

int writeArray(const unsigned arraySize, 
               void* array,
               const unsigned sizeOfElement);
int writeElement(void* element, const unsigned sizeOfElement);
