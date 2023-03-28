#include <stdint.h>

#ifndef BITMAP_H
#define BITMAP_H

#define RGB_BYTES 3 // rgb
#define GREY_BYTES 1
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BitmapFileHeader {
  uint16_t bfType;      // file type
  uint32_t bfSize;      // the size in bytes of the bitmap file
  uint16_t bfReserved1; // reserved; must be 0
  uint16_t bfReserved2; // reserved; must be 0
  uint32_t bfOffBits;   // start pixel of the array
} BitmapFileHeader;

typedef struct BitmapInfoHeader {
  uint32_t biSize;          // bitmap info header size in bytes
  uint32_t biWidth;         // image width in pixels
  uint32_t biHeight;        // image height in pixels
  uint16_t biPlanes;        // the number of color planes
  uint16_t biBitCount;      // the bits per pixel
  uint32_t biCompression;   // compression
  uint32_t biSizeImage;     // image size in bytes
  uint32_t biXPelsPerMeter; // horizontal resolution
  uint32_t biYPelsPerMeter; // vertical resolution
  uint32_t biClrUsed;       // number of colors in the color table
  uint32_t biClrImportant;  // number of colors that are important
} BitmapInfoHeader;

unsigned char* convertRGBtoGrey(unsigned char* imageData,
                                const unsigned int numberOfPixels);
unsigned char* readBitmapImage(char* filename,
                               BitmapInfoHeader* bitmapInfoHeader);

void generateBitmapImageRGB(unsigned char* image, const unsigned int height,
                            const unsigned int width, char* imageFileName);

void generateBitmapImageGrey(unsigned char* image, const unsigned int height,
                             const unsigned int width, char* imageFileName);

#ifdef __cplusplus
}
#endif

#endif // BITMAP_H
