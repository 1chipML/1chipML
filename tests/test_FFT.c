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

static void randomTestingFFT() {

  // limit length to 8 for faster testing
  const unsigned length = (unsigned)pow(2, rand() % 9);

  // allocate arrays for testing
  fft_real fftReals[length];
  fft_real fftImgs[length];
  fft_real dftReals[length];
  fft_real dftImgs[length];

  for (unsigned i = 0; i < length; ++i) {
    fftReals[i] = (float)rand() / RAND_MAX;
    fftImgs[i] = 0;
    dftReals[i] = fftReals[i];
    dftImgs[i] = fftImgs[i];
  }

  clock_t start, end;
  double algorithmTime = 0.0;

  // Algorithm time approximation
  start = clock();
  FFT(length, fftReals, fftImgs, 1);
  end = clock();
  algorithmTime = (double)(end - start) / CLOCKS_PER_SEC;
  printf("The elapsed time for the FFT is %f seconds\n", algorithmTime);

  start = clock();
  DFT(length, dftReals, dftImgs, 1);
  end = clock();
  algorithmTime = (double)(end - start) / CLOCKS_PER_SEC;
  printf("The elapsed time for the DFT is %f seconds\n", algorithmTime);
  
  int isSimilar = compareFT(length, fftReals, fftImgs, dftReals, dftImgs);
  
  printf("Random testing FFT: is the FFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");
  
}

static void randomTestingFFTI() {

  // limit length to 8 for faster testing
  const unsigned length = (unsigned)pow(2, rand() % 9);

  // allocate arrays for testing
  fft_real reals[length];
  fft_real imgs[length];
  fft_real fftReals[length];
  fft_real fftImgs[length];

  for (unsigned i = 0; i < length; ++i) {
    reals[i] = (float)rand() / RAND_MAX;
    imgs[i] = 0;
    fftReals[i] = reals[i];
    fftImgs[i] = imgs[i];
  }

  DFT(length, fftReals, fftImgs, 1);
  FFT(length, fftReals, fftImgs, -1);
  
  int isSimilar = compareFT(length, reals, imgs, fftReals, fftImgs);
  
  printf("Random testing inverse FFT: is the FFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");
  
}

static void knownTestingFFT() {
  const unsigned length = 4;

  // allocate arrays for testing
  fft_real fftReals[] = {8, 4, 8, 0};
  fft_real fftImgs[] = {0, 0, 0, 0};
  fft_real dftReals[] = {20, 0, 12, 0};
  fft_real dftImgs[] = {0, -4, 0, 4};

  FFT(length, fftReals, fftImgs, 1);
  printf("FFT output:\n");
  for (unsigned i = 0; i < length; ++i) {
    printf("real: %f , img: %f\n", fftReals[i], fftImgs[i]);
  }

  int isSimilar = compareFT(length, fftReals, fftImgs, dftReals, dftImgs);

  printf("Known testing FFT: is the FFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");

}

static void knownTestingFFTI() {
  const unsigned length = 4;

  // allocate arrays for testing
  fft_real dftReals[] = {8, 4, 8, 0};
  fft_real dftImgs[] = {0, 0, 0, 0};
  fft_real fftReals[] = {20, 0, 12, 0};
  fft_real fftImgs[] = {0, -4, 0, 4};

  FFT(length, fftReals, fftImgs, -1);

  int isSimilar = compareFT(length, fftReals, fftImgs, dftReals, dftImgs);

  printf("Known testing inverse FFT: is the FFT working as intended? ");
  if (isSimilar == 0)
    printf("true\n");
  else
    printf("false\n");

}

int main() {

  int seed = time(NULL);
  printf("seed used : %d\n", seed);
  srand(seed);

  knownTestingFFT();
  knownTestingFFTI();
  randomTestingFFT();
  randomTestingFFTI();

  return 0;
}
