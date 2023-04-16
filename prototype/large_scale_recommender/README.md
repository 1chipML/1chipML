# Large Scale Recommender

## Presentation

This prototype tries to solve the matrix factorization problem used in some large scale recommendation systems. To do so,
it takes a square matrix as an input with some missing values. In this case, the input matrix is called the "feedback matrix". 
The feedback matrix contains the feedback of some users (rows) about some products (columns). When a value is '-1' this means
that we did not gather the feedback of the user for this particular product. This prototype then tries to predict the missing
values to know which products would be the most interesting to present to the user. A value of 1 means that the user is really interested
and a value of 0 means that the user does not like the product.

## Requirements

1. An arduino board that supports serial communications
2. Arduino IDE
3. Python

## Quick launch guide

Follow these steps for a quick launch:

1. Open the Arduino project folder [large_scale_recommender](.) with Arduino IDE.
2. From [large_scale_recommender.ino](./large_scale_recommender.ino) Upload the project unto the Arduino board.
3. Open [large_scale_recommender.py](large_scale_recommender.py). In the main function, update the serial name to your board serial port. By default, it is set to `/dev/ttyACM0`.
4. Run the prototype
```bash
python3 large_scale_recommender.py
```

## How it works

The large scale recommendation system solves the matrix factorization problem with the following steps:
1. The arduino boards receives the feedback matrix through the serial port.
2. The LU Decomposition algorithm is applied to the feedback matrix to get a good first guess of the factorization.
3. A gradient descent algorithm is applied on the factorized matrix to try and minimize the cost function. The finite_difference algorithm is used so that we don't have to manually compute  the derivative.
4. The 2 matrices are then multiplied together to get the predicted feedback matrix. 
5. The prediction is returned to the computer.
