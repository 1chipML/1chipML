from serial_port import CustomSerial
import csv
import matplotlib.pyplot as plt
import numpy as np

def wait_for_arduino(port: CustomSerial, nbValues):
    nbDims = port.readElement('<b', 1)
    new_dataset = [[0 for x in range(nbDims)] for y in range(nbValues)] 
    for i in range(nbValues):
        for j in range(nbDims):
            new_dataset[i][j] = port.readElement('<f', 4) / 10
    return  new_dataset

def display_results(dataset):
    np_dataset = np.array(dataset)
    np_first_dataset = np_dataset[0:49]
    np_second_dataset = np_dataset[50:99]
    
    plt.scatter(np_first_dataset[:, 0], np_first_dataset[:, 1], c="red", label="Iris-setosa")
    plt.scatter(np_second_dataset[:, 0], np_second_dataset[:, 1], c="blue", label="Iris-versicolor")

    plt.xlabel('Second principal component')
    plt.ylabel('First principal component')

    plt.legend(loc='lower center')
    plt.tight_layout()
    plt.show()

def send_data_through_serial(port: CustomSerial, dataset):
    nb_entries = len(dataset)
    nb_caract_per_entry = len(dataset[0])
    print(f"We need to send {nb_entries} entries")
    print(f"Each entry is composed of {nb_caract_per_entry} caracteristics")
    
    port.writeElement('<B', nb_entries)
    port.writeElement('<B', nb_caract_per_entry)

    for entry in dataset:
        for value in entry:
            # Value is a float. We multiply it by 10 to normalize the data
            # between 0 and 100.
            port.writeElement('<B', int(float(value) * 10))


def load_dataset() :
    with open('dataset.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        return list(csv_reader)

def main():
    port: CustomSerial = CustomSerial('/dev/ttyACM0', 9600)

    data = load_dataset()
    send_data_through_serial(port, data)
    new_dataset = wait_for_arduino(port, len(data))
    display_results(new_dataset)

    port.closeSerial()

if __name__ == "__main__":
    main()
