// C library headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "serial_port.h"

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

static int setupSerialPort(int serialPort);
static int readElement(void* element, const unsigned sizeOfElement);
static int fileDescriptor = -1;


int openSerialPort(char* portName) {
    fileDescriptor = open(portName, O_RDWR | O_NOCTTY);
    if (fileDescriptor < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return 1;
    }

    int setupReturnCode = setupSerialPort(fileDescriptor);
    if (setupReturnCode != 0) {
        closeSerialPort();
        return setupReturnCode;
    }

    // The Arduino resets when the first connection is created.
    // This prevents from directly sending data to the arduino when
    // the serial connection is initiated, as it is resetting.
    // This behaviour is linked to the DTR.
    // Sleep for 2 seconds to successfully send data to
    // the Arduino for the first time.
    // In order to bypass this behavior and remove the sleep(), 
    // it is possible to attach a 10 uF electrolytic capacitor between 
    // the Reset and the Ground on the arduino UNO board.
    sleep(2);

    return 0;
}

void closeSerialPort() {
    if (fileDescriptor < 0) {
        return;
    }
    close(fileDescriptor);
    fileDescriptor = -1;
}

/*
int main() {

    char* portName = "/dev/ttyACM0";
    int fd = open(portName, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return 1;
    }

    int setupReturnCode = setupSerialPort(fd);
    if (setupReturnCode != 0) {
        close(fd);
        return setupReturnCode;
    }

    // The Arduino resets when the first connection is created.
    // This prevents from directly sending data to the arduino when
    // the serial connection is initiated, as it is resetting.
    // This behaviour is linked to the DTR.
    // Sleep for 2 seconds to successfully send data to
    // the Arduino for the first time.
    // In order to bypass this behavior and remove the sleep(), 
    // it is possible to attach a 10 uF electrolytic capacitor between 
    // the Reset and the Ground on the arduino UNO board.
    sleep(2);

    unsigned char message[] = "hello world!";
    //int result = write(fd, message, sizeof(message));

    float sentFloat = 3.00392;
    unsigned char* sentFloatData = (unsigned char *) &sentFloat; 
    write(fd, sentFloatData, sizeof(sentFloat));


    close(fd);

    printf("sentfloat: %.20f\n", sentFloat);
    //printf("write res = %i\n", result);
    return 0;
}*/

static int setupSerialPort(int serialPort) {
    struct termios tty;

    if(tcgetattr(serialPort, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    // set up tty
    // Control modes
    tty.c_cflag &= ~(PARENB | PARODD); // Disable parity
    tty.c_cflag &= ~CSTOPB; // Only one stop bit
    tty.c_cflag &= ~CSIZE; // Clear current byte size
    tty.c_cflag |= CS8; // 8 bits per byte
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    tty.c_cflag |= (CLOCAL | CREAD);// Ignore modem controls, enable reading

    // Local modes
    tty.c_lflag &= ~ICANON; // Disable canonical mode
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable newline echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    // Input modes
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling

    // Output modes
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    // VMIN and VTIME have no effect in canonical mode
    tty.c_cc[VMIN] = 1; // Wait until one byte is received
    tty.c_cc[VTIME] = 0; // Wait for up to 0s (Blocking at 0)
    
    cfsetospeed (&tty, B9600); // Output baud rate 
    cfsetispeed (&tty, B9600); // Input baud rate 

    // Save tty settings
    tcflush(serialPort, TCIOFLUSH);
    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    if (tcsetattr(serialPort, TCSAFLUSH, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    return 0;
}

int writeFloatArray(const uint32_t size, float* array) {
    if (fileDescriptor < 0) {
        return -1;
    }

    int numBytesWritten = 0;
    int returnValue = -1;

    // Send array size first
    returnValue = write(fileDescriptor, &size, sizeof(size));
    if (returnValue == -1) {
        printf("Error %i from writing to serial port: %s\n", errno, strerror(errno));
        return returnValue;
    }
    numBytesWritten += returnValue;

    // Send individual floats
    for (uint32_t i = 0; i < size; ++i) {
        returnValue = write(fileDescriptor, &array[i], sizeof(array[i]));

        if (returnValue == -1) {
            printf("Error %i from writing to serial port: %s\n", errno, strerror(errno));
            return returnValue;
        }
        numBytesWritten += returnValue;
    }

    return numBytesWritten;
}

int readFloatArray(const uint32_t readSize, float* outArray) {
    if (fileDescriptor < 0) {
        return -1;
    }

    uint32_t arraySize = 0;
    int numBytesRead = 0;
    int returnValue = -1;

    // Read array size first
    returnValue = readElement(&arraySize, sizeof(arraySize));
    if (returnValue == -1) {
        return returnValue;
    }
    numBytesRead += returnValue;

    // Read individual floats
    for (uint32_t i = 0; i < arraySize; ++i) {
        returnValue = readElement(&outArray[i], sizeof(outArray[i]));
        if (returnValue == -1) {
            return returnValue;
        }
        numBytesRead += returnValue;
    }

    return numBytesRead;
}

/**
 * 
 * 1D array reference
*/
int readUnkownFloatArray(uint32_t* outSize, float** outArray) {
    if (fileDescriptor < 0) {
        return -1;
    }

    uint32_t arraySize = 0; 
    float* readArray = NULL; 
    int numBytesRead = 0;
    int returnValue = -1;

    // Read array size first
    returnValue = readElement(&arraySize, sizeof(arraySize));
    if (returnValue == -1) {
        return returnValue;
    }
    numBytesRead += returnValue;

    if (arraySize == 0) {
        *outSize = arraySize;
        return numBytesRead;
    }

    // Allocate memory for reading
    readArray = malloc(arraySize * sizeof(float));

    // Read individual floats
    for (uint32_t i = 0; i < arraySize; ++i) {
        returnValue = readElement(&readArray[i], sizeof(readArray[i]));
        if (returnValue == -1) {
            free(readArray);
            return returnValue;
        }
        numBytesRead += returnValue;
    }

    *outSize = arraySize;
    *outArray = readArray;

    return numBytesRead;
}

/**
 * @brief According to the current tty.c_cc[VMIN] setting,
 * read one byte at a time
*/
static int readElement(void* element, const unsigned sizeOfElement) {
    unsigned char* readElement = (unsigned char*) element;

    int returnValue = 0;
    int numBytesRead = 0;

    for(unsigned i = 0; i < sizeOfElement; i++) {
        returnValue = read(fileDescriptor, &readElement[i], sizeof(unsigned char));
        if (returnValue == -1) {
            printf("Error %i from reading to serial port: %s\n", errno, strerror(errno));
            return -1;
        }
        numBytesRead += returnValue;
    }

    return numBytesRead;
}