#include <math.h>
#include <stdlib.h>

#define ITMAX 100
#define ZEPS 1.0e-10
#define TINY 1.0e-20
#define TOL 2.0e-4
#define EPS 1.0e-10
#define GLIMIT 100.0
#define CGOLD 0.3819660
#define GOLD 1.618034 // Put reference to paper as to why this number is there

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))

struct f1DimParam {
    int ncom;
    double *pcom;
    double *xicom;
    double (*nrfunc)(double[]);
};

typedef struct MinBracket
{
    double a;
    double b;
    double c;
} MinBracket;


typedef double (*function)(double[]);
typedef void (*derivative)(double[], double[]);

double gradient_descent(function func, derivative dfunc, double point[], int n);