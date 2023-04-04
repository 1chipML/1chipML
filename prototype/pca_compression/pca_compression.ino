#define REAL_NUMBER float
#define INTEGER uint8_t
#define DIGITS_PRECISION 3

#include "jacobi.h"
#include "arduino_serial_port.hpp"

enum State {
  READING,
  PROCESSING,
};

State state = State::READING;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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
  writeArray(totalSize, out, sizeof(REAL_NUMBER));
  writeArray(totalSize, data, sizeof(REAL_NUMBER));

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
