#include <math.h>
#include <stdlib.h>

typedef double gradient_real;

#define EPS 1.0e-10

// Limit the number of iteration in Brent's routine for lineSearch
// Based on :
// http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c10-2.pdf
// p.404
#define ITMAX_BRENT 100

// Limit the travel distance to the parabolic inflexion point on the initial
// minimum bracketing Based on :
// http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c10-1.pdf
// p.400
#define GLIMIT 100.0

// Golden ratio constants used for Golden Search in Brent's routine for
// lineSearch Based on :
// http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c10-2.pdf
// p.404
#define CGOLD 0.3819660
#define GOLD 1.618034

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

gradient_real gradient_descent(function func, derivative dfunc,
                               gradient_real guess[], int n, gradient_real tol,
                               int itMax);