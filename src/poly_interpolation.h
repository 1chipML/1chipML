/* This file is part of the 1chipML library. */
#ifndef _BASE_LIB_
#define _BASE_LIB_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#endif

/*
 * The definition below can take the values "float" or "double"
 * and defines the precision of the variables used in the polynomial
 * interpolation and extrapolation method.
 */

#define poly_real double

/* Functions are declared below */

/*
 * This function implements the polynomial interpolation or extrapolation
 * using Neville's algorithm. Polynomial approach is only suitable for small
 * amount of measured data.
 * Input:
 *	x_a: Measured x values. Those value has to be ordered.
 *	y_a: Measured y values pairing with x values.
 *	n: The number of measured data.
 *	x: The x value of the data to be interpolated/extrapolated.
 *	y: Return the y value of the data to be interpolcated/extrapolated.
 *	error: Return the error estimation.
 * Return:
 *	0 as success, and -1 as error.
 */
int poly_interpolation(poly_real x_a[], poly_real y_a[], unsigned int n,
                       poly_real x, poly_real* y, poly_real* error);

/* -- End of file -- */
