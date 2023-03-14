#define REAL_NUMBER float
#define INTEGER uint8_t
#define DIGITS_PRECISION 3

#include "jacobi.h"

enum State {
  READING,
  PROCESSING,
};

State state = State::READING;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void readElement(void* element, const size_t sizeOfElement) {
  unsigned char* readElement = (unsigned char*)element;
  for (uint32_t i = 0; i < sizeOfElement; ++i) {
    while (Serial.available() < 1); // Wait for element
    Serial.readBytes(&readElement[i], sizeof(unsigned char));
  }
}

int readArray(const uint32_t arraySize, void* outArray, const size_t sizeOfElement) {
  readElement(outArray, arraySize * sizeOfElement);
}

void writeElement(void* element, const size_t sizeOfElement) {
  while (Serial.availableForWrite() < sizeOfElement); // wait for write
  Serial.write((unsigned char*)element, sizeOfElement);
  Serial.flush(); // wait until data is sent
}

void writeRealNumberArray(const uint32_t arraySize, REAL_NUMBER* array, const size_t sizeOfElement) {
  for (uint32_t i = 0; i < arraySize; ++i) {
    writeElement(&array[i], sizeOfElement);
  }
}

uint32_t size = 0;
uint32_t totalSize = 0;
REAL_NUMBER* data;

void read() {
 
  if (Serial.available()) {

    readElement(&size, sizeof(size));
    totalSize = size * size;
    data = (REAL_NUMBER*) malloc(sizeof(REAL_NUMBER) * totalSize);
    readArray(totalSize, data, sizeof(REAL_NUMBER));
    state = State::PROCESSING;
  }
}


void process() {
  INTEGER sizes[3] = {size, size, size};
  REAL_NUMBER out[totalSize];

  // Compute eigenvectors and eigenvalues
  jacobi(data, size, out, -1, 0);

  // Send back matrices to host
  writeRealNumberArray(totalSize, out, sizeof(REAL_NUMBER));
  writeRealNumberArray(totalSize, data, sizeof(REAL_NUMBER));

  free(data);
  state = State::READING;
}

void loop() {
  switch (state) {
    case State::READING:
      read();
      break;
    case State::PROCESSING:
      process();
      break;
  }

}
