# Shortest path graph traversal (Monte Carlo Reinforcement Learning Method)

## Presentation

The current prototype uses a Monte Carlo RL approach to find the shortest path to traverse a graph.

## Requirements

1. An arduino board that supports serial communications
2. Arduino IDE
3. Python
4. Numpy
5. PySerial

## Quick launch guide

Follow these steps for a quick launch:
1. Open the Arduino project folder [arduino_mc_shortest_path](./arduino_mc_shortest_path) with Arduino IDE.
2. From [arduino_mc_shortest_path.ino](./arduino_mc_shortest_path/arduino_mc_shortest_path.ino) Upload the project unto the Arduino board.
3. Open [mc_shortest_path.py](mc_shortest_path.py). In the main function, change the `serial.Serial` port to your board serial port. By default, it is set to `/dev/tty.usbmodem2101`.
4. (Optional) Open [mc_shortest_path.py](mc_shortest_path.py). In the main function, change the values of the board to change the weights of each node.
6. In the current folder [mc_shortest_path](.), launch the prototype with the following command:
```bash
python3 ./arduino_mc_shortest_path/genetic_curve_fitting.py
```

## How it works

The prototype finds the next best action to execute in order to traverse the graph by means of the shortest path.

The starting point is the top left corner of a 3x3 graph (9 nodes), and the end point is the bottom right corner of the graph. Every node has a specific weight associated. The weight determines the distance from another node to this node, adding it to the total distance of the path. -1 is the weight representing the current position of the player in the graph. 

The only possible directions when moving from one node to another are UP, DOWN, LEFT or RIGHT. If there is no node in a given direction, it isn't possible to go in that direction. 

After simulating a number of episodes or after reaching a certain threshold, the next best action is returned to the user. 