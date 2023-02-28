#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define fft_real float

// project includes
#include "bitmap.h"
#include "serial_port.h"

/**
 * @brief Sends an FFT over serial communication. 
 * The serial communication must be opened beforehand for the data to be sent.
 * @param length The length of the FFT array. Must be a power of 2.
 * @param reals The real part of the FFT array.
 * This array will contain the end result of the real part of the FFT
 * @param reals The imaginary part of the FFT array.
 * This array will contain the end result of the imaginary part of the FFT
 * @param dir The direction of the FFT. 1 for the FFT, -1 for the inverse FFT.
 * @return 0 if everything went correctly, 1 otherwise
*/
static int FFTOverSerial(uint16_t length, fft_real* reals, fft_real* imgs, int16_t dir) {
  // Writing
  // write length
  int returncode = writeElement(&length, sizeof(length));
  // write reals
  if (returncode >= 0) {
    int writeCode = writeArray(length, reals, sizeof(fft_real));
    returncode = writeCode < 0 ? writeCode : returncode + writeCode;
  }
  // write imaginaries
  if (returncode >= 0) {
    int writeCode = writeArray(length, imgs, sizeof(fft_real));
    returncode = writeCode < 0 ? writeCode : returncode + writeCode;
  }
  // write direction
  if (returncode >= 0) {
    int writeCode = writeElement(&dir, sizeof(dir));
    returncode = writeCode < 0 ? writeCode : returncode + writeCode;
  }

  // Reading
  // read reals
  if (returncode >= 0) {
    int writeCode = readArray(length, reals, sizeof(fft_real));
    returncode = writeCode < 0 ? writeCode : returncode + writeCode;
  }
  // read imaginaries
  if (returncode >= 0) {
    int writeCode = readArray(length, imgs, sizeof(fft_real));
    returncode = writeCode < 0 ? writeCode : returncode + writeCode;
  }

  return returncode >= 0 ? 0 : 1;
}

/**
 * @brief Finds the next power of 2 of the incoming value.
 * @param value The value to find the next power of 2.
 * @return The next power of 2 of the incoming value
*/
static inline unsigned int nextPowerOf2(const unsigned value) {
  unsigned k = 1;
  while (k < value) {
    k <<= 1;
  }
  return k;
}

/**
 * @brief Shift FFT matrix. Only works for even sized matrix.
 * @param height The height of the matrix.
 * @param width The width of the matrix.
 * @param reals The real part of the FFT matrix.
 * @param imgs The imaginary part of the FFT matrix.
*/
static void fftShift(unsigned int height, unsigned int width, fft_real** reals, fft_real** imgs) {
  // Shift the matrix horizontally first
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width / 2; ++j) {
      fft_real tmpReal = reals[i][j];
      fft_real tmpImg = imgs[i][j];
      reals[i][j] = reals[i][j + width / 2];
      imgs[i][j] = imgs[i][j + width / 2];
      reals[i][j + width / 2] = tmpReal;
      imgs[i][j + width / 2] = tmpImg;
    }
  }

  // Shift the matrix vertically
  for (int i = 0; i < height / 2; ++i) {
    for (int j = 0; j < width; ++j) {
      fft_real tmpReal = reals[i][j];
      fft_real tmpImg = imgs[i][j];
      reals[i][j] = reals[i + height / 2][j];
      imgs[i][j] = imgs[i + height / 2][j];
      reals[i + height / 2][j] = tmpReal;
      imgs[i + height / 2][j] = tmpImg;
    }
  }
}

/**
 * @brief Computes the 2D FFT on a matrix of reals and imaginary numbers.
 * @param height The height of the matrix. Must be a power of 2.
 * @param width The width of the matrix. Must be a power of 2.
 * @param reals The real part of the FFT matrix.
 * This matrix will contain the end result of the real part of the FFT.
 * @param imgs The imaginary part of the FFT matrix.
 * This matrix will contain the end result of the imaginary part of the FFT.
 * @param dir The direction of the FFT. 1 for the FFT, -1 for the inverse FFT.
 * @return 0 if everything went correctly
*/
static int fft2D(unsigned height, unsigned int width, fft_real** reals, fft_real** imgs, int dir) {
  
  // Start with the rows
  for (unsigned i = 0; i < height; i++) {
    int success = FFTOverSerial(width, reals[i], imgs[i], dir);
    if (success != 0) {
      return success;
    }
  }

  // End with columns
  for (unsigned i = 0; i < width; i++) {
    fft_real nextReals[height];
    fft_real nextImgs[height];

    // Prepare the FFT array
    for (unsigned j = 0; j < height; j++) {
      nextReals[j] = reals[j][i];
      nextImgs[j] = imgs[j][i];
    }

    // Compute the FFT
    int success = FFTOverSerial(height, nextReals, nextImgs, dir);
    if (success != 0) {
      return success;
    }

    // Copy the result back
    for (unsigned j = 0; j < height; j++) {
      reals[j][i] = nextReals[j];
      imgs[j][i] = nextImgs[j];
    }
  }

  return 0;
}

int main() {

  int exitCode = 0;
  char* serialPortName = "/dev/ttyACM0";
  char* imageName = "1chipML_color.bmp";

  // Open the serial port
  exitCode = openSerialPort(serialPortName);
  if (exitCode != 0) {
    return exitCode;
  }

  BITMAPINFOHEADER bitmapInfoHeader;

  // Open the image
  // The origin of the image (0,0) is at the bottom left
  unsigned char* originalImageData = readBitmapImage(imageName, &bitmapInfoHeader);
  if (originalImageData == NULL) {
    printf("Could not load the image\n");
    return 1;
  }

  // Kernel matrix definition and initialization
  unsigned kernelHeight = 9;
  unsigned kernelWidth = 9;
  fft_real kernelMatrix[kernelHeight][kernelWidth];
  for (unsigned i = 0; i < kernelHeight; ++i) {
    for (unsigned j = 0; j < kernelWidth; ++j) {
      kernelMatrix[i][j] = 1.0 / 9.0;
    }
  }


  // Prepare padding, depending on the kernel size
  // padding occurs on the top and on the right
  const unsigned pixelByteSize = bitmapInfoHeader.biBitCount >> 3;
  unsigned height = bitmapInfoHeader.biHeight + kernelHeight - 1;
  unsigned width = bitmapInfoHeader.biWidth + kernelWidth - 1;

  // Get the next power of 2 for the future FFT
  height = nextPowerOf2(height);
  width = nextPowerOf2(width);

  // Pad the image with 0 to the calculated width and height
  unsigned char* imageData = calloc(height * width * pixelByteSize, sizeof(unsigned char));

  // Copy the image data to the padded image
  for (unsigned i = 0; i < bitmapInfoHeader.biHeight; ++i) {
    for (unsigned j = 0; j < bitmapInfoHeader.biWidth; ++j) {
      for (unsigned k = 0; k < pixelByteSize; ++k) {
        const unsigned targetH = i * width * pixelByteSize;
        const unsigned targetW = j * pixelByteSize;
        const unsigned targetIdx = targetH + targetW + k;

        const unsigned sourceH = i * bitmapInfoHeader.biWidth * pixelByteSize;
        const unsigned sourceW = j * pixelByteSize;
        const unsigned sourceIdx = sourceH + sourceW + k;
        imageData[targetIdx] = originalImageData[sourceIdx];
      }
    }
  }

  // free the original image data
  free(originalImageData);
  originalImageData = NULL;

  // Convert the image data to Greyscale
  unsigned char* imageDataG = convertRGBtoGrey(imageData, width * height);

  // Matrix initialization
  fft_real** imageReals = malloc(height * sizeof(fft_real*));
  fft_real** imageImgs = malloc(height * sizeof(fft_real*));
  fft_real** kernelReals = malloc(height * sizeof(fft_real*));
  fft_real** kernelImgs = malloc(height * sizeof(fft_real*));
  for (int i = 0; i < height; ++i) {
    imageReals[i] = malloc(width * sizeof(fft_real));
    imageImgs[i] = malloc(width * sizeof(fft_real));
    kernelReals[i] = malloc(width * sizeof(fft_real));
    kernelImgs[i] = malloc(width * sizeof(fft_real));
  }

  // copy to array for easier access
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      imageReals[i][j] = imageDataG[i * width + j]; 
      imageImgs[i][j] = 0; 
    }
  }

  // show resulting image
  generateBitmapImageGrey((unsigned char*) imageDataG, height, width, "bitmapImageGrey.bmp");

  // Free the initial image data
  free(imageData);
  free(imageDataG);
  imageData = NULL;
  imageDataG = NULL;

  // Compute the first 2D fft
  exitCode = fft2D(height, width, imageReals, imageImgs, 1);
  printf("Image FFT result %d\n", exitCode);
  if (exitCode != 0 ) {
    closeSerialPort();
    return exitCode;
  }

  // convolution matrix padding
  unsigned int totalPaddingVertical = height - kernelHeight;
  unsigned int totalPaddingHorizontal = width - kernelWidth;
  unsigned int startVertical = (totalPaddingVertical + 1) / 2;
  unsigned int startHorizontal = (totalPaddingHorizontal + 1) / 2;

  for(int i = 0; i < height; ++i) {
    for(int j = 0; j < width; ++j) {
      kernelImgs[i][j] = 0;
      if (i >= startVertical && i < startVertical + kernelHeight 
      && j >= startHorizontal && j < startHorizontal + kernelWidth) {
        kernelReals[i][j] = 1.0 / (kernelHeight * kernelWidth);
      }
      else {
        kernelReals[i][j] = 0;
      }
    }
  }

  // Compute the inverse FFT shift the padded kernel.
  // here, because the padded kernel is even, the inverse
  // FFT shift is the same as a normal fft shift
  fftShift(height, width, kernelReals, kernelImgs);

  // display the kernel
  unsigned char* kernelRealsImage = malloc(height * width);
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      kernelRealsImage[i * width + j] = (unsigned char) (kernelReals[i][j] > 0 ? 255 : 0);
    }
  }
  generateBitmapImageGrey((unsigned char*) kernelRealsImage, height, width, "kernel.bmp");
  free(kernelRealsImage);

  /*****Start of magnitude*****/
  // Shift 2D fft center for magnitude
  fftShift(height, width, imageReals, imageImgs);

  // get magnitude
  fft_real* imageAbs = malloc(height * width *sizeof(fft_real));
  for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
          fft_real abs = sqrt(pow(imageReals[i][j], 2) + pow(imageImgs[i][j], 2));
          imageAbs[i * width + j] = abs;
      }
  }

  // get maximum magnitude
  fft_real max = imageAbs[0];
  for (int i = 1; i < height * width; ++i) {
    if (imageAbs[i] > max) {
      max = imageAbs[i];
    }
  }

  // log scalling magnitude and color coding
  fft_real greyColor = 255 / log10(1 + max);
  for (int i = 1; i < height * width; ++i) {
    imageAbs[i] = greyColor * log10(1 + imageAbs[i]);
  }

  // Shift 2D fft to get original placement
  fftShift(height, width, imageReals, imageImgs);
  /*****End of magnitude*****/

  // 2d fft of the kernel
  fft2D(height, width, kernelReals, kernelImgs, 1);

  // element-wise multiplication between the kernel and the image
  for(int i = 0; i < height; ++i) {
    for(int j = 0; j < width; ++j) {
      fft_real tmpImageReal = imageReals[i][j];
      imageReals[i][j] = tmpImageReal * kernelReals[i][j] - imageImgs[i][j] * kernelImgs[i][j];
      imageImgs[i][j] = tmpImageReal * kernelImgs[i][j] + imageImgs[i][j] * kernelReals[i][j];
    }
  }

  // inverse fft on element-wise multiplication
  fft2D(height, width, imageReals, imageImgs, -1);
  // Then the result is obtained!


  unsigned char imageGreyScaleR[height][width][GREY_BYTES];
  unsigned char imageGreyScaleI[height][width][GREY_BYTES];
  unsigned char imageGreyScaleMag[height][width][GREY_BYTES];
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      imageGreyScaleR[i][j][0] = (unsigned char) abs(imageReals[i][j]);
      imageGreyScaleI[i][j][0] = (unsigned char) abs(imageImgs[i][j]);
      imageGreyScaleMag[i][j][0] = (unsigned char) (imageAbs[i * width + j]);
    }
  }

  generateBitmapImageGrey((unsigned char*) imageGreyScaleR, height, width, "real.bmp");
  generateBitmapImageGrey((unsigned char*) imageGreyScaleI, height, width, "imaginary.bmp");
  generateBitmapImageGrey((unsigned char*) imageGreyScaleMag, height, width, "magnitudelog.bmp");

  // trim the image of its padding for the final result
  unsigned char imageResult[bitmapInfoHeader.biHeight][bitmapInfoHeader.biWidth];
  for (unsigned int i = 0; i < bitmapInfoHeader.biHeight; ++i) {
    for (unsigned int j = 0; j < bitmapInfoHeader.biWidth; ++j) {
      imageResult[i][j] = imageReals[i][j];
    }
  }
  generateBitmapImageGrey((unsigned char*) imageResult, bitmapInfoHeader.biHeight, bitmapInfoHeader.biWidth, "result.bmp");
  printf("Image generated!!\n");

  // free all
  free(imageAbs);

  for (int i = 0; i < height; ++i) {
    free(imageReals[i]);
    free(imageImgs[i]);
    free(kernelReals[i]);
    free(kernelImgs[i]);
  }
  free(imageReals);
  free(imageImgs);
  free(kernelReals);
  free(kernelImgs);

  closeSerialPort();

  return 0;
}
