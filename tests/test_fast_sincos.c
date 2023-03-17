#include <1chipml.h>

static double getFastSinError(int lowerBound, int upperBound, int step,
                              double multFactor, int degree,
                              int verbose);
static double getFastCosError(int lowerBound, int upperBound, int step,
                              double multFactor, int degree, 
                              int verbose);
static double getGenericError(int lowerBound, int upperBound, 
                int step, double multFactor,
                int degree, char* title, double (*actualFunc)(double),
                fast_sincos_real (*approxFunc)(fast_sincos_real, int), 
                int verbose);
static double GetFixedError(char* title, double (*actualFunc)(double),
                            fast_sincos_real (*approxFunc)(uint32_t),
                            int verbose);

int main() {
  const int verbose = 0;

  int isErrorExpected = 1;

  printf("Fixed point: \n");
  isErrorExpected &= GetFixedError("sine", sin, fastFixedSin, verbose) < 9e-05;
  isErrorExpected &= GetFixedError("cosine", cos, fastFixedCos, verbose) < 9e-05;
  printf("Is error expceted? %d\n", isErrorExpected);

  int lowerBound = -100;
  int upperBound = 100;
  int step = 1;
  fast_sincos_real multFactor = 0.1;

  printf("\n");
  printf("Floating point: \n");
  printf("Sine \n");
  isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 1, verbose) < 9e-03;
  isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 2, verbose) < 9e-05;
  isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 3, verbose) < 9e-05;
  isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 5, verbose) < 9e-07;
  isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 7, verbose) < 9e-08;
  printf("Is error expceted? %d\n", isErrorExpected);
  printf("Cosine \n");
  isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 1, verbose) < 9e-03;
  isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 2, verbose) < 9e-05;
  isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 3, verbose) < 9e-05;
  isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 5, verbose) < 9e-07;
  isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 7, verbose) < 9e-08;
  printf("Is error expceted? %d\n", isErrorExpected);

  return isErrorExpected ? 0 : 1;
}

static double getFastSinError(int lowerBound, int upperBound, int step,
                              double multFactor, int degree, int verbose) {
  return getGenericError(lowerBound, upperBound, step, multFactor, degree,
                         "FASTSIN", sin, fastSin, verbose);
}

static double getFastCosError(int lowerBound, int upperBound, int step,
                              double multFactor, int degree, int verbose) {
  return getGenericError(lowerBound, upperBound, step, multFactor, degree,
                         "FASTCOS", cos, fastCos, verbose);
}

static double
getGenericError(int lowerBound, int upperBound, int step, double multFactor,
                int degree, char* title, double (*actualFunc)(double),
                fast_sincos_real (*approxFunc)(fast_sincos_real, int),
                int verbose) {
  
  double maxAbsoluteError = 0.0;
  double maxRelativeError = 0.0;

  double avgAbsoluteError = 0.0;
  double avgRelativeError = 0.0;

  for (int i = lowerBound; i < upperBound; i += step) {

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

  if (verbose) {
    printf("%s: Error with degree %d\n", title, degree);
    printf("Average absolute error = %.10e\n", avgAbsoluteError);
    printf("Average relative error = %.10e\n", avgRelativeError);
    printf("Max absolute error = %.10e\n", maxAbsoluteError);
    printf("Max relative error = %.10e\n", maxRelativeError);
  }

  return avgAbsoluteError;
}

static double GetFixedError(char* title, double (*actualFunc)(double),
                            fast_sincos_real (*approxFunc)(uint32_t),
                            int verbose) {
  
  double maxAbsoluteError = 0.0;
  double avgAbsoluteError = 0.0;

  const uint32_t upperBound = FIXED_2_PI;
  for (uint32_t i = 0; i < upperBound; i += 20) { // < 2^25
    double actualInput = (double)i * (2.0 * M_PI) / FIXED_2_PI;

    double actual = actualFunc(actualInput);
    double approx = approxFunc(i);

    double absoluteError = fabs(actual - approx);
    avgAbsoluteError += absoluteError;
    maxAbsoluteError = fmax(maxAbsoluteError, absoluteError);
  }

  avgAbsoluteError /= (double)upperBound;

  if (verbose) {
    printf("Fixed %s error:\n", title);
    printf("Average absolute error = %.10e\n", avgAbsoluteError);
    printf("Max absolute error = %.10e\n", maxAbsoluteError);
  }

  return avgAbsoluteError;
}
