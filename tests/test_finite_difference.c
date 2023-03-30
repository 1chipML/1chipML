#include <finite_difference.h>
#include <math.h>
#include <stdio.h>

#define N 2
#define EPS 1e-6
#define TOL 1e-4
#define CENTRAL_TOL 1e-6

static int isAlmostEqual(const real* value, const real* expected, int n,
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
static real func(real* p) { return pow(p[0] - 3.5, 2) + pow(p[1] + 4, 2); }

// Gradient of (x - 3.5)^2 + (y + 4)^2
static void dfunc(real* p, real* grad) {
  grad[0] = 2 * (p[0] - 3.5);
  grad[1] = 2 * (p[1] + 4);
}

int main() {
  real point[N] = {3, 5};
  real approxGradient[N];
  real gradient[N];
  dfunc(point, gradient);

  printf("The function to derive is (x - 3.5)^2 + (y + 4)^2\n");
  printf("At the point X = %f Y= %f \n", point[0], point[1]);
  printf("The gradient of the function is : X = %f Y = %f\n", gradient[0],
         gradient[1]);

  gradientApproximation(func, point, approxGradient, N, EPS, Forward);
  printf("Using Foward approximation\n");
  printf("The approximate gradient of the function is : X = %f Y = %f\n",
         approxGradient[0], approxGradient[1]);

  if (isAlmostEqual(gradient, approxGradient, N, TOL) == 1) {
    return 1;
  }

  gradientApproximation(func, point, approxGradient, N, EPS, Backward);
  printf("Using Backward approximation\n");
  printf("The approximate gradient of the function is : X = %f Y = %f\n",
         approxGradient[0], approxGradient[1]);

  if (isAlmostEqual(gradient, approxGradient, N, TOL) == 1) {
    return 1;
  }

  gradientApproximation(func, point, approxGradient, N, EPS, Central);
  printf("Using Central approximation\n");
  printf("The approximate gradient of the function is : X = %f Y = %f\n",
         approxGradient[0], approxGradient[1]);

  // Central approximation should be more precise if function is smooth
  if (isAlmostEqual(gradient, approxGradient, N, CENTRAL_TOL) == 1) {
    return 1;
  }

  printf("Success\n");
  return 0;
}
