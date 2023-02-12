#include "fast_sincos.h"

uint32_t start, stop;
volatile float x;
volatile int y;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.println(__FILE__);
  delay(10);

  start = micros();
  for (float r = -50.0; r < 50.0; r += 0.1)
  {
    x = sin(r);
  }
  stop = micros();
  Serial.print("SIN: \t\t");
  Serial.print(stop - start);
  Serial.println(" us");
  delay(10);


  start = micros();
  for (float r = -50.0; r < 50.0; r += 0.1)
  {
    x = fastSin(r);
  }
  stop = micros();
  Serial.print("ISIN: \t\t");
  Serial.print(stop - start);
  Serial.println(" us");
  delay(10);

}

void loop() {
  // put your main code here, to run repeatedly:

}
