#include "fast_sincos.h"

uint32_t start, stop;
volatile float x;

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
    Serial.println(String(title) + ": Error with degree " + String(degree));
    String precision = dtostre(avgAbsoluteError, buffer, 10, 0);
    Serial.println("Average absolute error = " + precision);
    precision = dtostre(avgRelativeError, buffer, 10, 0);
    Serial.println("Average relative error = " + precision);
    precision = dtostre(maxAbsoluteError, buffer, 10, 0);
    Serial.println("Max absolute error = " + precision);
    precision = dtostre(maxRelativeError, buffer, 10, 0);
    Serial.println("Max relative error = " + precision);
}
