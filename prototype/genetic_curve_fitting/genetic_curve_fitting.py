from serial_port import CustomSerial
import numpy as np
import csv
import matplotlib.pyplot as plt


"""Use calculated coefficients to estimate the data point

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
def sendCoordinates(xCoordinates, yCoordinates, nbOfCoordinatesToSend, port: CustomSerial):
  port.writeElement('<H', nbOfCoordinatesToSend)
  
  for x in xCoordinates[:nbOfCoordinatesToSend]:
    port.writeElement('<f', float(x))
  
  for y in yCoordinates[:nbOfCoordinatesToSend]:
    port.writeElement('<f', float(y))

"""Sends the limits for each coefficient through the serial connection to the Arduino

:param limits: A list containing the limits for each coefficient
:param port: The PySerial port connection
"""    
def sendLimits(limits, port: CustomSerial):
    port.writeArray('<f', limits)

"""Obtains the values calulated by the Arduino 

:param port: The PySerial port connection
:param polynomialDegree: The maximum polynomial degree of the equation
""" 
def getResults(port: CustomSerial, polynomialDegree):
  fitness = port.readElement('<f', 4)
  values = port.readArray('<f', 4, polynomialDegree + 1)

  return fitness,values

def curveFitting():
  yCoordinates = list()
  xCoordinates = list()
  coordinatesSize = 0 
  
  #This can be modifieed to read another set of co  ordinates
  with open("data.csv", 'r') as file:
    csvreader = csv.reader(file)
    for row in csvreader:
      yCoordinates.append(float(row[1]))
      xCoordinates.append(float(row[0]))
      coordinatesSize += 1

  # All of these values are sent to the Arduino and control the execution of the genetic algorithm

  minCoordinates = 3
  epsilon = 0.
  mutationRate = 0.1
  populationSize = 50
  tourneySize = 10
  maxIterations = 5
  numberOfEliteValues = 2
  
  # The absolute limits of each coefficient (must be positive)
  limits = [1.,1.,1.]
  polynomialDegree = 2
  cutoffValue =  10.0

  port: CustomSerial = CustomSerial("/dev/ttyACM0", 9600)

  anomaly_listX = list()
  anomaly_listY = list()

  while (minCoordinates < coordinatesSize):
    port.writeElement('<f', epsilon)
    port.writeElement('<f', mutationRate)
    port.writeElement('<H', populationSize)
    port.writeElement('<H', tourneySize)
    port.writeElement('<H', maxIterations)
    port.writeElement('<H', polynomialDegree)
    port.writeElement('<H', numberOfEliteValues)
    sendCoordinates(xCoordinates,yCoordinates,minCoordinates,port)   
    sendLimits(limits,port)
    
    
    _, bestValues =  getResults(port, polynomialDegree)
    
    # We exclude a point if it is very far from the predicted value
    
    prediction = predict(xCoordinates[minCoordinates], bestValues, limits, polynomialDegree)
    
    difference = abs(prediction - yCoordinates[minCoordinates])
    percentageDifference = abs(prediction - yCoordinates[minCoordinates]) / yCoordinates[minCoordinates] * 100
    
    
    if (min(difference, percentageDifference) > cutoffValue):
        anomaly_listX.append(xCoordinates[minCoordinates])
        anomaly_listY.append(yCoordinates[minCoordinates])
        xCoordinates.pop(minCoordinates)
        yCoordinates.pop(minCoordinates)
        coordinatesSize -= 1
    else:
      minCoordinates += 1

  port.closeSerial()

  fig = plt.figure()
  ax1 = fig.add_subplot(111)
  ax1.scatter(xCoordinates, yCoordinates, c='b')
  ax1.scatter(anomaly_listX, anomaly_listY, c='r')
  
  line_x = np.sort(np.array(xCoordinates+anomaly_listX))
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

if __name__ == "__main__":
  curveFitting()
