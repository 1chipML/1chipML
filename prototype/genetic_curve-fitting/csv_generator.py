import csv
import random

with open("test3.csv", 'w', newline='') as file:

    writer = csv.writer(file,delimiter= ',',quotechar='|', quoting=csv.QUOTE_MINIMAL)
    
    for x in range (50):
        value = (random.random()-0.5)*2+pow(x,2)
        writer.writerow([x,value])