#include <1chipml.h>

static double getFastSinError(int lowerBound, int upperBound, int step, double multFactor, int degree);
static double getFastCosError(int lowerBound, int upperBound, int step, double multFactor, int degree);
static double getGenericError(int lowerBound, int upperBound, int step, double multFactor, int degree, 
char* title, double(*actualFunc)(double), fast_sincos_real(*approxFunc)(fast_sincos_real, int));

int main() {
    int isErrorExpected = 1;

    int lowerBound = -100;
    int upperBound = 100;
    int step = 1;
    fast_sincos_real multFactor = 0.1;

    printf("Sine \n");
    isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 1) < 9e-03;
    isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 2) < 9e-05;
    isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 3) < 9e-05;
    isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 5) < 9e-07;
    isErrorExpected &= getFastSinError(lowerBound, upperBound, step, multFactor, 7) < 9e-08;
    printf("Cosine \n");
    isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 1) < 9e-03;
    isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 2) < 9e-05;
    isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 3) < 9e-05;
    isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 5) < 9e-07;
    isErrorExpected &= getFastCosError(lowerBound, upperBound, step, multFactor, 7) < 9e-08;
    printf("Is error expcted? %d\n", isErrorExpected);

    return isErrorExpected ? 0 : 1;
}

static double getFastSinError(int lowerBound, int upperBound, int step, double multFactor, int degree) {
    return getGenericError(lowerBound, upperBound, step, multFactor, degree, "FASTSIN", sin, fastSin);
}

static double getFastCosError(int lowerBound, int upperBound, int step, double multFactor, int degree) {
    return getGenericError(lowerBound, upperBound, step, multFactor, degree, "FASTCOS", cos, fastCos);
}


static double getGenericError(int lowerBound, int upperBound, int step, double multFactor, int degree, 
char* title, double(*actualFunc)(double), fast_sincos_real(*approxFunc)(fast_sincos_real, int)) {
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

    printf("%s: Error with degree %d\n", title, degree);
    printf("Average absolute error = %.10e\n", avgAbsoluteError);
    printf("Average relative error = %.10e\n", avgRelativeError);
    printf("Max absolute error = %.10e\n", maxAbsoluteError);
    printf("Max relative error = %.10e\n", maxRelativeError);

    return avgAbsoluteError;
}
