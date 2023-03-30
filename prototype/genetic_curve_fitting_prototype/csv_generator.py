import csv
import random

def csvGenerator(name): 
    with open(name, 'w', newline='') as file:

        writer = csv.writer(file,delimiter= ',',quotechar='|', quoting=csv.QUOTE_MINIMAL)

        for x in range (50):

            if random.random() < 0.05:
                value = random.random() * 100
                writer.writerow([x,value])

            else:    
                value = (random.random() - 0.5) * 2 + pow(x,2)
                writer.writerow([x,value])
                
if __name__ == '__main__':
    csvGenerator("data.csv")               
