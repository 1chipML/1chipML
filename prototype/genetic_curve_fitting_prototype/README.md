# Anomaly Detection (Genetic Algorithm)

## Presentation

The current prototype uses a genetic algorithm to estimate the polynomial equation representing a series of coordinates in order to detect possibles anomalies in the coordinates.

## Requirements

1. An arduino board that supports serial communications
2. Arduino IDE
3. Python

## Quick launch guide

Follow these steps for a quick launch:
1. Open the Arduino project folder [genetic_curve_fitting](./genetic_curve_fitting) with Arduino IDE.
2. From [genetic_curve_fitting.ino](./genetic_curve_fitting/genetic_curve_fitting.ino) Upload the project unto the Arduino board.
3. Open [genetic_curve_fitting.py](genetic_curve_fitting.py). In the main function, change the `serial.Serial` port to your board serial port. By default, it is set to `/dev/ttyACM0`.
4. (Optional) Open [genetic_curve_fitting.py](genetic_curve_fitting.py). In the main function, change the `test3csv` to the file you wish to load. By default, it is set to `test3.csv`.
5. (Optional) Open [genetic_curve_fitting.py](genetic_curve_fitting.py). In the main function, all the parameters for the genetic algorithm are defined and can be modified to match the dataset
6. In the current folder [genetic_curve_fitting_prototype](.),launch the prototype with the following command:
```bash
python3 genetic_curve_fitting.py
```


## How it works

The prototypes attempts to estimate the correlation betwewen a series of point with a polynomial equation and uses it to identify anomalies. This process is done in the  [genetic_curve_fitting.py](genetic_curve_fitting.py) file:
1. The data points are read from a file
2. The prototype paramaters are defined
3. A serial connection with the Arduino board is initiated
4. A few coordinates (x) are sent over in order to estimate the first equation
5. The genetic algorithm from the Arduino is applied in order to find the polynomial equation that represents the points the best
6. The coefficients are sent from the Arduino to the device executing the python application
7. The python applications uses the calculated coefficients in order to determinee if the following coordinate is and anomaly
8. The coordinate is excluded if it is
9. x+1 are coordinates are sent over and the process is repeated until no points are left
10. The python application outputs a graph showing the excluded points in red and the final polynomial equation
