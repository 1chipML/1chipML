#include <1chipml.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int compareFT(const unsigned length, fft_real* incomingReals,
                     fft_real* incomingImgs, fft_real* expectedReals,
                     fft_real* expectedImgs) {

  const fft_real epsilon = 1e-10;

  for (unsigned i = 0; i < length; ++i) {
    if (fabs(incomingReals[i] - expectedReals[i]) > epsilon ||
        fabs(incomingImgs[i] - expectedImgs[i]) > epsilon) {
      return 1;
    }
  }

  return 0;
}

static int randomTestingFFT(const unsigned maxLengthPower) {

  const unsigned length = (unsigned)pow(
      2, (unsigned)(linear_congruential_random_generator() * maxLengthPower));

  // allocate arrays for testing
  fft_real fftReals[length];
  fft_real fftImgs[length];
  fft_real dftReals[length];
  fft_real dftImgs[length];

  for (unsigned i = 0; i < length; ++i) {
    fftReals[i] = linear_congruential_random_generator();
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
  int returnCode = 0;
  if (isSimilar == 0) {
    printf("true\n");
    returnCode = 0;
  } else {
    printf("false\n");
    returnCode = 1;
  }
  return returnCode;
}

static int randomTestingFFTI(const unsigned maxLengthPower) {

  const unsigned length = (unsigned)pow(
      2, (unsigned)(linear_congruential_random_generator() * maxLengthPower));

  // allocate arrays for testing
  fft_real reals[length];
  fft_real imgs[length];
  fft_real fftReals[length];
  fft_real fftImgs[length];

  for (unsigned i = 0; i < length; ++i) {
    reals[i] = linear_congruential_random_generator();
    imgs[i] = 0;
    fftReals[i] = reals[i];
    fftImgs[i] = imgs[i];
  }

  DFT(length, fftReals, fftImgs, 1);
  FFT(length, fftReals, fftImgs, -1);

  int isSimilar = compareFT(length, reals, imgs, fftReals, fftImgs);

  printf("Random testing inverse FFT: is the FFT working as intended? ");
  int returnCode = 0;
  if (isSimilar == 0) {
    printf("true\n");
    returnCode = 0;
  } else {
    printf("false\n");
    returnCode = 1;
  }
  return returnCode;
}

static int knownTestingFFT(const unsigned length, fft_real* inputReals,
                           fft_real* inputImaginaries, fft_real* expectedReals,
                           fft_real* expectedImaginaries) {

  FFT(length, inputReals, inputImaginaries, 1);

  int isSimilar = compareFT(length, inputReals, inputImaginaries, expectedReals,
                            expectedImaginaries);

  printf("Known testing FFT: is the FFT working as intended? ");
  int returnCode = 0;
  if (isSimilar == 0) {
    printf("true\n");
    returnCode = 0;
  } else {
    printf("false\n");
    returnCode = 1;
  }
  return returnCode;
}

static int knownTestingFFTI(const unsigned length, fft_real* inputReals,
                            fft_real* inputImaginaries, fft_real* expectedReals,
                            fft_real* expectedImaginaries) {

  FFT(length, inputReals, inputImaginaries, -1);

  int isSimilar = compareFT(length, inputReals, inputImaginaries, expectedReals,
                            expectedImaginaries);

  printf("Known testing inverse FFT: is the FFT working as intended? ");
  int returnCode = 0;
  if (isSimilar == 0) {
    printf("true\n");
    returnCode = 0;
  } else {
    printf("false\n");
    returnCode = 1;
  }
  return returnCode;
}

int main() {

  int seed = time(NULL);
  printf("seed used : %d\n", seed);
  set_linear_congruential_generator_seed(seed);

  int returnCode = 0;

  const unsigned randomLengthPower = 8;
  returnCode |= randomTestingFFT(randomLengthPower);
  returnCode |= randomTestingFFTI(randomLengthPower);

  const unsigned length = 4;
  fft_real FFTinputReals[] = {8, 4, 8, 0};
  fft_real FFTinputImgs[] = {0, 0, 0, 0};
  fft_real FFTexpectedReals[] = {20, 0, 12, 0};
  fft_real FFTexpectedImgs[] = {0, -4, 0, 4};
  returnCode |= knownTestingFFT(length, FFTinputReals, FFTinputImgs,
                                FFTexpectedReals, FFTexpectedImgs);

  fft_real FFTIinputReals[] = {20, 0, 12, 0};
  fft_real FFTIinputImgs[] = {0, -4, 0, 4};
  fft_real FFTIexpectedReals[] = {8, 4, 8, 0};
  fft_real FFTIexpectedImgs[] = {0, 0, 0, 0};
  returnCode |= knownTestingFFTI(length, FFTIinputReals, FFTIinputImgs,
                                 FFTIexpectedReals, FFTIexpectedImgs);

  return returnCode;
}
