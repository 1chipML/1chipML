#include "gradient_descent.h"

/**
 * @brief Transforms an N dimension function into a 1 dimension function from the initialPoint along the given direction
 * 
 * @param func Initial function to transform
 * @param x Value at which to evaluate the function
 * @param initialPoint Initial point to base the movement of
 * @param direction Direction in which to move by x
 * @param n Number of dimensions of the original function
 * @return Function value after moving by x from the initial point in the given direction
 */
static gradient_real oneDimension(function func, gradient_real x,
                                  gradient_real initialPoint[],
                                  gradient_real direction[], int n) {
  gradient_real point[n];

  // Compute new point when moving by x in the given direction
  for (int i = 0; i < n; i++) {
    point[i] = initialPoint[i] + x * direction[i];
  }

  return func(point);
}

/**
 * @brief Determines the function minimum from 3 points initially bracketing the minimum. Uses a mix of parabolic interpolation and Golden Section search.
 * 
 * @param xMin Abscissa of the minimum. Will contain the Abscissa of the minimum after function executes
 * @param min Return parameter containing the value of the function at the minimum
 * @param bracket Initial 3 points bracketing the minimum
 * @param func Function transforming an N dimension function to a 1 dimension function
 * @param f Function we want to transfor to a 1 dimension function
 * @param initialPoint Initial point to base the movement of
 * @param direction Direction in which to move
 * @param n Number of dimensions of the original function
 * @param tol Tolerance for the minimum estimate. Should be no smaller then the square-root of the machine floating point precision
 * @return Value indicating if brent was a succes or an error
 */ 
static int brent(gradient_real *xMin, gradient_real* min, Bracket bracket,
                           f1dimension func, function f,
                           gradient_real initialPoint[],
                           gradient_real direction[], int n,
                           gradient_real tol) {

  // Distance moved
  gradient_real distance = 0.0;

  // Distance moved previously
  gradient_real previousDistance = 0.0;

  // Minimum is bracketed between a and b
  gradient_real a = fmin(bracket.a, bracket.c);
  gradient_real b = fmax(bracket.a, bracket.c);

  // x : Point with the smallest function value found so far
  // secondXMin : Point with the second smallest function value found so far
  // prevSecondXMin : Previous value of secondXMin
  gradient_real x, prevSecondXMin, secondXMin;
  x = prevSecondXMin = secondXMin = bracket.b;

  // Value of the function at the above points
  gradient_real fx, fPrevSecondXMin, fSecondXMin;
  fx = fPrevSecondXMin = fSecondXMin = func(f, x, initialPoint, direction, n);

  for (int iter = 0; iter < ITMAX_BRENT; iter++) {
    gradient_real midpoint = 0.5 * (a + b);

    // Tolerance for estimates
    gradient_real tol1 = tol * fabs(x) + EPS;
    gradient_real tol2 = 2.0 * tol1;

    // Checks if we found the minimum with enough precision
    if (fabs(x - midpoint) <= (tol2 - 0.5 * (b - a))) {
      *xMin = x;
      *min = fx;
      return GRADIENT_SUCCESS;
    }

    // No need to evaluate function if we haven't moved by at least tol1
    if (fabs(previousDistance) > tol1) {
      // Construct trial parabolic fit
      gradient_real r = (x - secondXMin) * (fx - fPrevSecondXMin);
      gradient_real q = (x - prevSecondXMin) * (fx - fSecondXMin);
      gradient_real p = (x - prevSecondXMin) * q - (x - secondXMin) * r;
      q = 2.0 * (q - r);

      if (q > 0.0) {
        p = -p;
      }
      q = fabs(q);

      // Check the acceptability of parabolic fit
      if (fabs(p) >= fabs(0.5 * q * previousDistance) || p <= q * (a - x) ||
          p >= q * (b - x)) {
        // Take the golden section step
        previousDistance = x >= midpoint ? a - x : b - x;
        distance = CGOLD * previousDistance;
      } else {
        // Take the parabolic step
        previousDistance = distance;
        distance = p / q;
        gradient_real u = x + distance;
        if (u - a < tol2 || b - u < tol2) {
          distance = SIGN(tol1, midpoint - x);
        }
      }
    } else {
      // Take the golden section step
      previousDistance = x >= midpoint ? a - x : b - x;
      distance = CGOLD * previousDistance;
    }

    // current: Most recent point at which function was evaluated
    gradient_real current = fabs(distance) >= tol1
                                ? x + distance
                                : x + SIGN(tol1, distance);
    gradient_real fCurrent = func(f, current, initialPoint, direction, n);

    // Form a more precise bracket for the minimum
    if (fCurrent <= fx) {
      if (current >= x) {
        a = x;
      } else {
        b = x;
      }

      prevSecondXMin = secondXMin;
      secondXMin = x;
      x = current;

      fPrevSecondXMin = fSecondXMin;
      fSecondXMin = fx;
      fx = fCurrent;
    } else {
      if (current < x) {
        a = current;
      } else {
        b = current;
      }
      if (fCurrent <= fSecondXMin || secondXMin == x) {
        prevSecondXMin = secondXMin;
        secondXMin = current;
        fPrevSecondXMin = fSecondXMin;
        fSecondXMin = fCurrent;
      } else if (fCurrent <= fPrevSecondXMin || prevSecondXMin == x ||
                 prevSecondXMin == secondXMin) {
        prevSecondXMin = current;
        fPrevSecondXMin = fCurrent;
      }
    }
  }

  // We should not reach this point to many iterations
  return GRADIENT_ERROR;
}

/**
 * @brief Initially brackets the minimum between 3 points (a, b, c). Uses parabolic interpolation to find the bracket
 * 
 * @param func Function transforming an N dimension function to a 1 dimension function
 * @param f Function we want to transfor to a 1 dimension function
 * @param initialPoint Initial point to base the movement of
 * @param direction Direction in which to move
 * @param n Number of dimensions of the original function
 * @return Structure containing the bracket for the minimum
 */
static Bracket bracketMinimum(f1dimension func, function f,
                              gradient_real initialPoint[],
                              gradient_real direction[], int n) {
  // Set a, b and c at arbitrary initial values
  Bracket bracket = {.a = 0.0, .b = 1.0, .c = 0.0};

  // Evaluate function at initial points a and b
  gradient_real fa = func(f, bracket.a, initialPoint, direction, n);
  gradient_real fb = func(f, bracket.b, initialPoint, direction, n);

  // We want to go downhill from a to b to bracket the minimum
  // so if fb > fa we need to swap them
  if (fb > fa) {
    gradient_real temp = bracket.a;
    bracket.a = bracket.b;
    bracket.b = temp;

    temp = fa;
    fa = fb;
    fb = temp;
  }

  // Initial guess for c
  bracket.c = bracket.b + GOLD * (bracket.b - bracket.a);
  gradient_real fc = func(f, bracket.c, initialPoint, direction, n);

  // Loop until we bracket the minimum
  while (fb > fc) {
    // Compute u the inflexion point of the parabola defined by
    // points a, b and c
    gradient_real r = (bracket.b - bracket.a) * (fb - fc);
    gradient_real q = (bracket.b - bracket.c) * (fb - fa);
    gradient_real diff = q - r;
    if (fabs(diff) < EPS) {
      diff = EPS; // Prevent division by 0
    }
    gradient_real inflexion = (bracket.b) - ((bracket.b - bracket.c) * q -
                                             (bracket.b - bracket.a) * r) /
                                                (2.0 * diff);

    // Value of function at inflexion point
    gradient_real fInflexion;

    // Set a maximum distance for where we will look for a minimum
    gradient_real ulim = bracket.b + GLIMIT * (bracket.c - bracket.b);

    // Check if u is between b and c
    if ((bracket.b - inflexion) * (inflexion - bracket.c) > 0.0) {

      // Compute function value at u
      fInflexion = func(f, inflexion, initialPoint, direction, n);

      // Check if we have a minimum between b and c
      if (fInflexion < fc) {
        bracket.a = bracket.b;
        bracket.b = inflexion;
        return bracket;
      }

      // Check if we have a minimum between a and u
      else if (fInflexion > fb) {
        bracket.c = inflexion;
        return bracket;
      }

      // Our parabolic interpolation was not useful
      // Magnify u and proceed to the next iteration
      inflexion = (bracket.c) + GOLD * (bracket.c - bracket.b);
      fInflexion = func(f, inflexion, initialPoint, direction, n);
    }

    // Check if u is between c and the limit
    else if ((bracket.c - inflexion) * (inflexion - ulim) > 0.0) {

      // Compute function value at u
      fInflexion = func(f, inflexion, initialPoint, direction, n);

      // Check if we need to continue looking after u
      if (fInflexion < fc) {
        // Magnify u and proceed to the next iteration
        bracket.b = bracket.c;
        bracket.c = inflexion;
        inflexion = bracket.c + GOLD * (bracket.c - bracket.b);

        fb = fc;
        fc = fInflexion;
        fInflexion = func(f, inflexion, initialPoint, direction, n);
      }
    }

    // If inflexion point is further than the limit
    else if ((inflexion - ulim) * (ulim - bracket.c) >= 0.0) {
      inflexion = ulim;
      fInflexion = func(f, inflexion, initialPoint, direction, n);
    }

    // Reject parabolic interpolation
    // Magnify and proceed to next iteration
    else {
      inflexion = (bracket.c) + GOLD * (bracket.c - bracket.b);
      fInflexion = func(f, inflexion, initialPoint, direction, n);
    }

    // Eliminate oldest point and continue
    bracket.a = bracket.b;
    bracket.b = bracket.c;
    bracket.c = inflexion;

    fa = fb;
    fb = fc;
    fc = fInflexion;
  }

  return bracket;
}

/**
 * @brief Applies linear search to find the minimum of a function in a given direction
 * 
 * @param point Initial point to base the movement of. Will contain the minimum after the function executes
 * @param min Return parameter containing the value of the function at the minimum
 * @param direction Direction in which to move
 * @param n Number of dimensions of the original function
 * @param func Function we want to minimize
 * @param tol Tolerance for the minimum estimate. Should be no smaller then the square-root of the machine floating point precision
 * @return Value indicating if brent was a succes or an error
 */
static int lineSearch(gradient_real point[], gradient_real* min,
                                gradient_real direction[], int n, function func,
                                gradient_real tol) {

  // Initially bracket the minimum between 3 points
  Bracket bracket = bracketMinimum(oneDimension, func, point, direction, n);

  // Find the minimum on the line given by the direction vector
  gradient_real xmin = 0.0;
  gradient_real value = 0.0;

  // It is possible for brent to not be able to find the minimum
  // In that case we stop de descent and return an error
  int status = brent(&xmin, &value, bracket, oneDimension, func, point, direction, n, tol);
  if (status == GRADIENT_ERROR) {
    return GRADIENT_ERROR;
  }

  // Move point to the minimum
  for (int i = 0; i < n; i++) {
    point[i] += direction[i] * xmin;
  }

  *min = value;
  return GRADIENT_SUCCESS;
}

/**
 * @brief Applies the conjugate gradient descent method to find the minimum of a provided function
 * 
 * @param func Function to minimize
 * @param dfunc Derivative of the function to minimize
 * @param min Return parameter containing the value of the function at the minimum
 * @param guess Initial guess from which to start the search. Return parameter containing the minimum point
 * @param n Number of dimensions of the function
 * @param tol Tolerance for the estimate of the minimum. Should be no smaller then the square-root of the machine floating point precision
 * @param iterations Maximum number of iterations before stoping the search. Return parameter containing the number of iterations done
 * @return Value indicating if the descent was a succes or an error
 */
int gradient_descent(function func, derivative dfunc, gradient_real* min,
                               gradient_real guess[], int n, gradient_real tol,
                               int* iterations) {
  // Gradient of the function
  gradient_real gradient[n];
  gradient_real nextGradient[n];

  // Conjugate gradient of the function
  gradient_real conjugate[n];

  // Compute initial value of function at the guess point
  gradient_real value = func(guess);

  // Compute gradient of the function at the guess point
  dfunc(guess, gradient);

  // Initialize the gradient and the conjugate
  for (int i = 0; i < n; i++) {
    gradient_real temp = -gradient[i];
    gradient[i] = temp;
    conjugate[i] = temp;
  }

  gradient_real minValue = 0.0;
  for (int its = 0; its < *iterations; its++) {
    // Moves the guess to the minimum along the conjugate gradient direction
    // Obtain the value of the function at that minimum
    int status = lineSearch(guess, &minValue, conjugate, n, func, tol);

    // Check if the line search was successful
    if (status == GRADIENT_ERROR) {
      return GRADIENT_ERROR;
    }

    // Checks if we have reached a minimum
    if (2.0 * fabs(minValue - value) <= tol * (fabs(minValue) + fabs(value) + EPS)) {
      *iterations = its;
      *min = minValue;
      return GRADIENT_SUCCESS;
    }

    // Set the value of the function at the new guess point
    value = minValue;

    // Compute the gradient at the new guess point
    dfunc(guess, nextGradient);

    // Compute the adjustment factor for the new conjugate
    gradient_real dgg = 0.0;
    gradient_real gg = 0.0;
    for (int i = 0; i < n; i++) {
      gradient_real grad = gradient[i];
      gradient_real nextGrad = nextGradient[i];
      gg += grad * grad;
      dgg += (nextGrad + grad) * nextGrad;
    }
    gradient_real adjustement = dgg / (gg + EPS);

    // Set the new conjugate and gradient for the next iteration
    for (int i = 0; i < n; i++) {
      gradient[i] = -nextGradient[i];
      conjugate[i] = gradient[i] + adjustement * conjugate[i];
    }
  }

  // We should not reach here something went wrong
  return GRADIENT_ERROR;
}
