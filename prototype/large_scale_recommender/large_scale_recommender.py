import csv, struct, serial, time, array
import matplotlib.pyplot as plt
import numpy as np
from math import sqrt

def wait_for_arduino(port, nbValues):
    new_dataset = [0 for x in range(nbValues)] 
    print("waiting for arduino...")
    for i in range(nbValues):
        new_dataset[i] = struct.unpack('<f', port.read(4))[0]
        if(new_dataset[i] < 0):
            new_dataset[i] = 0
        if(new_dataset[i] > 1):
            new_dataset[i] = 1
    return  new_dataset

def display_results(dataset_initial, dataset_final):
    size_side = int(sqrt(len(dataset_final)))
    print(f"Received a matrix of {size_side}x{size_side} = {len(dataset_final)} values")

    feedback_matrix_final = np.array(dataset_final).reshape(size_side, size_side)
    feedback_matrix_initial = np.array(dataset_initial).reshape(size_side, size_side)
    feedback_matrix_initial[feedback_matrix_initial == -1] = np.nan
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))
    im1 = ax2.imshow(feedback_matrix_final, cmap='coolwarm_r', vmin=0, vmax=1)
    ax2.figure.colorbar(im1, ax=ax2)

    ax2.set_xticks(np.arange(feedback_matrix_final.shape[1]))
    ax2.set_yticks(np.arange(feedback_matrix_final.shape[0]))
    ax2.set_xticklabels(['Product 1', 'Product 2', 'Product 3', 'Product 4', 'Product 5', 'Product 6', 'Product 7'])
    ax2.set_yticklabels(['User 1', 'User 2', 'User 3', 'User 4', 'User 5', 'User 6', 'User 7'])
    ax2.set_title("Final User Feedback Heat Map")

    im2 = ax1.imshow(feedback_matrix_initial, cmap='coolwarm_r', vmin=0, vmax=1)
    ax1.figure.colorbar(im2, ax=ax1)

    ax1.set_xticks(np.arange(feedback_matrix_initial.shape[1]))
    ax1.set_yticks(np.arange(feedback_matrix_initial.shape[0]))
    ax1.set_xticklabels(['Product 1', 'Product 2', 'Product 3', 'Product 4', 'Product 5', 'Product 6', 'Product 7'])
    ax1.set_yticklabels(['User 1', 'User 2', 'User 3', 'User 4', 'User 5', 'User 6', 'User 7'])
    ax1.set_title("Initial User Feedback Heat Map")

    for ax in [ax1, ax2]:
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
             rotation_mode="anchor")

    fig.suptitle("User Feedback Heat Maps")

    plt.show()

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
            0.2, -1, 0.6,  0.2, -1, -1, -1]

    send_data_through_serial(port, data)
    new_dataset = wait_for_arduino(port, len(data))
    display_results(data, new_dataset)

if __name__ == "__main__":
    main()
