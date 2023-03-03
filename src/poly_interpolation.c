/* This file is part of the 1chipML library. */

#include "poly_interpolation.h"

/*
  This function implements the polynomial interpolation or extrapolation
  using Neville's algorithm.
  */
int poly_interpolation(poly_real x_a[], poly_real y_a[], unsigned int n,
                       poly_real x, poly_real* y, poly_real* error) {
  unsigned int nearest_index = 0;

  /* Change input arrays to unit-offset. */
  poly_real* px_a = x_a - 1;
  poly_real* py_a = y_a - 1;

  /* Create c and d vector to calculate differences c and d between
    2 parents and 1 child. */
  poly_real* c = (poly_real*)malloc((n + 1) * sizeof(poly_real));
  poly_real* d = (poly_real*)malloc((n + 1) * sizeof(poly_real));

  /* Set the index as boundary if it is an extrapolation problem. */
  if (x <= px_a[1]) {
    nearest_index = 1;
  } else if (x >= px_a[n]) {
    nearest_index = n;
  }

  for (unsigned int i = 1; i <= n; i++) {
    /* Initialize the tableau of c and d as y_a. */
    *(c + i) = py_a[i];
    *(d + i) = py_a[i];

    /* If the input x is in measured data, just return the measured value. */
    if (x == px_a[i]) {
      *y = py_a[i];
      *error = 0;
      return 0;
    }

    /* Find the index as the closest table entry if it is the
      interpolation problem. */
    if (nearest_index == 0 && i < n && px_a[i] < x && x < px_a[i + 1]) {
      nearest_index = i;
    }
  }

  /* Get initial approximation of y. */
  *y = py_a[nearest_index--];

  /* For each column of the tableau, we loop over c and d and update them. */
  for (unsigned int column = 1; column < n; column++) {
    for (unsigned int i = 1; i <= n - column; i++) {
      if (px_a[i] == px_a[i + column]) {
        return -1;
      }

      /* Calculate partial of c, d. */
      poly_real part_c = px_a[i] - x;
      poly_real part_d = px_a[i + column] - x;
      poly_real common = (c[i + 1] - d[i]) / (px_a[i] - px_a[i + column]);

      /* Update c, d. */
      c[i] = part_c * common;
      d[i] = part_d * common;
    }

    /* Decide which correction we want to add to y. The last error is the error
     * indication. */
    *error = (nearest_index << 1) < (n - column) ? c[nearest_index + 1]
                                                 : d[nearest_index--];
    *y += *error;
  }

  /* Free the memory. */
  free(c);
  free(d);

  return 0;
}

/* -- End of file -- */
