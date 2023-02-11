#include "DFT.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
 * @param length The length of the input vectors.
 * @param realArray 1D array containing the real part of the incoming vector.
 * This array will contain the end result of the real part of the DFT
 * @param imaginaryArray 1D array containing the imaginary part of the incoming
 * vector. This array will contain the end result of the imaginary part of the
 * DFT
 * @param dir Direction of the DFT. 1 for the DFT, -1 for the inverse DFT
 */
void DFT(const unsigned length, dft_real* realArray, dft_real* imaginaryArray, const int dir) {

  dft_real* outputReals = calloc(length, sizeof(dft_real));
  dft_real* outputImaginaries = calloc(length, sizeof(dft_real));

  int thetaFactor = -1;
  if (dir < 0) {
    thetaFactor = 1;
  }

  for (unsigned i = 0; i < length; ++i) {
    dft_real exponentBase = thetaFactor * 2.0 * M_PI * (dft_real)i / (dft_real)length;
    for (unsigned k = 0; k < length; ++k) {
      dft_real wCos = cos(k * exponentBase); // real part
      dft_real wSin = sin(k * exponentBase); // imaginary part
      outputReals[i] += realArray[k] * wCos - imaginaryArray[k] * wSin; // real
      outputImaginaries[i] += realArray[k] * wSin + imaginaryArray[k] * wCos; // imaginary
    }
  }

  // inverse DFT
  if (dir < 0) {
    const dft_real inverseLength = 1.0 / length;
    for (unsigned i = 0; i < length; ++i) {
      outputReals[i] *= inverseLength;
      outputImaginaries[i] *= inverseLength;
    }
  }

  // place result in arrays
  memcpy(realArray, outputReals, length * sizeof(dft_real));
  memcpy(imaginaryArray, outputImaginaries, length * sizeof(dft_real));

  free(outputReals);
  free(outputImaginaries);
}
