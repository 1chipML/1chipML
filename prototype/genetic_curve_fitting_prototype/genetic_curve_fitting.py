import time
import struct
import serial
import numpy as np
import csv
import matplotlib.pyplot as plt


"""Use calculated coefficiants to estimate the data point

:param x The x data point to estimate ethe value
:param parameters: The coefficients that are used (they are all between 0 and 1)
:param limits: The maximum value of each coefficient in a list
:param polynomialDegree: The maximum polynomial degree of the equation
:returns: estimated y value
"""
def predict(x,parameters,limits, polynomialDegree):
  val = 0
  
  for i in range(polynomialDegree+1):
    val += (parameters[i] - 0.5) * 2 * limits[i] * pow(x,i)
  
  return val

"""Sends coordinates to the Arduino through a serial port connection

:param xCoordinates: A list containing the values of x for each data point
:param yCoordinates: A list containing the values of y for each data point
:param nbOfCoordinatesToSend: The number of coordinates to send through serial connection
:param port: The PySerial port connection
"""
def sendCoordinates(xCoordinates,yCoordinates,nbOfCoordinatesToSend,port):
  port.write(struct.pack('<H',nbOfCoordinatesToSend))
  
  for x in xCoordinates[:nbOfCoordinatesToSend]:
    x = float(x)
    port.write(struct.pack('<f',float(x)))
  
  for y in yCoordinates[:nbOfCoordinatesToSend]:
    port.write(struct.pack('<f',float(y)))

"""Sends the limits for each coefficient through the serial connection to the Arduino

:param limits: A list containing the limits for each coefficient
:param port: The PySerial port connection
"""    
def sendLimits(limits,port):
    for x in limits:
      port.write(struct.pack('<f',x))

"""Obtains the values calulated by the Arduino 

:param port: The PySerial port connection
:param polynomialDegree: The maximum polynomial degree of the equation
""" 
def getResults(port,polynomialDegree):
  fitness = struct.unpack('<f',port.read(4))[0]

  values = list()
  for _ in range(polynomialDegree+1):
    value = struct.unpack('<f',port.read(4))[0]
    values.append(value)

  return fitness,values

def main():
  testYCoordinates = list()
  testXCoordinates = list()
  coordinatesSize = 0 
  
  #This can be modifieed to read another seet of coordinates
  with open("test3.csv", 'r') as file:
    csvreader = csv.reader(file)
    for row in csvreader:
      testYCoordinates.append(float(row[1]))
      testXCoordinates.append(float(row[0]))
      coordinatesSize+=1

  # All of these values are sent to the Arduino and control the execution of the genetic algorithm

  minCoordinates = 3
  epsilon = 0.
  mutationRate = 0.1
  populationSize = 50
  tourneySize = 5
  maxIterations = 5
  
  # The absolute limits of each coefficient (must be positive)
  limits = [1.,1.,1.]
  polynomialDegree = 2
  cutoffValue =  10.0

  port = serial.Serial("/dev/ttyACM0", 9600)
  
  #Necessary after establishing a serial connection with the Arduino
  time.sleep(2)


  anomaly_listX = list()
  anomaly_listY = list()

  while (minCoordinates < coordinatesSize):
    port.write(struct.pack('<f',epsilon))
    port.write(struct.pack('<f',mutationRate))
    port.write(struct.pack('<H',populationSize))
    port.write(struct.pack('<H',tourneySize))
    port.write(struct.pack('<H',maxIterations))
    port.write(struct.pack('<H',polynomialDegree))

    sendCoordinates(testXCoordinates,testYCoordinates,minCoordinates,port)   
    sendLimits(limits,port)
    
    
    _,bestValues =  getResults(port,polynomialDegree)
    
    # We exclude a point if it is very far from the predicted value
    
    prediction = predict(testXCoordinates[minCoordinates], bestValues, limits, polynomialDegree)
    
    if (min(abs(prediction - testYCoordinates[minCoordinates]), abs((prediction - testYCoordinates[minCoordinates]) / testYCoordinates[minCoordinates] * 100) ) > cutoffValue):
        anomaly_listX.append(testXCoordinates[minCoordinates])
        anomaly_listY.append(testYCoordinates[minCoordinates])
        testXCoordinates.pop(minCoordinates)
        testYCoordinates.pop(minCoordinates)
        coordinatesSize-=1
    else:
      minCoordinates+=1

  port.close()

  fig = plt.figure()
  ax1 = fig.add_subplot(111)
  ax1.scatter(testXCoordinates, testYCoordinates, c='b')
  ax1.scatter(anomaly_listX, anomaly_listY, c='r')
  
  line_x = np.sort(np.array(testXCoordinates+anomaly_listX))
  line_y = list()

  for x in line_x:
    line_y.append(predict(x,bestValues,limits,polynomialDegree))

  print("The final equation is:")
  
  equation=""
  exponent = 0
  for val in bestValues:
    equation += str(val*limits[exponent])
    equation += "x^" + str(exponent) + " "
    exponent += 1     
  print(equation)

  plt.plot(line_x, line_y)
  plt.show()
  port.close()

if __name__ == "__main__":
  main()
