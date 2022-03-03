/* This file is part of the microML library. */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/* The definition below can take the values "float" or "double"
   and defines the precision of the variables used in the
   linear congruential random number generator. */

#define lcrg_real double

/* Functions are defined below */
void set_linear_congruential_generator_seed(int num);
lcrg_real linear_congruential_random_generator(void);

/* -- End of file -- */

