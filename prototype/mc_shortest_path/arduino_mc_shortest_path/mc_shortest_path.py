import struct
import time
import serial
import numpy as np

def main():
    size = 9
    boardValues = [-1, 10, 10, 1, 1, 10, 10, 1, 1]
    lastIdx = 0
    
    np.set_printoptions(suppress=True) # Prevents from printing in scientific notation
    
    port = serial.Serial('/dev/tty.usbmodem101', 9600)
    time.sleep(2) # Arduino takes time to initialize the serial connection
    
    while (True):
        port.write(struct.pack('<B', size)) # Make sure that size in the arduino program is of 1 byte
        
        for value in boardValues:
            port.write(struct.pack('<b', value))

        nPlayers = 1
        port.write(struct.pack('<B', nPlayers))

        xPos = struct.unpack('<b', port.read(1))[0]
        yPos = struct.unpack('<b', port.read(1))[0]

        print("Best action: [", xPos, ",", yPos, "]")
        
        if xPos == 2 and yPos == 2:
            break
        boardValues[lastIdx] = -2
        boardValues[xPos * 3 + yPos] = -1
        lastIdx = xPos * 3 + yPos

    port.close()

if __name__ == "__main__":
    main()