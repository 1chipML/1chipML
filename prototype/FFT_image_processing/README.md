# FFT image processing

## Presentation

The current prototype offers the logic necessary to compute an image convolution with the help of an Arduino board.
The convolution is computed using Fast Fourier transforms (FFTs). 
The FFT uploaded on the Arduino board is the one from the source folder.
The image used for the prototype follows the bitmap (.bmp) format.

## Requirements

1. An arduino board that supports serial communications
2. Arduino IDE
3. GCC

## Quick launch guide

Follow these steps for a quick launch:
1. Open the Arduino project folder [arduino_image_processing](./arduino_image_processing) with Arduino IDE.
2. From [arduino_image_processing.ino](./arduino_image_processing/arduino_image_processing.ino) Upload the project unto the Arduino board.
3. Open [image_processing.c](image_processing.c). In the main function, change the `serialPortName` to your board serial port. By default, it is set to `/dev/ttyACM0`.
4. (Optional) Open [image_processing.c](image_processing.c). In the main function, change the `openedImageName` to the image you wish to load. By default, it is set to `1chipML_color32.bmp`.
5. (Optional) Open [image_processing.c](image_processing.c). In the main function, change the `kernelMatrix` to the desired Kernel. By default, a smoothing kernel is used.
6. In the current folder [FFT_image_processing](.), make the prototype with the following command:
```bash
make image_processing
```
7. Run the result from the previous command
```bash
./image_processing.out
```
8. The prototype should be running.

## How it works

The image convolution using FFTs follows these steps in the main function of the [image_processing.c](image_processing.c) file:
1. The prototype paramaters as well as the convolution matrix are defined
2. A serial connection with the Arduino board is initiated
3. The bitmap image on which to apply the convolution is loaded
4. The bitmap image is expanded with  0s to fit the convolution matrix and to be a power of 2.
5. The loaded image is converted to grayscale
6. A 2D FFT over serial is applied on the image. In other words, the data for wich to calculate the FFT is sent to the Arduino board and then, the result is received.
7. The magnitude of the image is computed
8. The convolution matrix is expanded to match the current image size
9. The convolution matrix is shifted to match the origin of the current image
10. A 2D FFT over serial is applied on the convolution matrix.
11. Element-wise mutliplication is done between the convolution matrix and the image.
12. A 2D FFT over serial is applied on the result of the element-wise mutliplication.
13. The resulting images are generated.
14. The serial connection is closed.

