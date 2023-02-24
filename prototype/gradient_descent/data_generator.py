import random
from typing import Tuple
import pandas as pd

def generate(coeff: "list[float]", x: float) -> float:
    y_predict:float = 0

    for i, val in enumerate(coeff):
        y_predict += val * pow(x, i)

    return y_predict

def randomSign():
    return 1 if random.random() < 0.5 else -1

def generateData(degree: int, outliersOdds: float, nPoints: int, filename: str, outlierRange: Tuple[int, int], normalRange: Tuple[int, int]): 
    coeff = []
    for _ in range(degree + 1):
        coeff.append(random.uniform(-10, 10))

    x = []
    y = []
    for i in range(-nPoints//2, nPoints//2):
        x.append(float(i))

        y_gen = generate(coeff, float(i))
    
        if (random.random() < outliersOdds and i > -nPoints//2 + 15):
            y_gen += random.uniform(outlierRange[0], outlierRange[1]) * randomSign()
        else:
            y_gen += random.uniform(normalRange[0], normalRange[1]) * randomSign()

        y.append(y_gen)

    df = pd.DataFrame()
    df["x"] = x
    df["y"] = y
    df.to_csv(filename)
    
generateData(1, 0.2, 40, f"Linear_20%_outliers.csv", (500, 1000,), (0, 200,))
generateData(3, 0.2, 40, f"Cubic_20%_outliers.csv", (5000, 10000,), (0, 2000,))
    
        