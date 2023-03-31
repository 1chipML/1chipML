import struct
import serial
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import time
import ctypes

def computeRegression(port: serial.Serial, x: "list[float]", y: "list[float]", degree: int) -> "list[float]":
    # Send the number of points
    port.write(struct.pack('<i', len(x)))

    # Send the points
    for val in x:
        port.write(struct.pack('<f', val))

    for val in y:
        port.write(struct.pack('<f', val))

    port.write(struct.pack('<i', degree))

    # Receive the coefficients of the regression from the arduino
    coeff: list[float] = []
    for _ in range(degree + 1):
        coeff.append(struct.unpack('<f', port.read(ctypes.sizeof(ctypes.c_float)))[0])

    return coeff

def predict(coeff: "list[float]", x: float) -> float:
    y_predict:float = 0

    for i, val in enumerate(coeff):
        y_predict += val * pow(x, i)

    return y_predict

def detectAnomaly(filename: str, degree: int, treshold: int):
    port = serial.Serial('/dev/ttyACM0', 115200)

    minPoints: int = 10

    df = pd.read_csv(filename)
    x: list[float] = df["x"].values.tolist()
    y: list[float] = df["y"].values.tolist()

    anomaly_x: list[float] = []
    anomaly_y: list[float] = []

    # Let arduino have enough time to initialise serial connection
    time.sleep(2)

    n = minPoints + 1
    while (n < len(x)):
        coeff: list(float) = computeRegression(port, x[:n], y[:n], degree)

        y_predict: float = predict(coeff, x[n])

        # If the value is too far off the prediction. It is probably an anomaly
        if abs(y[n] - y_predict) > treshold:
            anomaly_x.append(x[n])
            anomaly_y.append(y[n])
            del x[n]
            del y[n]
        else:
            n += 1
    
    coeff: list[float] = computeRegression(port, x, y, degree)

    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    ax1.scatter(x, y, c='b')
    ax1.scatter(anomaly_x, anomaly_y, c='r')

    line_x = np.linspace(x[0], x[-1], 100)
    plt.plot(line_x, predict(coeff, line_x))
    plt.show()
    
    port.close()

if __name__ == "__main__":
    detectAnomaly("data/Linear.csv", 1, 300)
    #detectAnomaly("data/Cubic_20%_outliers.csv", 3, 3000)
