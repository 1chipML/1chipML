/* ************************************************* *
 * Author        : Qinan Qi                          *
 * Created       : 06 May 2022, Montreal, QC, Canada *
 * Last modified : 06 May 2022, Montreal, QC, Canada *
 * ************************************************* */
#include "../src/poly_interpolation.h"

int test_poly(poly_real x_a[], poly_real y_a[], unsigned int n, poly_real x,
              poly_real y_true) {
  poly_real y = 0., error = 0.;

  int isSuccessful = 0;
  /* Print solution on screen */
  if (poly_interpolation(x_a, y_a, n, x, &y, &error) == 0 &&
      round(y * 1000) == round(y_true * 1000)) {
    printf("Success: ");
  } else {
    printf("Fail: ");
    isSuccessful = 1;
  }
  printf("x = %0.3f, y = %0.3f, error indication = %0.3f, y_true = %0.3f\n", x,
         y, error, y_true);

  return isSuccessful;
}

int main(void) {
  /* Define measured data. */
  poly_real x_a[] = {2., 4., 6.};
  poly_real y_a[] = {4., 16., 36.};
  int isSuccessful = 0;

  /* Apply polynomial interpolation to find the y */
  isSuccessful |= test_poly(x_a, y_a, 3, 3., 9.);
  isSuccessful |= test_poly(x_a, y_a, 3, 5.5, 30.25);

  /* Apply polynomial extrapolation to find the y */
  isSuccessful |= test_poly(x_a, y_a, 3, 7.5, 56.25);
  isSuccessful |= test_poly(x_a, y_a, 3, 1., 1);
  isSuccessful |= test_poly(x_a, y_a, 3, -2., 4);

  /* Define measured data. */
  poly_real x_a2[] = {-1.5, -0.2, 1., 5., 10.};
  poly_real y_a2[] = {-1.2, 0., 0.5, 1., 1.2};

  /* Apply polynomial interpolation to find the y */
  isSuccessful |= test_poly(x_a2, y_a2, 5, -1., -0.632);
  isSuccessful |= test_poly(x_a2, y_a2, 5, 3., 0.776);

  /* Apply polynomial extrapolation to find the y */
  isSuccessful |= test_poly(x_a2, y_a2, 5, -3., -4.024);
  isSuccessful |= test_poly(x_a2, y_a2, 5, 12., -1.082);

  return isSuccessful;
}

/* -- End of file -- */
