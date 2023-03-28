import struct
import time
import serial
import numpy as np

def main():
    np.set_printoptions(suppress=True) # Prevents from printing in scientific notation

    port = serial.Serial('/dev/tty.usbmodem2101', 9600)
    time.sleep(2)

    size = 9
    port.write(struct.pack('<B', size)) # Make sure that size in the arduino program is of 1 byte
    
    boardValues = [1, 2, 3, 1, 2, 4, 5, 2, 1]
    for value in boardValues:
        port.write(struct.pack('<b', value))

    nPlayers = 0
    port.write(struct.pack('<B', nPlayers))

    print('Trying to read from port')

    xPos = struct.unpack('<b', port.read(1))[0]
    yPos = struct.unpack('<b', port.read(1))[0]

    port.close()

    print("Best action: [", xPos, ",", yPos, "]")

if __name__ == "__main__":
    main()