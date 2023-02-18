#include <gradient_descent.h>

double lineSearch(double point[], double gradient[], int n, function func, double tol);
double brent(double* xmin, Bracket bracket, f1dimension func, function f, double initialPoint[], double direction[], int n, double tol);
Bracket bracketMinimum(f1dimension func, function f, double initialPoint[], double direction[], int n);
double oneDimension(function func, double x, double initialPoint[], double direction[], int n);

// TODO - Calculate finite difference with 2 orders and let user specify it
// TODO - Give option of 1st order or second order (if not in loop)
// TODO - ITMAX parameter

double gradient_descent(function func, derivative dfunc, double guess[], int n, double tol) {
    // Gradient of the function
    double *gradient = malloc(n * sizeof(double));
    double *nextGradient = malloc(n * sizeof(double));

    // Conjugate gradient of the function
    double *conjugate = malloc(n * sizeof(double));

    // Compute initial value of function at the guess point
    double value = (*func)(guess);

    // Compute gradient of the function at the guess point
    (*dfunc)(guess, gradient);

    // Initialize the gradient and the conjugate
    double temp;
    for (int i = 0; i < n; i++) {
        temp = -gradient[i];
        gradient[i] = temp;
        conjugate[i] = temp;
    }

    for (int its = 0; its < ITMAX; its++) {
        // Moves the guess to the minimum along the conjugate gradient direction
        // Obtain the value of the function at that minimum
        double min = lineSearch(guess, conjugate, n, func, tol);

        // Checks if we have reached a minimum
        if (2.0*fabs(min - value) <= tol*(fabs(min)+fabs(value)+EPS)) {
            return min;
        }

        // Set the value of the function at the new guess point
        value = min;

        // Compute the gradient at the new guess point
        (*dfunc)(guess,nextGradient);

        // Compute the adjustment factor for the new conjugate
        double dgg = 0.0;
        double gg = 0.0;
        for (int i = 0; i < n; i++) {
            gg += gradient[i]*gradient[i];
            dgg += (nextGradient[i]+gradient[i])*nextGradient[i];
        }
        if (gg == 0.0) {
            gg = EPS;  // Prevent division by 0
        }
        double adjustement = dgg / gg;

        // Set the new conjugate and gradient for the next iteration
        for (int i = 0; i < n; i++) {
            gradient[i] = -nextGradient[i];
            conjugate[i] = gradient[i] + adjustement * conjugate[i];
        }
    }
    
    // We should not reach here something went wrong
    return 0.0;
}

double lineSearch(double point[], double direction[], int n, function func, double tol) {

    // Initially bracket the minimum between 3 points
    Bracket bracket = bracketMinimum(oneDimension, func, point, direction, n);

    // Find the minimum on the line given by the direction vector
    double xmin = 0.0;
    double value = brent(&xmin, bracket, oneDimension, func, point, direction, n, tol);

    // Move point to the minimum
    for (int i = 0; i < n; i++) {
        point[i] += direction[i] * xmin;
    }

    return value;
}

// Determines the minimum from an initial bracket
// Uses a mix of parabolic interpolation and bisection
double brent(double* xmin, Bracket bracket, f1dimension func, function f, double initialPoint[], double direction[], int n, double tol) {
    double d,etemp,xm;
    double e = 0.0;

    double a = (bracket.a < bracket.c ? bracket.a : bracket.c);
    double b = (bracket.a > bracket.c ? bracket.a : bracket.c);

    double x, v, w;
    x = v = w = bracket.b;

    double fx, fv, fw;
    fx = fv = fw = (*func)(f, x, initialPoint, direction, n);

    for (int iter = 0 ; iter < ITMAX; iter++) {
        xm = 0.5 * (a + b);
        double tol1 = tol * fabs(x) + EPS;
        double tol2 = 2.0 * tol1;
        
        // Checks if we found the minimum with enough precision
        if (fabs(x - xm) <= (tol2 - 0.5 * (b - a))) {
            *xmin = x;
            return fx;
        }

        if (fabs(e) > tol1) {
            // Construct trial parabolic fit
            double r = (x - w) * (fx - fv);
            double q = (x - v) * (fx - fw);
            double p = (x - v) * q - (x - w) * r;
            q = 2.0 * (q - r);
            
            if (q > 0.0) {
                p = -p;
            }

            q = fabs(q);

            etemp = e;
            e = d;
            if (fabs(p) >= fabs(0.5 * q * etemp) || p <= q * (a - x) || p >= q * (b - x)) {
                e = x >= xm ? a-x : b-x;
                d = CGOLD * e;
            } 
            else {
                d = p / q;
                double u = x + d;
                if (u - a < tol2 || b - u < tol2)
                    d = SIGN(tol1,xm - x);
            }
        }
        else {
            e = x >= xm ? a - x : b - x;
            d = CGOLD * e;
        }

        double u = fabs(d) >= tol1 ? x + d : x + SIGN(tol1,d);
        double fu = (*func)(f, u, initialPoint, direction, n);

        if (fu <= fx) {
            if (u >= x) {
                a = x;
            } 
            else {
                b = x;
            }

            v = w;
            w = x;
            x = u;

            fv = fw;
            fw = fx;
            fx = fu;
        }
        else {
            if (u < x) {
                a=u;
            } 
            else {
                b=u;
            }
            if (fu <= fw || w == x) {
                v = w;
                w = u;
                fv = fw;
                fw = fu;
            } else if (fu <= fv || v == x || v == w) {
                v = u;
                fv = fu;
            }
        }
    }

    // We should not reach this point to many iterations
    *xmin=x;
    return fx;
}

// Initially brackets the minimum between 3 points (a, b, c)
// Uses parabolic interpolation to find the bracket
Bracket bracketMinimum(f1dimension func, function f, double initialPoint[], double direction[], int n) {
    // Set a, b and c at arbitrary initial values
    Bracket bracket = {0.0, 1.0, 0.0};

    // Evaluate function at initial points a and b
    double fa = (*func)(f, bracket.a, initialPoint, direction, n);
    double fb = (*func)(f, bracket.b, initialPoint, direction, n);

    // We want to go downhill from a to b to bracket the minimum 
    // so if fb > fa we need to swap them
    if (fb > fa) {
        double temp = bracket.a;
        bracket.a = bracket.b;
        bracket.b = temp;

        temp = fa;
        fa = fb;
        fb = fa;
    }

    // Initial guess for c
    bracket.c = (bracket.b) + GOLD * (bracket.b - bracket.a);
    double fc = (*func)(f, bracket.c, initialPoint, direction, n);

    // Loop until we bracket the minimum 
    while (fb > fc) {
        // Compute u the inflexion point of the parabola defined by 
        // points a, b and c
        double r = (bracket.b - bracket.a) * (fb - fc);
        double q = (bracket.b - bracket.c) * (fb - fa);
        double diff = q - r;
        if (diff == 0) {
            diff = EPS; // Prevent division by 0
        }
        double inflexion = (bracket.b) - ((bracket.b - bracket.c) * q - (bracket.b - bracket.a) * r) / (2.0 * diff);
            
        // Value of function at inflexion point
        double fInflexion;

        // Set a maximum distance for where we will look for a minimum
        double ulim = (bracket.b) + GLIMIT * (bracket.c - bracket.b);

        // Check if u is between b and c
        if ((bracket.b - inflexion) * (inflexion - bracket.c) > 0.0) {

            // Compute function value at u
            fInflexion = (*func)(f, inflexion, initialPoint, direction, n);

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
            fInflexion = (*func)(f, inflexion, initialPoint, direction, n); 
        }

        // Check if u is between c and the limit
        else if ((bracket.c - inflexion) * (inflexion - ulim) > 0.0) {

            // Compute function value at u
            fInflexion=(*func)(f, inflexion, initialPoint, direction, n);

            // Check if we need to continue looking after u
            if (fInflexion < fc) {
                // Magnify u and proceed to the next iteration
                bracket.b = bracket.c;
                bracket.c = inflexion;
                inflexion = bracket.c + GOLD * (bracket.c - bracket.b);
                
                fb = fc;
                fc = fInflexion;
                fInflexion = (*func)(f, inflexion, initialPoint, direction, n);
            }
        }

        // If inflexion point is further than the limit
        else if ((inflexion - ulim) * (ulim - bracket.c) >= 0.0) {
            inflexion = ulim;
            fInflexion = (*func)(f, inflexion, initialPoint, direction, n);
        }

        // Reject parabolic interpolation
        // Magnify and proceed to next iteration
        else {
            inflexion = (bracket.c) + GOLD * (bracket.c - bracket.b);
            fInflexion = (*func)(f, inflexion, initialPoint, direction, n);
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

// Transforms function into a one dimentsion function from the initialPoint along the given direction
double oneDimension(function func, double x, double initialPoint[], double direction[], int n) {
    double* point = malloc(n * sizeof(double));

    // Compute new point when moving by x in the given direction
    for (int i = 0; i < n; i++) {
        point[i] = initialPoint[i] + x * direction[i];
    }

    // Compute function value at new point
    double value = (*func)(point);

    free(point);
    
    return value;
}