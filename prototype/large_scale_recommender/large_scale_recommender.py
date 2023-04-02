import csv, struct, serial, time, array
import matplotlib.pyplot as plt
import numpy as np
from math import sqrt

def wait_for_arduino(port, nbValues):
    new_dataset = [0 for x in range(nbValues)] 
    print("waiting for arduino...")
    for i in range(nbValues):
        new_dataset[i] = struct.unpack('<f', port.read(4))[0]
    return  new_dataset

def display_results(dataset):
    size_side = int(sqrt(len(dataset)))
    print(f"Received a matrix of {size_side}x{size_side} = {len(dataset)} values")

    # Display the results
    print(np.array(dataset).reshape(size_side, size_side))

def send_data_through_serial(port, dataset):
    nb_data = len(dataset)
    size_side = sqrt(nb_data)

    port.write(struct.pack('<B', int(size_side)))

    print(f"We need to send a matrix of {size_side}x{size_side} = {nb_data} values")

    for val in dataset:
        port.write(struct.pack('<f', val))
    return


def main():
    port = serial.Serial('/dev/ttyACM0', 9600)
    time.sleep(2)

    data = [0.2, -1, -1,  1, -1, -1,  0.6,
            -1,  0.4, -1,  0.6, -1, -1,  1,
            -1, -1, -1, -1,  0.6, -1,  1,
            0.4, -1,  1, -1, -1,  0.6, -1,
            -1, -1, -1,  1, -1, -1,  1,
            -1,  1, -1, -1,  1, -1, -1,
            0.2, -1, -1,  0.2, -1, -1, -1]

    send_data_through_serial(port, data)
    new_dataset = wait_for_arduino(port, len(data))
    display_results(new_dataset)

if __name__ == "__main__":
    main()
