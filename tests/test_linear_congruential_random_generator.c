/* ************************************************* *
 * Author        : Jean Michel Sellier               *
 * Created       : 08 Feb 2022, Montreal, QC, Canada *
 * Last modified : 14 Feb 2022, Montreal, QC, Canada *
 * ************************************************* */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include "../src/1chipml.h"

int main(void){
 /* Variables and pointers declarations */
 int i,n=100;

 printf("linear congruential random generator\n");
 for(i=0;i<n;i++) printf("%1.3f ",linear_congruential_random_generator());
 printf("\n");
 return(0);
}

/* -- End of file -- */

