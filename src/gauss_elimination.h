/* This file is part of the 1chipML library. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* The definition below can take the values "float" or "double"
   and defines the precision of the variables used in the Gaussian
   elimination method for systems of linear equations. */

#define gauss_real double

/* Functions are declared below */
gauss_real* gauss_elimination(int n, gauss_real** matrix_a,
                              gauss_real* vector_b);

/* -- End of file -- */
