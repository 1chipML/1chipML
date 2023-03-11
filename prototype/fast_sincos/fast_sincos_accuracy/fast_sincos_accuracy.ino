#include "fast_sincos.h"

uint32_t start, stop;
volatile float x;
volatile int y;

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

  Serial.begin(115200);
  while (!Serial);
  Serial.println(__FILE__);
  delay(10);

  int lowerBound = -100;
  int upperBound = 100;
  int step = 1;
  double multFactor = 0.1;

  Serial.println("SIN");
  getFastSinError(lowerBound, upperBound, step, multFactor, 1);
  delay(10);
  getFastSinError(lowerBound, upperBound, step, multFactor, 2);
  delay(10);
  getFastSinError(lowerBound, upperBound, step, multFactor, 3);
  delay(10);
  getFastSinError(lowerBound, upperBound, step, multFactor, 5);
  delay(10);
  getFastSinError(lowerBound, upperBound, step, multFactor, 7);
  delay(10);

  Serial.println("COS");
  getFastCosError(lowerBound, upperBound, step, multFactor, 1);
  delay(10);
  getFastCosError(lowerBound, upperBound, step, multFactor, 2);
  delay(10);
  getFastCosError(lowerBound, upperBound, step, multFactor, 3);
  delay(10);
  getFastCosError(lowerBound, upperBound, step, multFactor, 5);
  delay(10);
  getFastCosError(lowerBound, upperBound, step, multFactor, 7);
  delay(10);

}

void loop() {
}

static void getFastSinError(int lowerBound, int upperBound, int step, double multFactor, int degree) {
    getGenericError(lowerBound, upperBound, step, multFactor, degree, "FASTSIN", sin, fastSin);
}

static void getFastCosError(int lowerBound, int upperBound, int step, double multFactor, int degree) {
    getGenericError(lowerBound, upperBound, step, multFactor, degree, "FASTCOS", cos, fastCos);
}


static void getGenericError(int lowerBound, int upperBound, int step, double multFactor, int degree, 
char* title, double(*actualFunc)(double), double(*approxFunc)(double, int)) {
    double maxAbsoluteError = 0.0;
    double maxRelativeError = 0.0;

    double avgAbsoluteError = 0.0;
    double avgRelativeError = 0.0;

    for(int i = lowerBound; i < upperBound; i += step) {
        
        double input = i * multFactor;

        double actual = actualFunc(input);
        double approx = approxFunc(input, degree);

        double absoluteError = fabs(actual - approx);
        double relativeError = 0;
        if (actual != 0.0) {
            relativeError = absoluteError / fabs(actual);
        }

        avgAbsoluteError += absoluteError;
        avgRelativeError += relativeError;

        maxAbsoluteError = fmax(maxAbsoluteError, absoluteError);
        maxRelativeError = fmax(maxRelativeError, relativeError);
    }

    int iterations = upperBound - lowerBound;
    if (upperBound < lowerBound) {
        iterations = lowerBound - upperBound;
    }
    if (iterations != 0) {
        avgAbsoluteError /= iterations;
        avgRelativeError /= iterations;
    }

    char buffer[20];
    Serial.println(String(title) + ": Error with degree" + String(degree));
    String precision = dtostre(avgAbsoluteError, buffer, 10, 0);
    Serial.println("Average absolute error = " + precision);
    precision = dtostre(avgRelativeError, buffer, 10, 0);
    Serial.println("Average relative error = " + precision);
    precision = dtostre(maxAbsoluteError, buffer, 10, 0);
    Serial.println("Max absolute error = " + precision);
    precision = dtostre(maxRelativeError, buffer, 10, 0);
    Serial.println("Max relative error = " + precision);
}
