from serial_port import CustomSerial
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

def encode_arduino_input(matrix):
    return int(matrix.shape[0]), list(map(lambda x: float(x), matrix.flatten()))

def main():
    np.set_printoptions(suppress=True) # Prevents from printing in scientific notation
    
    size_of_image = 8
    size_of_float_arduino_uno = 4

    port: CustomSerial = CustomSerial('/dev/ttyACM0', 9600)

    # Read img
    img = np.array(Image.open(f'seven_{size_of_image}.bmp').convert('L'))

    figure, axis = plt.subplots(3, 3)
    figure.delaxes(axis[2, 2])

    # Compute covariance matrix
    mean_values = img.mean(axis=0)
    centered_data = img - mean_values
    covariance_matrix = np.round(np.cov(centered_data.T), 3)

    # Encode covariance matrix and send it to arduino
    size, data = encode_arduino_input(covariance_matrix)

    print('Size : ', size)
    print('Data : ', data, '\n')

    port.writeElement('<i', size) # Make sure that size in the arduino program is a 4 byte integer
    port.writeArray('<f', data)

    # Retrieve eigenvalues and eigenvectors
    print('Trying to read from port')

    eig_vec = np.zeros((size * size))
    for i in range(size * size):
        eig_vec[i] = round(port.readElement('<f', size_of_float_arduino_uno), 3)
    eig_vec = eig_vec.reshape((size, size))

    eig_val = np.zeros((size * size))
    for i in range(size * size):
        eig_val[i] = round(port.readElement('<f', size_of_float_arduino_uno), 3)
    eig_val = eig_val.reshape((size, size))
    

    print('Vectors : ', eig_vec)
    print('Values : ', eig_val)
    port.closeSerial()

    # Sort eigenvalues and eigenvectors on descending order and take only k components
    for components in range(1, size_of_image + 1, size_of_image // 8):
        indices = np.argsort(-eig_val.diagonal())
        new_eig_vec = eig_vec[:,indices][:components, :]
        pca_data = np.dot(centered_data, new_eig_vec.T)
        reconstructed = np.dot(pca_data, new_eig_vec) + mean_values
        compression_ratio = (1 - (components * size / (size * size))) * 100
        current_axis = axis[(components - 1) // 3, (components - 1) % 3]
        current_axis.title.set_size(1)
        current_axis.imshow(reconstructed, cmap='gray')
        current_axis.set_title(f'Components={components}\n Compression={compression_ratio} %')
        current_axis.axis('off')
    
    plt.show()

if __name__ == "__main__":
    main()

