# Principal Component Analysis - Lanczos

## Presentation

This prototype showcase the application of a principal component analysis (PCA) on a classification dataset. 
The prototype reads a cvs file containing multiple entries with flower's caracteristics. 
It then applies a PCA to reduce the dimensionality of the data and displays the final result on a 2D graph.  


## Requirements

1. An arduino board that supports serial communications
2. Arduino IDE
3. GCC

## Quick launch guide

Follow these steps for a quick launch:
1. Open the Arduino project folder [pca](./pca) with Arduino IDE.
2. From [pca.ino](./pca/pca.ino) Upload the project unto the Arduino board.
3. Open [pca.py](pca.py). In the main function, update the serial name to your board serial port. By default, it is set to `/dev/ttyACM0`.
4. Run the prototype
```bash
python3 pca.py
```

## How it works

1. The dataset is loaded from the cvs file to memory in the python file.
2. This data is then transmitted to the arduino through the serial connection.
3. The arduino computes the covariance matrix.
4. The Lanczos algorithm is applied to the covariance matrix to simplifiy the research of the eigen vectors and eigenvalues
5. The eigen vectors and values are then extracted using the Jacobi method.
6. The highest eigenvectors are extracted from the matrix and the associated eigenvectors are used to create the transoformation matrix.
7. The transformation matrix is applied to the initial matrix to transform the dataset.
8. The result is sent back to the computer
9. A graph is produced displaying the values after the transformation.
