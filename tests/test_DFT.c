#include <1chipml.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static int compareFT(const unsigned length, dft_real* incomingReals, 
  dft_real* incomingImgs, dft_real* expectedReals, dft_real* expectedImgs) {

  const dft_real epsilon = 1e-20;

  for (unsigned i = 0; i < length; ++i) {
    if (abs(incomingReals[i] - expectedReals[i]) > epsilon ||
        abs(incomingImgs[i] - expectedImgs[i]) > epsilon) {
      return -1;
    }
  }

  return 0;
}

static void knownTestingDFT(const unsigned length,
  fft_real* inputReals,
  fft_real* inputImaginaries,
  fft_real* expectedReals, 
  fft_real* expectedImaginaries) {

  DFT(length, inputReals, inputImaginaries, 1);

  int isSimilar = compareFT(length, inputReals, inputImaginaries, expectedReals, expectedImaginaries);

  printf("Known testing DFT: is the DFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");

}

static void knownTestingDFTI(const unsigned length,
  fft_real* inputReals,
  fft_real* inputImaginaries,
  fft_real* expectedReals, 
  fft_real* expectedImaginaries) {

  DFT(length, inputReals, inputImaginaries, -1);

  int isSimilar = compareFT(length, inputReals, inputImaginaries, expectedReals, expectedImaginaries);

  printf("Known testing inverse DFT: is the DFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");

}

int main() {

  const unsigned length = 4;
  fft_real DFTinputReals[] = {8, 4, 8, 0};
  fft_real DFTinputImgs[] = {0, 0, 0, 0};
  fft_real DFTexpectedReals[] = {20, 0, 12, 0};
  fft_real DFTexpectedImgs[] = {0, -4, 0, 4};
  knownTestingDFT(length, DFTinputReals, DFTinputImgs, DFTexpectedReals, DFTexpectedImgs);

  fft_real DFTIinputReals[] = {20, 0, 12, 0};
  fft_real DFTIinputImgs[] = {0, -4, 0, 4};
  fft_real DFTIexpectedReals[] = {8, 4, 8, 0};
  fft_real DFTIexpectedImgs[] = {0, 0, 0, 0};
  knownTestingDFTI(length, DFTIinputReals, DFTIinputImgs, DFTIexpectedReals, DFTIexpectedImgs);

  return 0;
}
