/* This file is part of the 1chipML library. */

#include "gauss_elimination.h"

/* The routine below implements the Gauss elimination method
   to solve a system of n linear equations of the type matrix_a*x=vector_b.
   Inputs: n, matrix_a[][], vector_b[].
   Outputs: pointer to the solution array x[] */
gauss_real* gauss_elimination(int n, gauss_real** matrix_a,
                              gauss_real* vector_b) {

  /* Variables and pointers declarations */
  int i, j, k;
  int sum;
  int max;
  int indx;
  int count = 0;
  gauss_real r;
  gauss_real* x;
  gauss_real* tmp;
  gauss_real** a;

  /* Memory allocation */
  a = (gauss_real**)malloc(n * sizeof(gauss_real*));
  for (i = 0; i < n; i++)
    a[i] = (gauss_real*)malloc((n + 1) * sizeof(gauss_real));

  x = (gauss_real*)malloc(n * sizeof(gauss_real));

  /* Load the matrix a[][], the matrix a[][] is an n*(n+1) matrix which reads
   * [matrix_a, matrix_b] */
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++)
      a[i][j] = matrix_a[i][j];
    a[i][j] = vector_b[i];
  }

  /* Check if the equations satisfy the diagonal dominance condition */
  i = 0;
  while (i < n) {
    sum = 0;
    max = abs(a[i][i]);
    for (j = 0; j < n; j++) {
      if (i != j) {
        sum = sum + abs(a[i][j]);
        if (abs(a[i][j]) >= max) {
          max = abs(a[i][j]);
          indx = j;
        }
      }
    }
    /* If any equation does not satisfy the diagonal dominance condition then
     * re-order */
    if (abs(a[i][i]) < sum) {
      tmp = a[i];
      a[i] = a[indx];
      a[indx] = tmp;
      count++; /* This variables counts the number of re-ordered equations */
    } else {
      i++;
    }
    if (count == n - 1)
      break;
  }

  /* Apply the Gauss elimination method */
  for (i = 0; i < n - 1; i++) {
    for (j = i + 1; j < n; j++) {
      r = a[j][i] / a[i][i];
      for (k = 0; k < n + 1; k++)
        a[j][k] = a[j][k] - r * a[i][k];
    }
  }

  /* Apply back-tracking */
  x[n - 1] = a[n - 1][n] / a[n - 1][n - 1];

  for (i = n - 2; i >= 0; i--) {
    x[i] = a[i][n - 1];
    for (j = i; j < n; j++) {
      x[i] = x[i] - a[i][j] * x[j];
    }
    x[i] = x[i] / a[i][i];
  }

  /* Return solution */
  return (x);
}

/* -- End of file -- */
