import time
from serial_port import CustomSerial
import numpy as np

def main():
    size = 9
    boardValues = [-1, 10, 10, 1, 1, 10, 10, 1, 1]
    nPlayers = 1
    lastIdx = 0
    
    np.set_printoptions(suppress=True) # Prevents from printing in scientific notation
    
    port = CustomSerial('/dev/tty.usbmodem101', 9600)
    time.sleep(2) # Arduino takes time to initialize the serial connection
    
    while (True):
        port.writeElement('<B', size) # Make sure that size in the arduino program is of 1 byte
        port.writeArray('<b', boardValues)
        port.writeElement('<B', nPlayers)

        xPos = port.readElement( '<b', 1)
        yPos = port.readElement('<b', 1)

        print("Best action: [", xPos, ",", yPos, "]")
        
        if xPos == 2 and yPos == 2:
            break
        boardValues[lastIdx] = -2
        boardValues[xPos * 3 + yPos] = -1
        lastIdx = xPos * 3 + yPos

    port.closeSerial()

if __name__ == "__main__":
    main()