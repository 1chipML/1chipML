import struct
import time
import serial
import numpy as np
import matplotlib.pyplot as plt
import random
from sklearn.cluster import KMeans
import seaborn as sns
import pandas as pd

def generate_data(nb_clusters, dimensions, nb_points, start=-100, end=100):

    noise_size = (end - start) * 0.20

    points_per_cluster = int(nb_points / nb_clusters)

    data = []

    centers = []
    for cluster in range(nb_clusters):
        for _ in range(dimensions):
            coord = random.random() * (end - start) - end
            centers.append(coord)

    current_nb_points = nb_points
    for cluster in range(nb_clusters - 1):
        for _ in range(points_per_cluster):
            for d in range(dimensions):
                coord = centers[cluster + d] + random.random() * noise_size - noise_size / 2
                data.append(coord)
        current_nb_points -= points_per_cluster
            
    for _ in range(current_nb_points):
        for d in range(dimensions):
            coord = centers[-(dimensions - d)] + random.random() * noise_size - noise_size / 2
            data.append(coord)

    np_data = np.reshape(np.array(data), (nb_points, dimensions)) 
    np.random.shuffle(np_data)
    return np_data

def main():
    np.set_printoptions(suppress=True) # Prevents from printing in scientific notation

    SERIAL_PORT_NAME = '/dev/cu.usbmodem1101'

    port = serial.Serial(SERIAL_PORT_NAME, 9600)
    time.sleep(2)

    nb_points = 100
    nb_clusters = 5
    dimensions = 2

    data = generate_data(nb_clusters, dimensions, nb_points)
    data_to_send = list(map(lambda x: float(x), data.flatten()))

    port.write(struct.pack('<H', nb_points))
    port.write(struct.pack('<H', nb_clusters))
    port.write(struct.pack('<H', dimensions))
    for d in data_to_send:
        port.write(struct.pack('<f', d))

    print('Data sent')

    centroids = np.zeros(nb_clusters * dimensions)
    for i in range(nb_clusters * dimensions):
        centroids[i] = float(round(struct.unpack('<f', port.read(4))[0], 3))
    centroids = centroids.reshape((nb_clusters, dimensions))

    assignations = np.zeros(nb_points, dtype=int)

    for i in range(nb_points):
        read = port.read(2)
        assignations[i] = int(struct.unpack('<H', read)[0])

    port.close()

    model = KMeans(nb_clusters).fit(data)

    print(f'Centroids : {centroids}')
    print(f'Sklearn : {model.cluster_centers_}')
    print(assignations)

    # Show result
    centroids_to_show = np.array(centroids).reshape((nb_clusters, dimensions))
    centroids_x = centroids_to_show[:, 0]
    centroids_y = centroids_to_show[:, 1]
    if dimensions > 2:
        centroids_z = centroids_to_show[:, 2]

    data_x = data[:, 0]
    data_y = data[:, 1]
    if dimensions > 2:
        data_z = data[:, 2]


    centroids_used = list(set(assignations))
    print('Used : ', centroids_used)
    centroids_used += list(set(range(nb_clusters)) - set(assignations))

    if dimensions == 2:
        df = pd.DataFrame({'x': data_x, 'y': data_y, 'cluster': assignations, 'type': ['data'] * nb_points})
        df = pd.concat([df, pd.DataFrame({'x': centroids_x, 'y': centroids_y, 'cluster': centroids_used, 'type': ['centroid'] * nb_clusters})], ignore_index=True, axis=0)

        sns.set_style("darkgrid")
        sns.scatterplot(data=df, x='x', y='y', hue='cluster', s=100, style='type', markers={'data': 'X', 'centroid': 's'})  
    else :
        plt.figure (figsize = (10, 10))
        plot = plt.axes (projection='3d')
        plot.scatter(data_x, data_y, data_z, marker='o', c=assignations, cmap='RdBu', s=50)
        plot.scatter(centroids_x, centroids_y, centroids_z, marker='x', c=centroids_used, cmap='RdBu', s=80)
        plot.set_xlabel ('x')
        plot.set_ylabel ('y')
        plot.set_zlabel ('z')
    
    plt.show()




if __name__ == "__main__":
    main()

