#define REAL_NUMBER float
#define INTEGER uint16_t
#define DIGITS_PRECISION 3

#include "stats.h"

enum State {
  READING,
  PROCESSING,
};

State state = State::READING;

void readElement(void* element, const size_t sizeOfElement) {
  unsigned char* readElement = (unsigned char*)element;
  for (uint16_t i = 0; i < sizeOfElement; ++i) {
    while (Serial.available() < 1); // Wait for element
    Serial.readBytes(&readElement[i], sizeof(unsigned char));
  }
}

int readArray(const uint16_t arraySize, void* outArray, const size_t sizeOfElement) {
  readElement(outArray, arraySize * sizeOfElement);
}

void writeElement(void* element, const size_t sizeOfElement) {
  while (Serial.availableForWrite() < sizeOfElement); // wait for write
  Serial.write((unsigned char*)element, sizeOfElement);
  Serial.flush(); // wait until data is sent
}


void writeArray(const unsigned arraySize, void* array, const unsigned sizeOfElement) {
  unsigned char* elementIndex = (unsigned char*)array;
  for (unsigned i = 0; i < arraySize; ++i) {
    writeElement(elementIndex, sizeOfElement);
    elementIndex += sizeOfElement;
  }
}

void setup() {

    Serial.begin(9600);
}

uint16_t nbPoints = 0;
uint16_t nbClusters = 0;
uint16_t dimensions = 0;
REAL_NUMBER* data;

void read() {

  if (Serial.available()) {
    readElement(&nbPoints, sizeof(nbPoints));
    readElement(&nbClusters, sizeof(nbClusters));
    readElement(&dimensions, sizeof(dimensions));
    data = (REAL_NUMBER*) malloc(sizeof(REAL_NUMBER) * nbPoints * dimensions);
    readArray(nbPoints * dimensions, data, sizeof(REAL_NUMBER));

    state = State::PROCESSING;
  }
}


void process() {

  INTEGER assignations[nbPoints];
  REAL_NUMBER centroids[nbClusters * dimensions];

  memset(centroids, 0, sizeof(REAL_NUMBER) * nbClusters * dimensions);
  memset(assignations, 0, sizeof(INTEGER) * nbPoints);

  kmeans(data, nbPoints, dimensions, nbClusters, centroids, assignations);
  writeArray(nbClusters * dimensions, centroids, sizeof(REAL_NUMBER));
  writeArray(nbPoints, assignations, sizeof(INTEGER));

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
