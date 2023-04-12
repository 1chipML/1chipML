
#define fft_real float

#include "FFT.h"
#include "arduino_serial_port.hpp"

void setup() {
  // initialize serial:
  setupSerial(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

unsigned FFTLength = 0;
int dir = 0;
int fftCode = 0;

void loop() {
  if (Serial.available() && fftCode == 0) {

    digitalWrite(LED_BUILTIN, HIGH);

    // FFT over serial

    // Reading
    // read length
    readElement(&FFTLength, sizeof(FFTLength));

    // init arrays
    fft_real FFTreals[FFTLength];
    fft_real FFTimgs[FFTLength];

    // read reals
    readArray(FFTLength, FFTreals, sizeof(fft_real));
    // read imaginaries
    readArray(FFTLength, FFTimgs, sizeof(fft_real));
    // read direction
    readElement(&dir, sizeof(dir));

    // Execute FFT
    fftCode = FFT(FFTLength, FFTreals, FFTimgs, dir);
    if (fftCode != 0) {
      digitalWrite(LED_BUILTIN, LOW);
      return;
    }

    // Writing
    // write reals
    writeArray(FFTLength, FFTreals, sizeof(float));
    // write imaginaries
    writeArray(FFTLength, FFTimgs, sizeof(float));

  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
