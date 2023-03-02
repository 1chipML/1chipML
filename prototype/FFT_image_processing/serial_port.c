// C library headers
#include "serial_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linux headers
#include <errno.h>   // Error integer and strerror() function
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

static int setupSerialPort(int serialPort, const speed_t baudRate);
static speed_t getBaudRateCode(const unsigned baudRate);
static int fileDescriptor = -1;
static unsigned serialWriteBufferSize = -1;

/**
 * @brief Opens a serial port for communication.
 * The serial communication is blocking.
 * @param portName The name of the serial port.
 * @param serialBufferSize The buffer size of the serial port.
 * @param baudRate The baud rate for the serial port.
 */
int openSerialPort(char* portName, 
                   const unsigned serialBufferSize,
                   const unsigned baudRate) {
  if (serialBufferSize == 0) {
    printf("Serial buffer size must be greater than 0\n");
    return 1;
  }

  speed_t convertedBaudRate = getBaudRateCode(baudRate);
  if (convertedBaudRate == -1) {
    printf("The baudrate %d is not supported\n", baudRate);
    return 1;
  }

  fileDescriptor = open(portName, O_RDWR | O_NOCTTY);
  if (fileDescriptor < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
    return 1;
  }

  int setupReturnCode = setupSerialPort(fileDescriptor, baudRate);
  if (setupReturnCode != 0) {
    closeSerialPort();
    return setupReturnCode;
  }

  serialWriteBufferSize = serialBufferSize;

  // The Arduino UNO resets when the first connection is created.
  // This prevents from directly sending data to the board when
  // the serial connection is initiated, as it is resetting.
  // This behaviour is linked to the DTR.
  // Wait for a signal from the board
  // In order to bypass this behavior and remove the sleep(),
  // it is possible to attach a 10 uF electrolytic capacitor between
  // the Reset and the Ground on the Arduino UNO board.
  printf("Waiting for connection...\n");
  const unsigned messageLength = 6;
  char readyBuffer[messageLength];
  readElement(readyBuffer, messageLength);
  printf("Successfully connected! Connection message: %s\n", readyBuffer);

  return 0;
}

/**
 * @brief Closes the currently opened serial port.
 */
void closeSerialPort() {
  if (fileDescriptor < 0) {
    return;
  }
  close(fileDescriptor);
  fileDescriptor = -1;
  serialWriteBufferSize = -1;
}

/**
 * @brief Sets up serial port for blocking communication.
 * @param serialPort The file descriptor for the serial port.
 * @param baudRate The baud rate for the serial port.
 * @return 0 if successfull. 1 otherwise.
 */
static int setupSerialPort(int serialPort, const speed_t baudRate) {
  struct termios tty;

  if (tcgetattr(serialPort, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    return 1;
  }

  // set up tty
  // Control modes
  tty.c_cflag &= ~(PARENB | PARODD); // Disable parity
  tty.c_cflag &= ~CSTOPB;            // Only one stop bit
  tty.c_cflag &= ~CSIZE;             // Clear current byte size
  tty.c_cflag |= CS8;                // 8 bits per byte
  tty.c_cflag &= ~CRTSCTS;           // Disable RTS/CTS hardware flow control
  tty.c_cflag |= (CLOCAL | CREAD);   // Ignore modem controls, enable reading

  // Local modes
  tty.c_lflag &= ~ICANON; // Disable canonical mode
  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable newline echo
  tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP

  // Input modes
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
  tty.c_iflag &= ~(IGNBRK | BRKINT | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling
  tty.c_iflag &= ~PARMRK; // Disable parity and framing errors marking

  // Output modes
  // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~OPOST;
  // Prevent conversion of newline to carriage return/line feed
  tty.c_oflag &= ~ONLCR;

  // VMIN and VTIME have no effect in canonical mode
  tty.c_cc[VMIN] = 1;  // Wait until one byte is received
  tty.c_cc[VTIME] = 0; // Wait for up to 0s (Blocking at 0)

  cfsetospeed(&tty, baudRate); // Output baud rate
  cfsetispeed(&tty, baudRate); // Input baud rate

  // Save tty settings
  // Make the change immediatly
  if (tcsetattr(serialPort, TCSAFLUSH, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return 1;
  }

  return 0;
}

/**
 * @brief Read an array of length arraySize, where each element is of size
 * sizeOfElement. This function will block untill all elements are read.
 * @param arraySize The size of the array to read.
 * @param outArray The array in which to store the result of the read data.
 * @param sizeOfElement The size of each element in the array.
 * @return Read return code. 0 if everything went correctly.
 */
int readArray(const unsigned arraySize, 
              void* outArray,
              const unsigned sizeOfElement) {
  int numBytesWritten = readElement(outArray, arraySize * sizeOfElement);
  return numBytesWritten;
}

/**
 * @brief Write an array of length arraySize, where each element is of size
 * sizeOfElement. This function will block untill all elements are written.
 * @param arraySize The size of the array to read.
 * @param array The array containing the data to be written.
 * @param sizeOfElement The size of each element in the array.
 * @return The number of bytes written. -1 if there was an error.
 */
int writeArray(const unsigned arraySize, 
               void* array,
               const unsigned sizeOfElement) {
  return writeElement(array, arraySize * sizeOfElement);
}

/**
 * @brief Read an element of an arbitrary size
 * According to the current tty.c_cc[VMIN] setting,
 * One byte is read at a time to implement blocking behavior
 * This function will block until all elements are read.
 * @param element The element to read.
 * @param sizeOfElement The size of the element to read.
 * @return The number of bytes read. -1 if there was an error.
 */
int readElement(void* element, const unsigned sizeOfElement) {
  if (fileDescriptor < 0) {
    return -1;
  }

  unsigned char* readElement = (unsigned char*)element;
  int numBytesRead = 0;
  int returnValue = -1;

  for (unsigned i = 0; i < sizeOfElement; ++i) {
    returnValue = read(fileDescriptor, &readElement[i], sizeof(unsigned char));
    if (returnValue == -1) {
      printf("Error %i from reading to serial port: %s\n", errno, strerror(errno));
      return returnValue;
    }
    numBytesRead += returnValue;
  }

  return numBytesRead;
}

/**
 * @brief Write an element of an arbitrary size.
 * The number of elements written at once depends on the
 * buffer size.
 * This function will block until the element is written.
 * @param element The element to write.
 * @param sizeOfElement The size of the element to write.
 * @return The number of bytes written. -1 if there was an error.
 */
int writeElement(void* element, const unsigned sizeOfElement) {
  if (fileDescriptor < 0) {
    return -1;
  }

  int numBytesWritten = 0;

  for (unsigned i = 0; i < sizeOfElement; i += serialWriteBufferSize) {

    // Prepare the data to write
    void* elementStartAddress = element + i;
    unsigned sizeToWrite = serialWriteBufferSize;
    if (i + serialWriteBufferSize > sizeOfElement) {
      sizeToWrite = sizeOfElement - i;
    }

    // Write the data
    int returnValue = write(fileDescriptor, elementStartAddress, sizeToWrite);

    // Wait until all output has actually been sent to the terminal device.
    int drainCode = tcdrain(fileDescriptor);
    if (returnValue == -1) {
      printf("Error %i from writing to serial port: %s\n", errno, strerror(errno));
      return returnValue;
    }
    if (drainCode == -1) {
      printf("tcdrain error %i during writing to serial port: %s\n", errno, strerror(errno));
      return drainCode;
    }
    numBytesWritten += returnValue;
  }

  return numBytesWritten;
}

/**
 * @brief Converts the baudrate their speed_t equivalent.
 * @param baudRate The baudrate to convert
 * @return The speed_t equivalent. -1 if the baudrate is not supported
 */
static speed_t getBaudRateCode(const unsigned baudRate) {
  switch (baudRate) {
  case 0:
    return B0;
  case 50:
    return B50;
  case 75:
    return B75;
  case 110:
    return B110;
  case 134:
    return B134;
  case 150:
    return B150;
  case 200:
    return B200;
  case 300:
    return B300;
  case 600:
    return B600;
  case 1200:
    return B1200;
  case 1800:
    return B1800;
  case 2400:
    return B2400;
  case 4800:
    return B4800;
  case 9600:
    return B9600;
  case 19200:
    return B19200;
  case 38400:
    return B38400;

  // Non-Posix
  case 57600:
    return B57600;
  case 115200:
    return B115200;
  case 230400:
    return B230400;
  case 460800:
    return B460800;
  case 500000:
    return B500000;
  case 576000:
    return B576000;
  case 921600:
    return B921600;
  case 1000000:
    return B1000000;
  case 1152000:
    return B1152000;
  case 1500000:
    return B1500000;
  case 2000000:
    return B2000000;
  case 2500000:
    return B2500000;
  case 3000000:
    return B3000000;
  case 3500000:
    return B3500000;
  case 4000000:
    return B4000000;
  default:
    break;
  }

  return -1;
}
