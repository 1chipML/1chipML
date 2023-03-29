import serial
import time
import struct


'''
Custom serial class to write and read data
'''
class CustomSerial:
    """Custom serial class for communication
    """

    def __init__(self, address: str, baudRate: int):
        """Constructor of the serial communication. Opens the serial communication by default

        Args:
            address (str): The serial address. e.g.: /dev/ttyACM0
            baudRate (int): The communication baud rate. e.g.: 115200
        """
        self.port: serial.Serial = None
        self.openSerial(address, baudRate)

    def __del__(self):
        self.closeSerial()

    def closeSerial(self) -> None:
        """Closes a serial communication
        """
        if self.port is not None:
            self.port.close()
            self.port = None

    def openSerial(self, address: str, baudRate: int) -> None:
        """Opens a serial communication

        Args:
            address (str): The serial address. e.g.: /dev/ttyACM0
            baudRate (int): The communication baud rate. e.g.: 115200
        """
        self.closeSerial()

        self.port = serial.Serial(address, baudRate, bytesize=serial.EIGHTBITS, 
                        parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                        timeout=None, xonxoff=False, rtscts=False)
        
        # Must wait 2 seconds in order to give some boards the time to reset
        time.sleep(2)
        
        self.port.reset_input_buffer()
        self.port.reset_output_buffer()

    def writeElement(self, packing: str, element) -> None:
        """Writes an element to the serial port. Is blocking.

        Args:
            packing (str): The packing structure of the element. e.g.: <f, <H
            element (Any): The element to write to the serial port
        """
        self.port.write(struct.pack(packing, element))
        self.port.flush()
    
    def readElement(self, packing: str, sizeOfElement: int):
        """Reads an element from the serial port. Is blocking.

        Args:
            packing (str): The packing structure of the element. e.g.: <f, <H
            sizeOfElement (int): The size of the element to read, in bytes.

        Returns:
            Any: The element read from the serial port
        """
        return struct.unpack(packing, self.port.read(sizeOfElement))[0]
    
    def writeArray(self, packing: str, array) -> None:
        """Writes an array to the serial port. Is blocking.

        Args:
            packing (str): The packing structure of each element. e.g.: <f, <H
            array (Any): The array containing the values to be written
        """
        for element in array:
            self.writeElement(packing, element)

    def readArray(self, packing: str, sizeOfElement: int, sizeOfArray: int):
        """Reads an array from the serial port. Is blocking.

        Args:
            packing (str): The packing structure of each element. e.g.: <f, <H
            sizeOfElement (int): The size of each element to read, in bytes.
            sizeOfArray (int): The size of the array to read.

        Returns:
            Any: The array read from the serial port
        """
        returnedArray = []
        for i in range(sizeOfArray):
            returnedArray.append(self.readElement(packing, sizeOfElement))

        return returnedArray
