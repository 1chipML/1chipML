#include "fast_sincos.h"

int degree = 2;
uint32_t totalTime = 0;
int iterator = 0;
volatile float x;
volatile int y;

void setup() {

  Serial.begin(115200);
  while (!Serial);
  Serial.println(__FILE__);
  delay(10);

  constexpr int lowerBound = -100;
  constexpr int upperBound = 100;
  constexpr int step = 1;
  constexpr float factor = 0.1;
  constexpr float iterations = (upperBound - lowerBound) / step;

  // SIN
  totalTime = 0;
  iterator = lowerBound;
  while (iterator < upperBound) {
    float input = iterator * factor;

    uint32_t start = micros();
    x = sin(input);
    uint32_t stop = micros();

    totalTime += stop - start;
    iterator += step;
  }

  Serial.print("SIN: \t\t");
  Serial.print(totalTime / iterations);
  Serial.println(" us");
  delay(10);

  totalTime = 0;
  iterator = lowerBound;
  while (iterator < upperBound) {
    float input = iterator * factor;

    uint32_t start = micros();
    x = fastSin(input, degree);
    uint32_t stop = micros();

    totalTime += stop - start;
    iterator += step;
  }

  Serial.print("FASTSIN: \t");
  Serial.print(totalTime / iterations);
  Serial.println(" us");
  delay(10);

  // COS
  totalTime = 0;
  iterator = lowerBound;
  while (iterator < upperBound) {
    float input = iterator * factor;

    uint32_t start = micros();
    x = cos(input);
    uint32_t stop = micros();

    totalTime += stop - start;
    iterator += step;
  }

  Serial.print("COS: \t\t");
  Serial.print(totalTime / iterations);
  Serial.println(" us");
  delay(10);

  totalTime = 0;
  iterator = lowerBound;
  while (iterator < upperBound) {
    float input = iterator * factor;

    uint32_t start = micros();
    x = fastCos(input, degree);
    uint32_t stop = micros();

    totalTime += stop - start;
    iterator += step;
  }

  Serial.print("FASTCOS: \t");
  Serial.print(totalTime / iterations);
  Serial.println(" us");
  delay(10);

}

void loop() {
}
