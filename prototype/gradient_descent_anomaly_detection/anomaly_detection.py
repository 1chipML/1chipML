from serial_port import CustomSerial
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import ctypes

def computeRegression(port: CustomSerial, x: "list[float]", y: "list[float]", degree: int) -> "list[float]":
    # Send the number of points
    port.writeElement('<i', len(x))

    # Send the points
    port.writeArray('<f', x)
    port.writeArray('<f', y)

    port.writeElement('<i', degree)

    # Receive the coefficients of the regression from the arduino
    coeff: list[float] = []
    for _ in range(degree + 1):
        readCoeff = port.readElement('<f', ctypes.sizeof(ctypes.c_float))
        coeff.append(readCoeff)

    return coeff

def predict(coeff: "list[float]", x: float) -> float:
    y_predict:float = 0

    for i, val in enumerate(coeff):
        y_predict += val * pow(x, i)

    return y_predict

def detectAnomaly(filename: str, degree: int, treshold: int):
    port: CustomSerial = CustomSerial('/dev/ttyACM0', 115200)

    minPoints: int = 10

    df = pd.read_csv(filename)
    x: list[float] = df["x"].values.tolist()
    y: list[float] = df["y"].values.tolist()

    anomaly_x: list[float] = []
    anomaly_y: list[float] = []

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
    
    port.closeSerial()

if __name__ == "__main__":
    detectAnomaly("data/Linear.csv", 1, 300)
    #detectAnomaly("data/Cubic_20%_outliers.csv", 3, 3000)
