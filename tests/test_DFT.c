#include <1chipml.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static int compareFT(const unsigned length, fft_real* incomingReals, 
  fft_real* incomingImgs, fft_real* expectedReals, fft_real* expectedImgs) {

  const fft_real epsilon = 1e-20;

  for (unsigned i = 0; i < length; ++i) {
    if (abs(incomingReals[i] - expectedReals[i]) > epsilon ||
        abs(incomingImgs[i] - expectedImgs[i]) > epsilon) {
      return -1;
    }
  }

  return 0;
}

static void knownTestingDFT() {
  unsigned length = 4;

  // allocate arrays for testing
  fft_real dftReals[] = {8, 4, 8, 0};
  fft_real dftImgs[] = {0, 0, 0, 0};
  fft_real expectedReals[] = {20, 0, 12, 0};
  fft_real expectedImgs[] = {0, -4, 0, 4};

  DFT(length, dftReals, dftImgs, 1);

  int isSimilar = compareFT(length, dftReals, dftImgs, expectedReals, expectedImgs);

  printf("Known testing DFT: is the DFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");

}

static void knownTestingDFTI() {
  unsigned length = 4;

  // allocate arrays for testing
  fft_real dftReals[] = {20, 0, 12, 0};
  fft_real dftImgs[] = {0, -4, 0, 4};
  fft_real expectedReals[] = {8, 4, 8, 0};
  fft_real expectedImgs[] = {0, 0, 0, 0};

  DFT(length, dftReals, dftImgs, -1);

  int isSimilar = compareFT(length, dftReals, dftImgs, expectedReals, expectedImgs);

  printf("Known testing inverse DFT: is the DFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");

}

int main() {

  knownTestingDFT();
  knownTestingDFTI();

  return 0;
}
