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
 int i;
 int N;
 gauss_real A[2][2];
 gauss_real B[2];
 gauss_real *sol; /* pointer towards the solution */

 /* The following parameters define a system of 2 linear equations A*X=B */
 N=2;
 A[0][0]=+1.; A[0][1]=+1.;
 A[1][0]=+1.; A[1][1]=-1.;
 B[0]=0.;
 B[1]=1.;

 /* Apply gauss elimination method to solve the system */
 sol=gauss_elimination(N,A,B);

 /* Print solution on screen */
 for(i=0;i<N;i++) printf("sol[%d] = %0.3f\n",i,sol[i]);
 return(0);
}

/* -- End of file -- */

