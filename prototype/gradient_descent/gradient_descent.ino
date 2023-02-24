extern "C" {
  #include "gradient_descent.h"
}

#include "arduino_serial_port.h"

#define ITMAX 100
#define TOL 2.0e-4

uint32_t degree = 1;
uint32_t n = 1;
gradient_real *x;
gradient_real *y;

void setup() {
  Serial.begin(115200);
}

gradient_real cost(gradient_real p[]) {
  gradient_real sum = 0;

  for (int i = 0; i < n; i++) {
    gradient_real y_predict = 0;

    for (int j = 0; j < degree + 1; j++) {
      y_predict += p[j]*pow(x[i], j);
    }
    
    sum += pow(y_predict - y[i], 2);
  }
  
  return sum;
}

void dcost(gradient_real p[], gradient_real grad[]) {
  for (int j = 0; j < degree + 1; j++) {
    grad[j] = 0;
  }

  for(int i = 0; i < n; i++) {

    gradient_real y_predict = 0;
    for (int j = 0; j < degree + 1; j++) {
      y_predict += p[j]*pow(x[i], j);
    }

    for (int j = 0; j < degree + 1; j++) {
      grad[j] += 2 * (y_predict - y[i]) * pow(x[i], j);
    }
  }
}

void loop() {
  //Read number of points
  readElement(&n, sizeof(n));

  // Allocate x and y data points
  x = (gradient_real*)malloc(n * sizeof(gradient_real));
  y = (gradient_real*)malloc(n * sizeof(gradient_real));

  // Read x & y
  readArray(n, x, sizeof(gradient_real));
  readArray(n, y, sizeof(gradient_real));

  //Read degree
  readElement(&degree, sizeof(degree));

  // Initialize the curve with all coeff at 0
  gradient_real coeff[degree + 1];
  for (int i = 0; i < degree + 1; i++) {
    coeff[i] = 0;
  }
  
  gradient_real minCost = -1;

  minCost = gradient_descent(cost, dcost, coeff, degree + 1, TOL, ITMAX);

  // Output coefficients
  writeFloatArray(degree + 1, coeff);

  // Free memory
  free(x);
  free(y);
}
