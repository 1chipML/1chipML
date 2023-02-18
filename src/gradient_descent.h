#include <math.h>
#include <stdlib.h>

#define ITMAX 100
#define EPS 1.0e-10
#define GLIMIT 100.0
#define CGOLD 0.3819660
#define GOLD 1.618034 // Put reference to paper as to why this number is there

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

typedef struct Bracket
{
    double a;
    double b;
    double c;
} Bracket;


typedef double (*function)(double[]);
typedef void (*derivative)(double[], double[]);
typedef double (*f1dimension)(function func, double x, double initialPoint[], double direction[], int n);

double gradient_descent(function func, derivative dfunc, double point[], int n, double tol);