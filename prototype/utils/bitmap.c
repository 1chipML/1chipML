#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct ImageProperties {
  unsigned int height;
  unsigned int width;
  unsigned int paddingSize;
  unsigned int bytesPerPixel;
} ImageProperties;

static void writeBitmapHeaderRGB(const ImageProperties properties,
                                 FILE* imageFile);
static void writeBitmapHeaderGrey(const ImageProperties properties,
                                  FILE* imageFile);
static void writeGenericHeader(const unsigned int numberOfColors,
                               const ImageProperties properties,
                               FILE* imageFile);
static inline unsigned int getPaddingSize(const unsigned int width,
                                          const unsigned int bytesPerPixel);
static void writeImageToFile(const unsigned char* image,
                             const ImageProperties properties, FILE* imageFile);

/**
 * @brief Transforms the incoming RGB channels to Greyscale
 * @param imageData The image data that will be transformed to Greyscale.
 * Expected channel format: B, G, R (default bitmap format)
 * @param numberOfPixels The number of pixels of the incoming RGB image data
 * @return A 8-bit channel containing the Greyscale value. This data must be
 * freed by the user after use to prevent memory leaks
 */
unsigned char* convertRGBtoGrey(unsigned char* imageData,
                                const unsigned int numberOfPixels) {

  unsigned char* greyData =
      malloc(numberOfPixels * GREY_BYTES * sizeof(unsigned char));
  if (greyData == NULL) {
    return NULL;
  }

  // Luminosity method
  const double wBlue = 0.114;
  const double wGreen = 0.587;
  const double wRed = 0.299;

  for (int i = 0; i < numberOfPixels; ++i) {
    double y = imageData[i * RGB_BYTES] * wBlue +
               imageData[i * RGB_BYTES + 1] * wGreen +
               imageData[i * RGB_BYTES + 2] * wRed;
    greyData[i] = (unsigned char)y;
  }

  return greyData;
}

/**
 * @brief Reads a bitmap file
 * @param filename The bitmap filename with the extension
 * @param bitmapInfoHeader The struct that will hold the BITMAPINFOHEADER
 * of the opened bitmap file
 * @return The entire image data, in bytes. This data must be freed by the
 * user after use to prevent memory leaks
 */
unsigned char* readBitmapImage(char* filename,
                               BitmapInfoHeader* bitmapInfoHeader) {

  FILE* imageFile;
  BitmapFileHeader bitmapFileHeader;

  // open file in read binary mode
  imageFile = fopen(filename, "rb");
  if (imageFile == NULL) {
    return NULL;
  }

  // Read individual bitmap file header properties
  // Must be read individually to prevent packing issues
  fread(&bitmapFileHeader.bfType, sizeof(bitmapFileHeader.bfType), 1,
        imageFile);
  fread(&bitmapFileHeader.bfSize, sizeof(bitmapFileHeader.bfSize), 1,
        imageFile);
  fread(&bitmapFileHeader.bfReserved1, sizeof(bitmapFileHeader.bfReserved1), 1,
        imageFile);
  fread(&bitmapFileHeader.bfReserved2, sizeof(bitmapFileHeader.bfReserved2), 1,
        imageFile);
  fread(&bitmapFileHeader.bfOffBits, sizeof(bitmapFileHeader.bfOffBits), 1,
        imageFile);

  // Verify for the bm file type
  if (bitmapFileHeader.bfType != 0x4D42) {
    fclose(imageFile);
    return NULL;
  }

  // Read individual bitmap file header properties
  // Must be read individually to prevent packing issues
  fread(&bitmapInfoHeader->biSize, sizeof(bitmapInfoHeader->biSize), 1,
        imageFile);
  fread(&bitmapInfoHeader->biWidth, sizeof(bitmapInfoHeader->biWidth), 1,
        imageFile);
  fread(&bitmapInfoHeader->biHeight, sizeof(bitmapInfoHeader->biHeight), 1,
        imageFile);
  fread(&bitmapInfoHeader->biPlanes, sizeof(bitmapInfoHeader->biPlanes), 1,
        imageFile);
  fread(&bitmapInfoHeader->biBitCount, sizeof(bitmapInfoHeader->biBitCount), 1,
        imageFile);
  fread(&bitmapInfoHeader->biCompression,
        sizeof(bitmapInfoHeader->biCompression), 1, imageFile);
  fread(&bitmapInfoHeader->biSizeImage, sizeof(bitmapInfoHeader->biSizeImage),
        1, imageFile);
  fread(&bitmapInfoHeader->biXPelsPerMeter,
        sizeof(bitmapInfoHeader->biXPelsPerMeter), 1, imageFile);
  fread(&bitmapInfoHeader->biYPelsPerMeter,
        sizeof(bitmapInfoHeader->biYPelsPerMeter), 1, imageFile);
  fread(&bitmapInfoHeader->biClrUsed, sizeof(bitmapInfoHeader->biClrUsed), 1,
        imageFile);
  fread(&bitmapInfoHeader->biClrImportant,
        sizeof(bitmapInfoHeader->biClrImportant), 1, imageFile);

  // Go where the data is
  fseek(imageFile, bitmapFileHeader.bfOffBits, SEEK_SET);

  // handle cases where the size is 0
  if (bitmapInfoHeader->biSizeImage == 0) {
    bitmapInfoHeader->biSizeImage =
        bitmapFileHeader.bfSize - bitmapFileHeader.bfOffBits;
  }

  // Allocate memory for the bitmap data
  unsigned char* imageData =
      (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);
  if (imageData == NULL) {
    fclose(imageFile);
    return NULL;
  }

  // read in the bitmap image data
  fread(imageData, bitmapInfoHeader->biSizeImage, 1, imageFile);

  // close file and return bitmap image data in the BGR format
  fclose(imageFile);
  return imageData;
}

/**
 * @brief Creates a RGB bitmap image from data
 * @param image The image data. One byte per channel in the order: B, G, R is
 * expected
 * @param height The image height, in pixels
 * @param width The image width, in pixels
 * @param imageFileName The image file name, with the bitmap extension
 */
void generateBitmapImageRGB(unsigned char* image, const unsigned int height,
                            const unsigned int width, char* imageFileName) {

  const unsigned int paddingSize = getPaddingSize(width, RGB_BYTES);
  ImageProperties imageProperties = {height, width, paddingSize, RGB_BYTES};

  FILE* imageFile = fopen(imageFileName, "wb");
  writeBitmapHeaderRGB(imageProperties, imageFile);
  writeImageToFile(image, imageProperties, imageFile);
  fclose(imageFile);
}

/**
 * @brief Creates a Grayscale bitmap image from data, with a single 8-bit
 * channel
 * @param image The image data. One byte per pixel is expected
 * @param height The image height, in pixels
 * @param width The image width, in pixels
 * @param imageFileName The image file name, with the bitmap extension
 */
void generateBitmapImageGrey(unsigned char* image, const unsigned int height,
                             const unsigned int width, char* imageFileName) {
  const unsigned int paddingSize = getPaddingSize(width, GREY_BYTES);
  ImageProperties imageProperties = {height, width, paddingSize, GREY_BYTES};

  FILE* imageFile = fopen(imageFileName, "wb");
  writeBitmapHeaderGrey(imageProperties, imageFile);
  writeImageToFile(image, imageProperties, imageFile);
  fclose(imageFile);
}

static void writeBitmapHeaderRGB(const ImageProperties properties,
                                 FILE* imageFile) {

  const unsigned int numberOfColors = 0; // default
  writeGenericHeader(numberOfColors, properties, imageFile);
}

static void writeBitmapHeaderGrey(const ImageProperties properties,
                                  FILE* imageFile) {

  const unsigned int numberOfColors = 256;
  writeGenericHeader(numberOfColors, properties, imageFile);

  // write the color palette for Greyscale
  for (unsigned int i = 0; i < numberOfColors; ++i) {
    fprintf(imageFile, "%c", (unsigned char)i);
    fprintf(imageFile, "%c", (unsigned char)i);
    fprintf(imageFile, "%c", (unsigned char)i);
    fprintf(imageFile, "%c", (unsigned char)0);
  }
}

static void writeGenericHeader(const unsigned int numberOfColors,
                               const ImageProperties properties,
                               FILE* imageFile) {

  const unsigned int paddedWidthInBytes =
      properties.width * properties.bytesPerPixel + properties.paddingSize;

  const unsigned int imageSize = paddedWidthInBytes * properties.height;
  const unsigned int totalHeaderSize =
      FILE_HEADER_SIZE + INFO_HEADER_SIZE + numberOfColors * 4;
  const unsigned int fileSize = totalHeaderSize + imageSize;

  uint32_t largeHeaders[12] = {
      fileSize,          // the size in bytes of the bitmap file (bfSize)
      0,                 // reserved (bfReserved1 and bfReserved2)
      totalHeaderSize,   // start pixel of the array (bfOffBits)
      INFO_HEADER_SIZE,  // bitmap info header size in bytes (biSize)
      properties.width,  // image width in pixels (biWidth)
      properties.height, // image height in pixels (biHeight)
                         // Here would go the number of color planes (biPlanes)
                         // Here would go the bits per pixel (biBitCount)
      0,                 // compression (biCompression)
      imageSize,         // image size in bytes (biSizeImage)
      0,                 // horizontal resolution (biXPelsPerMeter)
      0,                 // vertical resolution (biYPelsPerMeter)
      numberOfColors,    // colors in color table (biClrUsed)
      0                  // important color count (biClrImportant)
  };

  uint16_t biPlanes = 1; // number of color planes (biPlanes)
  uint16_t biBitCount =
      properties.bytesPerPixel * 8; // bits per pixel (biBitCount)

  // Write one byte at a time to take into account big-endian and little-endian
  fprintf(imageFile, "BM");

  // write bfSize, reserved, bfOffBits, biSize, biWidth and biHeight
  for (int i = 0; i < 6; ++i) {
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i]));
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i] >> 8));
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i] >> 16));
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i] >> 24));
  }

  // write biPlanes
  fprintf(imageFile, "%c", (unsigned char)(biPlanes));
  fprintf(imageFile, "%c", (unsigned char)(biPlanes >> 8));

  // write biBitCount
  fprintf(imageFile, "%c", (unsigned char)(biBitCount));
  fprintf(imageFile, "%c", (unsigned char)(biBitCount >> 8));

  // write biCompression, biSizeImage, biXPelsPerMeter, biYPelsPerMeter,
  // biClrUsed, biClrImportant
  for (int i = 6; i < 12; ++i) {
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i]));
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i] >> 8));
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i] >> 16));
    fprintf(imageFile, "%c", (unsigned char)(largeHeaders[i] >> 24));
  }
}

static void writeImageToFile(const unsigned char* image,
                             const ImageProperties properties,
                             FILE* imageFile) {

  const unsigned int widthInBytes = properties.width * properties.bytesPerPixel;
  const unsigned char padding[3] = {0, 0, 0};

  for (unsigned int i = 0; i < properties.height; ++i) {
    fwrite(image + (i * widthInBytes), properties.bytesPerPixel,
           properties.width, imageFile);
    fwrite(padding, 1, properties.paddingSize, imageFile);
  }
}

static inline unsigned int getPaddingSize(const unsigned int width,
                                          const unsigned int bytesPerPixel) {

  const unsigned int widthInBytes = width * bytesPerPixel;
  return (4U - widthInBytes % 4U) % 4U;
}
