# Gradient descent anomaly detection

## Presentation

The current prototype offers the necessary logic to perform simple anomaly detection on data points coming from a CSV file.
To do so the arduino is sent the data points and will then compute the polynomial regression coefficients that best fits them.
A python script will use these coefficients to predict the next data point and check if it is close to the next data point from the CSV file.

## Requirements

1. An arduino board that supports serial communications
2. Arduino IDE
3. Python 3

## Quick launch guide

Follow these steps for a quick launch:
1. Open the Arduino project folder [gradient_descent](./gradient_descent) with Arduino IDE.
2. From [gradient_descent.ino](./gradient_descent.ino) Upload the project unto the Arduino board.
3. Open [anomaly_detection.py](anomaly_detection.py). In the main function, change the `serialPortName` to your board serial port.
4. Run the python script
```bash
python anomaly_detection.py
```
5. The prototype should be running.

# Dependencies 
- Numpy
- PySerial
- Pandas
- Matplotlib
