#include <gradient_descent.h>

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
 * @param bracket Initial 3 points bracketing the minimum
 * @param func Function transforming an N dimension function to a 1 dimension function
 * @param f Function we want to transfor to a 1 dimension function
 * @param initialPoint Initial point to base the movement of
 * @param direction Direction in which to move
 * @param n Number of dimensions of the original function
 * @param tol Tolerance for the minimum estimate. Should be no smaller then the square-root of the machine floating point precision
 * @return Value of the original function at the minimum found
 */ 
static gradient_real brent(gradient_real *xMin, Bracket bracket,
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
  gradient_real b = fax(bracket.a, bracket.c);

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
      return fx;
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
  *xMin = x;
  return fx;
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
    fb = fa;
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
    if (diff == 0) {
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
 * @param direction Direction in which to move
 * @param n Number of dimensions of the original function
 * @param func Function we want to minimize
 * @param tol Tolerance for the minimum estimate. Should be no smaller then the square-root of the machine floating point precision
 * @return Value of the original function at the minimum found
 */
static gradient_real lineSearch(gradient_real point[],
                                gradient_real direction[], int n, function func,
                                gradient_real tol) {

  // Initially bracket the minimum between 3 points
  Bracket bracket = bracketMinimum(oneDimension, func, point, direction, n);

  // Find the minimum on the line given by the direction vector
  gradient_real xmin = 0.0;
  gradient_real value =
      brent(&xmin, bracket, oneDimension, func, point, direction, n, tol);

  // Move point to the minimum
  for (int i = 0; i < n; i++) {
    point[i] += direction[i] * xmin;
  }

  return value;
}

/**
 * @brief Applies the conjugate gradient descent method to find the minimum of a provided function
 * 
 * @param func Function to minimize
 * @param dfunc Derivative of the function to minimize
 * @param guess Initial guess from which to start the search
 * @param n Number of dimensions of the function
 * @param tol Tolerance for the estimate of the minimum. Should be no smaller then the square-root of the machine floating point precision
 * @param itMax Maximum number of iterations before stoping the search
 * @return Value of the function at the minimum
 */
gradient_real gradient_descent(function func, derivative dfunc,
                               gradient_real guess[], int n, gradient_real tol,
                               int itMax) {
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

  gradient_real min = 0.0;
  for (int its = 0; its < itMax; its++) {
    // Moves the guess to the minimum along the conjugate gradient direction
    // Obtain the value of the function at that minimum
    min = lineSearch(guess, conjugate, n, func, tol);

    // Checks if we have reached a minimum
    if (2.0 * fabs(min - value) <= tol * (fabs(min) + fabs(value) + EPS)) {
      return min;
    }

    // Set the value of the function at the new guess point
    value = min;

    // Compute the gradient at the new guess point
    dfunc(guess, nextGradient);

    // Compute the adjustment factor for the new conjugate
    gradient_real dgg = 0.0;
    gradient_real gg = 0.0;
    for (int i = 0; i < n; i++) {
      gg += gradient[i] * gradient[i];
      dgg += (nextGradient[i] + gradient[i]) * nextGradient[i];
    }
    gradient_real adjustement = dgg / (gg + EPS);

    // Set the new conjugate and gradient for the next iteration
    for (int i = 0; i < n; i++) {
      gradient[i] = -nextGradient[i];
      conjugate[i] = gradient[i] + adjustement * conjugate[i];
    }
  }

  // We should not reach here something went wrong
  return min;
}