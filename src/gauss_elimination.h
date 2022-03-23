/* This file is part of the 1chipML library. */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/* The definition below can take the values "float" or "double"
   and defines the precision of the variables used in the Gaussian
   elimination method for systems of linear equations. */

#define gauss_real double

/* Functions are declared below */
gauss_real* gauss_elimination(int n,gauss_real matrix_a[n][n],gauss_real vector_b[n]);

/* -- End of file -- */
