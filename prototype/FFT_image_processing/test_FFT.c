#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define fft_real float
// project includes
#include <1chipml.h>
#include "bitmap.h"
#include "serial_port.h"

static void FFTOverSerial(uint32_t length, fft_real* reals, fft_real* imgs, int32_t dir) {
  // Writing
  // write length
  writeElement(&length, sizeof(length));
  // write reals
  writeFloatArray(length, reals);
  // write imaginaries
  writeFloatArray(length, imgs);
  // write direction
  writeElement(&dir, sizeof(dir));

  // Reading
  // read reals
  readFloatArray(length, reals);
  // read imaginaries
  readFloatArray(length, imgs);
}

static inline unsigned int nextPowerOf2(const unsigned int value) {
  unsigned int k = 1;
  while (k < value) {
    k <<= 1;
  }
  return k;
}

static void fftShift(unsigned int height, unsigned int width, fft_real** reals, fft_real** imgs) {
  // shift fft matrix
  // Move horizontally
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

  // Move vertically
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

static int fft2D(unsigned int height, unsigned int width, fft_real** reals, fft_real** imgs, int dir) {
  
  // start with the rows
  for (int i = 0; i < height; i++) {
    int success = FFT(width, reals[i], imgs[i], dir);
    if (success == -1) {
      return success;
    }
  }

  // end with columns
  for (int i = 0; i < width; i++) {
    fft_real nextReals[height];
    fft_real nextImgs[height];
    for (int j = 0; j < height; j++) {
      nextReals[j] = reals[j][i];
      nextImgs[j] = imgs[j][i];
    }

    int success = FFT(height, nextReals, nextImgs, dir);
    if (success == -1) {
      return success;
    }

    for (int j = 0; j < height; j++) {
      reals[j][i] = nextReals[j];
      imgs[j][i] = nextImgs[j];
    }
  }

  return 0;
}

int main() {

  int exitCode = 0;

  exitCode = openSerialPort("/dev/ttyACM0");
  if (exitCode != 0) {
    return exitCode;
  }

  uint32_t size = 5;
  float* array = malloc(size * sizeof(float));

  for(uint32_t i = 0; i < size; ++i) {
    array[i] = i + 1.0 * 0.1;
  }

  writeFloatArray(size, array);

  for(uint32_t i = 0; i < size; ++i) {
    array[i] = 0;
  }

  readFloatArray(size, array);

  printf("size = %d\n", size);
  for(uint32_t i = 0; i < size; ++i) {
    printf("%f\n", array[i]);
  }

  free(array);

  closeSerialPort();

  return 0;
  BITMAPINFOHEADER bitmapInfoHeader;

  // The origin of the image (0,0) is at the bottom left
  unsigned char* originalImageData = readBitmapImage("cameraman400x400.bmp", &bitmapInfoHeader);

  if (originalImageData == NULL) {
    return -1;
  }

  unsigned int kernelHeight = 9;
  unsigned int kernelWidth = 9;

  // pad the image with 0, depending on the kernel size
  // padding occurs on the top and on the right
  const unsigned int pixelByteSize = bitmapInfoHeader.biBitCount >> 3;
  unsigned int height = bitmapInfoHeader.biHeight + kernelHeight - 1;
  unsigned int width = bitmapInfoHeader.biWidth + kernelWidth - 1;


  // Get the next power of 2 for the future FFT
  height = nextPowerOf2(height);
  width = nextPowerOf2(width);

  
  unsigned char* imageData = calloc(height * width * pixelByteSize, sizeof(unsigned char));

  for (unsigned int i = 0; i < bitmapInfoHeader.biHeight; ++i) {
    for (unsigned int j = 0; j < bitmapInfoHeader.biWidth; ++j) {
      for (unsigned int k = 0; k < pixelByteSize; ++k) {
        const unsigned int targetH = i * width * pixelByteSize;
        const unsigned int targetW = j * pixelByteSize;
        const unsigned int targetIdx = targetH + targetW + k;

        const unsigned int sourceH = i * bitmapInfoHeader.biWidth * pixelByteSize;
        const unsigned int sourceW = j * pixelByteSize;
        const unsigned int sourceIdx = sourceH + sourceW + k;
        imageData[targetIdx] = originalImageData[sourceIdx];
      }
    }
  }

  // free the original image data
  free(originalImageData);
  originalImageData = NULL;

  // Convert to Greyscale
  unsigned char* imageDataG = convertRGBtoGrey(imageData, width * height);

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

  // show resulting images
  generateBitmapImageRGB((unsigned char*) imageData, height, width, "bitmapImageRGB.bmp");
  generateBitmapImageGrey((unsigned char*) imageDataG, height, width, "bitmapImageGrey.bmp");

  free(imageData);
  free(imageDataG);
  imageData = NULL;
  imageDataG = NULL;

  // Compute 2D fft
  int success = fft2D(height, width, imageReals, imageImgs, 1);
  //success = fft2D(height, width, imageReals, imageImgs, -1);
  printf("FFT result %d\n", success);


  // convolution matrix generation and padding
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

  // inverse fft shift the kernel
  // here, because the kernel is even, the inverse
  // fft shift is the same as a normal fft shift
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


  return 0;
}
