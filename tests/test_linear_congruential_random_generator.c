/* ************************************************* *
 * Author        : Jean Michel Sellier               *
 * Created       : 08 Feb 2022, Montreal, QC, Canada *
 * Last modified : 14 Feb 2022, Montreal, QC, Canada *
 * ************************************************* */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/1chipml.h"

int main(void) {
  /* Variables and pointers declarations */
  int i, n = 100;

  printf("linear congruential random generator\n");
  for (i = 0; i < n; i++) {
    const double value = linear_congruential_random_generator();

    if (fabs(value) > 1) {
      printf("Error! : Generated number is not between 0 and 1 is %f \n",
             value);
      return 1;
    }
  }
  printf("Success : linear_congruential_random_generator \n");

  return 0;
}

/* -- End of file -- */
