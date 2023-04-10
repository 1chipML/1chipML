# K-Means Clustering

## Presentation

The current prototype offers the logic necessary to perform K-Means clustering on a set of randomly generated points. A python scripts generates points which can be easily clustered. The points are then sent to an Arduino Board that computes the K-Means algorithm and returns backs the coordinates of the centroids as well as a vector assigning each point to a cluster.

## Requirements

1. An arduino board that supports serial communications
2. Arduino IDE (or VS Code with the Arduino Extension)
3. Python 3.10
4. All dependencies present in the requirements.txt. To install all the dependencies run the command :
```bash
pip install -r requirements.txt
```


## Quick launch guide

Follow these steps for a quick launch:
1. Open the Arduino project folder [clustering.ino](./clustering.ino) with Arduino IDE.
2. From [clustering.ino](./clustering/clustering.ino) Upload the project unto the Arduino board.
3. Open the [clustering.py](clustering.py) file
4. Change the number of points generated, the number of clusters needed and the dimensions of the points.
5. Update the SERIAL_PORT_NAME variable to match the port corresponding to the Arduino board (for linux, it usually is `/dev/ttyACM0` and for mac it usually is `/dev/cu.usbmodemXXXXX`)
6. Run the following command in a terminal :
```bash
python clustering.py
```
