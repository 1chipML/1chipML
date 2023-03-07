/* This file is part of the 1chipML library. */

#include "linear_congruential_random_generator.h"

/* The routines below implements the linear congruential random
   number generator. In particular, the function
   "set_linear_congruential_generator_seed" sets the seed of the random
   generator, while the function "linear_congruential_random_generator"
   returns a pseudo-random real number in the range [0, 1.0]. */

/* Variables and pointers declarations */
uint16_t ISEED = 16; /* Default value in case the user does not specify the seed */
#define LINEAR_RAND_MAX pow(2, sizeof(ISEED) * 8)

void set_linear_congruential_generator_seed(uint16_t num) { ISEED = num; }

lcrg_real linear_congruential_random_generator() {
  ISEED = 75 * ISEED + 74;
  return ISEED * (1.0 / LINEAR_RAND_MAX);
}

/* -- End of file -- */
