#include <stdio.h>
#include <math.h>

#include "../src/1chipml.h"

#define TOL 2.0e-4

// Function (x)^4 + (y)^4 - x + y^3
double func(double* p) {
    return pow(p[0], 4) + pow(p[1], 4) - p[0] + pow(p[1], 3);
}

// Gradient of (x)^4 + (y)^4 - x + y^3
void dfunc(double* p, double* grad) {
    grad[0] = 4 * pow(p[0], 3) - 1; 
    grad[1] = 4 * pow(p[1], 3) + 3 * pow(p[1], 2);
}

int main() {
    double min = 0;
    double p[2] = {0, -0.2};

    min = gradient_descent(func, dfunc, p, 2, TOL);
    
    printf("Minimizing function: (x)^4 + (y)^4 - x + y^3\n");
    printf("The minimum of the function is %f\n", min);
    printf("X: %f\n", p[0]);
    printf("Y: %f\n", p[1]);
}