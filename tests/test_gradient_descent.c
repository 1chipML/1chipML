#include <gradient_descent.h>
#include <math.h>
#include <stdio.h>

#define TOL 2.0e-4
#define ITMAX 10
#define N 2

static int isAlmostEqual(const gradient_real* value,
                         const gradient_real* expected, int n,
                         double tolerance) {

  for (int i = 0; i < n; i++) {
    if (fabs(value[i] - expected[i]) > tolerance) {
      printf("Fail: Expected %f got %f\n", expected[i], value[i]);
      return 1;
    }
  }

  return 0;
}

// Function (x - 3.5)^2 + (y + 4)^2
static gradient_real func(gradient_real* p) {
  return pow(p[0] - 3.5, 2) + pow(p[1] + 4, 2);
}

// Gradient of (x - 3.5)^2 + (y + 4)^2
static void dfunc(gradient_real* p, gradient_real* grad) {
  grad[0] = 2 * (p[0] - 3.5);
  grad[1] = 2 * (p[1] + 4);
}

int main() {
  gradient_real min = 0.0;
  gradient_real point[N] = {3, 5};
  gradient_real expected[N] = {3.5, -4.0};

  int iterations = ITMAX;

  int status = gradient_descent(func, dfunc, &min, point, N, TOL, &iterations);

  if (status == GRADIENT_ERROR) {
    return 1;
  }

  printf("The function to minimize is (x - 3.5)^2 + (y + 4)^2\n");
  printf("The minimum of the function is %f\n", min);
  printf("X: %f\n", point[0]);
  printf("Y: %f\n", point[1]);

  if (isAlmostEqual(point, expected, N, TOL) == 1) {
    return 1;
  }

  printf("Success\n");
  return 0;
}
