#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

typedef struct serialPort_t {
    unsigned baudRate;         // The baudrate
    unsigned serialBufferSize; // The serial buffer size, in bytes
    char* serialPortName;      // The serial port name
    int fileDescriptor;        // The file descriptor of the opened serial port. Should not be changed
} serialPort_t;


void closeSerialPort(serialPort_t* serialPort);
int openSerialPort(serialPort_t* serialPort);

int readElement(serialPort_t* serialPort, 
                void* element, 
                const unsigned sizeOfElement);

int writeElement(serialPort_t* serialPort, 
                void* element, 
                const unsigned sizeOfElement);

#endif // SERIAL_PORT_H
