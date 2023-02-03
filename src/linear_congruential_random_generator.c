/* This file is part of the 1chipML library. */

#include "linear_congruential_random_generator.h"

/* The routines below implements the linear congruential random
   number generator. In particular, the function
   "set_linear_congruential_generator_seed" sets the seed of the random
   generator, while the function "linear_congruential_random_generator"
   returns a pseudo-random real number in the range [0, 1.0]. */

/* Variables and pointers declarations */
int ISEED = 16.; /* Default value in case the user does not specify the seed */

/* Set the seed for the random number generator */
void set_linear_congruential_generator_seed(int num) { ISEED = num; }

/* Linear congruential random number generator */
lcrg_real linear_congruential_random_generator(void) {
  ISEED = fmod(75. * ISEED + 74., 65537.);
  return (ISEED / 65537.);
}

/* -- End of file -- */
