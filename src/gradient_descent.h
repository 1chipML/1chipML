#include <math.h>
#include <stdlib.h>

#ifndef REAL_NUMBER
#define REAL_NUMBER double
#endif

typedef REAL_NUMBER gradient_real;

#ifndef EPS
#define EPS 1.0e-10
#endif

// Limit the number of iteration in Brent's routine for lineSearch
// Based on :
// http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c10-2.pdf
// p.404
#ifndef ITMAX_BRENT
#define ITMAX_BRENT 100
#endif

// Limit the travel distance to the parabolic inflexion point on the initial
// minimum bracketing Based on :
// http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c10-1.pdf
// p.400
#ifndef GLIMIT
#define GLIMIT 100.0
#endif

// Golden ratio constants used for Golden Search in Brent's routine for
// lineSearch Based on :
// http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c10-2.pdf
// p.404
#define CGOLD 0.3819660
#define GOLD 1.618034

#define GRADIENT_ERROR 1
#define GRADIENT_SUCCESS 0

#define SIGN(a, b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

typedef struct Bracket {
  gradient_real a;
  gradient_real b;
  gradient_real c;
} Bracket;

typedef gradient_real (*function)(gradient_real[]);
typedef void (*derivative)(gradient_real[], gradient_real[]);
typedef gradient_real (*f1dimension)(function func, gradient_real x,
                                     gradient_real initialPoint[],
                                     gradient_real direction[], int n);

#ifdef __cplusplus
extern "C" {
#endif

int gradient_descent(function func, derivative dfunc, gradient_real *min,
                     gradient_real guess[], int n, gradient_real tol,
                     int *iterations);

#ifdef __cplusplus
}
#endif