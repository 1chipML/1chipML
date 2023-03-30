#ifndef REAL_NUMBER
#define REAL_NUMBER double
#endif

typedef REAL_NUMBER real;

typedef real (*function)(real[]);

typedef enum { Forward, Backward, Central } approximationType;

#ifdef __cplusplus
extern "C" {
#endif

void gradientApproximation(function func, real point[], real grad[], int n,
                           real eps, approximationType type);

#ifdef __cplusplus
}
#endif
