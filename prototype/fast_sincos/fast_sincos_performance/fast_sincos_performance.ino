#include "fast_sincos.h"

int degree = 1;
uint32_t totalTime = 0;
int iterator = 0;
volatile float x;

const uint16_t readTable(const uint16_t* address) {
  return pgm_read_word_near(address);
}

static const uint16_t sineTable[129] PROGMEM = {
    0,  804, 1608, 2412, 3216, 4019, 4821, 5623,
 6424, 7223, 8022, 8820, 9616,10411,11204,11996,
12785,13573,14359,15142,15924,16703,17479,18253,
19024,19792,20557,21319,22078,22834,23586,24334,
25079,25820,26557,27291,28020,28745,29465,30181,
30893,31600,32302,32999,33692,34379,35061,35738,
36409,37075,37736,38390,39039,39682,40319,40950,
41575,42194,42806,43411,44011,44603,45189,45768,
46340,46905,47464,48014,48558,49095,49624,50145,
50659,51166,51664,52155,52638,53113,53580,54039,
54490,54933,55367,55794,56211,56620,57021,57413,
57797,58171,58537,58895,59243,59582,59913,60234,
60546,60850,61144,61429,61704,61970,62227,62475,
62713,62942,63161,63371,63571,63762,63943,64114,
64276,64428,64570,64703,64826,64939,65042,65136,
65219,65293,65357,65412,65456,65491,65515,65530,
65535
};

void setup() {

  // Set for PROGMEM
  setTableAccessFunction(readTable);

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
