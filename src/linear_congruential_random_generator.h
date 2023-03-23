/* This file is part of the 1chipML library. */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* The definition below can take the values "float" or "double"
   and defines the precision of the variables used in the
   linear congruential random number generator. */

#define lcrg_real double

/* Functions are defined below */
void set_linear_congruential_generator_seed(uint16_t num);
lcrg_real linear_congruential_random_generator();

/* -- End of file -- */
