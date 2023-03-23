import time
import struct
import serial
import numpy as np
import csv
import matplotlib.pyplot as plt

def predict(x,parameters):
  return parameters[0] + parameters[1]*(x)+ parameters[2]* pow(x,2)

def sendCoordinates(testXCoordinates,testYCoordinates,minCoordinates,port):
  
  port.write(struct.pack('<H',minCoordinates))
  
  for x in testXCoordinates[:minCoordinates]:
    x = float(x)
    port.write(struct.pack('<f',float(x)))
  
  for y in testYCoordinates[:minCoordinates]:
    port.write(struct.pack('<f',float(y)))
    
def sendLimits(limits,port):
    for x in limits:
      port.write(struct.pack('<f',x))
  
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
  with open("test3.csv", 'r') as file:
    csvreader = csv.reader(file)
    for row in csvreader:
      testYCoordinates.append(float(row[1]))
      testXCoordinates.append(float(row[0]))

  minCoordinates = 3
  coordinatesSize = 30
  epsilon = 0.
  mutationRate = 0.1
  populationSize = 50
  tourneySize = 5
  maxIterations = 5
  limits = [1.,1.,1.]
  polynomialDegree = 2
  cutoffValue =  10.0

  port = serial.Serial("/dev/ttyACM0", 9600)
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
    if (abs(predict(testXCoordinates[minCoordinates],bestValues) - testYCoordinates[minCoordinates]) > cutoffValue):
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
  
  line_x = np.array(testXCoordinates)
  line_y = list()

  for x in line_x:
    line_y.append(predict(x,bestValues))

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