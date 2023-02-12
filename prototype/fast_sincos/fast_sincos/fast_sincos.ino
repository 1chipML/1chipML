#include "fast_sincos.h"

const int degree = 5;
uint32_t start, stop;
volatile float x;
volatile int y;

void setup() {

  Serial.begin(115200);
  while (!Serial);
  Serial.println(__FILE__);
  delay(10);

  // SIN
  start = micros();
  for (float r = -100.0; r < 100.0; r += 0.1)
  {
    x = sin(r);
  }
  stop = micros();
  Serial.print("SIN: \t\t");
  Serial.print(stop - start);
  Serial.println(" us");
  delay(10);

  start = micros();
  for (float r = -100.0; r < 100.0; r += 0.1)
  {
    x = fastSin(r, degree);
  }
  stop = micros();
  Serial.print("FASTSIN: \t");
  Serial.print(stop - start);
  Serial.println(" us");
  delay(10);

  // COS
  start = micros();
  for (float r = -100.0; r < 100.0; r += 0.1)
  {
    x = cos(r);
  }
  stop = micros();
  Serial.print("COS: \t\t");
  Serial.print(stop - start);
  Serial.println(" us");
  delay(10);

  start = micros();
  for (float r = -100.0; r < 100.0; r += 0.1)
  {
    x = fastCos(r, degree);
  }
  stop = micros();
  Serial.print("FASTCOS: \t");
  Serial.print(stop - start);
  Serial.println(" us");
  delay(10);

}

void loop() {
}
