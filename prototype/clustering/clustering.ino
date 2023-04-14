#define REAL_NUMBER float
#define INTEGER uint16_t
#define DIGITS_PRECISION 3

#include "stats.h"
#include "arduino_serial_port.hpp"

enum State {
  READING,
  PROCESSING,
};

State state = State::READING;

void setup() {

    Serial.begin(9600);
}

uint16_t nbPoints = 0;
uint16_t nbClusters = 0;
uint16_t dimensions = 0;
real_number* data;

void read() {

  if (Serial.available()) {
    readElement(&nbPoints, sizeof(nbPoints));
    readElement(&nbClusters, sizeof(nbClusters));
    readElement(&dimensions, sizeof(dimensions));
    data = (real_number*) malloc(sizeof(real_number) * nbPoints * dimensions);
    readArray(nbPoints * dimensions, data, sizeof(real_number));

    state = State::PROCESSING;
  }
}


void process() {

  vec_size assignations[nbPoints];
  real_number centroids[nbClusters * dimensions];

  memset(centroids, 0, sizeof(real_number) * nbClusters * dimensions);
  memset(assignations, 0, sizeof(vec_size) * nbPoints);

  kmeans(data, nbPoints, dimensions, nbClusters, centroids, assignations);
  writeArray(nbClusters * dimensions, centroids, sizeof(real_number));
  writeArray(nbPoints, assignations, sizeof(vec_size));

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
