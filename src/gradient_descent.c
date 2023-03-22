#include <gradient_descent.h>

double linmin(double point[], double gradient[], int n, function func);
double brent(MinBracket bracket, double (*func)(double x, struct f1DimParam), double tol, double* xmin, struct f1DimParam param);
MinBracket mnbrak(MinBracket bracket, double (*func)(double x, struct f1DimParam), struct f1DimParam param);
double f1dim(double x, struct f1DimParam param);

// TODO - Calculate finite difference with 2 orders and let user specify it
// TODO - Give option of 1st order or second order (if not in loop)

double gradient_descent(function func, derivative dfunc, double guess[], int n) {
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

    for (int its = 0; its <= ITMAX; its++) { // TODO - ITMAX parameter
        // Moves the guess to the minimum along the conjugate gradient direction
        // Obtain the value of the function at that minimum
        double min = linmin(guess, conjugate, n, func);

        // Checks if we have reached a minimum
        if (2.0*fabs(min - value) <= TOL*(fabs(min)+fabs(value)+EPS)) { // TODO - Tolerance defined by user EPS
            return min;
        }

        // Set the value of the function at the new guess point
        value = min;

        // Compute the gradient at the new guess point
        (*dfunc)(guess,nextGradient);

        // Compute the adjustment factor gamma for the new conjugate
        double dgg = 0.0;
        double gg = 0.0;
        for (int i = 0; i < n; i++) {
            gg += gradient[i]*gradient[i];
            dgg += (nextGradient[i]+gradient[i])*nextGradient[i];
        }
        double gamma = dgg / (gg + EPS); // Add small epsilon to prevent division by 0

        // Set the new conjugate and gradient for the next iteration
        for (int i = 0; i < n; i++) {
            gradient[i] = -nextGradient[i];
            conjugate[i] = gradient[i] + gamma * conjugate[i];
        }
    }
    
    // We should not reach here something went wrong
    return 0.0;
}

double linmin(double point[], double direction[], int n, function func) {
    double xmin,ret;

    struct f1DimParam param;
    param.ncom = n;
    param.xicom = malloc(n * sizeof(double));
    param.pcom = malloc(n * sizeof(double));
    param.nrfunc = func;
    for (int i = 0; i <= n; i++) {
        param.pcom[i] = point[i];
        param.xicom[i] = direction[i];
    }

    MinBracket bracket = {0.0, 1.0, 0.0};
    bracket = mnbrak(bracket, f1dim, param);
    ret = brent(bracket, f1dim, TOL, &xmin, param);

    for (int i = 0; i < n; i++) {
        point[i] += direction[i] * xmin;
    }

    free(param.xicom);
    free(param.pcom);

    return ret;
}

double brent(MinBracket bracket, double (*func)(double x, struct f1DimParam), double tol, double* xmin, struct f1DimParam param) {
    int iter;
    double a,b,d,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
    double e=0.0;

    a=(bracket.a < bracket.c ? bracket.a : bracket.c);
    b=(bracket.a > bracket.c ? bracket.a : bracket.c);
    x=w=v=bracket.b;
    fw=fv=fx=(*func)(x, param);

    for (iter=1;iter<=ITMAX;iter++) {
        xm=0.5*(a+b);
        tol2=2.0*(tol1=tol*fabs(x)+ZEPS);

        if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
            *xmin=x;
            return fx;
        }

        if (fabs(e) > tol1) {
            r=(x-w)*(fx-fv);
            q=(x-v)*(fx-fw);
            p=(x-v)*q-(x-w)*r;
            q=2.0*(q-r);
            if (q > 0.0) p = -p;
            q=fabs(q);
            etemp=e;
            e=d;
            if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x)) {
                d=CGOLD*(e=(x >= xm ? a-x : b-x));
            } 
            else {
                d=p/q;
                u=x+d;
                if (u-a < tol2 || b-u < tol2)
                    d=SIGN(tol1,xm-x);
            }
        }
        else {
            d=CGOLD*(e=(x >= xm ? a-x : b-x));
        }

        u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
        fu=(*func)(u, param);

        if (fu <= fx) {
            if (u >= x) a=x; else b=x;
            SHFT(v,w,x,u);
            SHFT(fv,fw,fx,fu);
        }
        else {
            if (u < x) a=u; else b=u;
            if (fu <= fw || w == x) {
                v=w;
                w=u;
                fv=fw;
                fw=fu;
            } else if (fu <= fv || v == x || v == w) {
                v=u;
                fv=fu;
            }
        }
    }
    // We should not reach this point to many iterations
    *xmin=x;
    return fx;
}

// Brackets the minimum between 3 points (ax, bx, cx)
// ax and bx are the initial points
MinBracket mnbrak(MinBracket bracket, double (*func)(double x, struct f1DimParam), struct f1DimParam param) {
    double ulim, u, r, q, fu, dum, fa, fb, fc;

    // Evaluate function at initial points a and b
    fa = (*func)(bracket.a, param);
    fb = (*func)(bracket.b, param);

    // We want to go downhill from a to b to bracket the minimum 
    // so if fb > fa we need to switch them
    if (fb > fa) {
        SHFT(dum, bracket.a, bracket.b, dum);
        SHFT(dum, fb, fa, dum);
    }

    // Initial guess for c
    bracket.c = (bracket.b) + GOLD * (bracket.b - bracket.a);
    fc = (*func)(bracket.c, param);

    // Loop until we bracket the minimum 
    while (fb > fc) {
        // Parabolic interpolation for u
        r = (bracket.b - bracket.a) * (fb - fc);
        q = (bracket.b - bracket.c) * (fb - fa);
        // TODO - find simpler way to prevent division by 0
        u = (bracket.b) - ((bracket.b - bracket.c) * q - (bracket.b - bracket.a) * r) / (2.0 * SIGN(FMAX(fabs(q - r), TINY), q - r)); //TODO replace TINY by epsilon

        ulim = (bracket.b) + GLIMIT * (bracket.c - bracket.b);
        if ((bracket.b - u) * (u - bracket.c) > 0.0) {
            fu = (*func)(u, param);
            if (fu < fc) {
                bracket.a = (bracket.b);
                bracket.b = u;
                fa = (fb);
                fb = fu;
                return bracket;
            }
            else if (fu > fb) {
                bracket.c = u;
                fc = fu;
                return bracket;
            }
            u = (bracket.c) + GOLD * (bracket.c - bracket.b);
            fu = (*func)(u, param); 
        }
        else if ((bracket.c - u) * (u - ulim) > 0.0) {
            fu=(*func)(u, param);
            if (fu < fc) {
                SHFT(bracket.b, bracket.c, u, bracket.c + GOLD * (bracket.c - bracket.b));
                SHFT(fb, fc, fu, (*func)(u, param));
            }
        }
        else if ((u - ulim) * (ulim - bracket.c) >= 0.0) {
            u = ulim;
            fu = (*func)(u, param);
        }
        else {
            u = (bracket.c) + GOLD * (bracket.c - bracket.b);
            fu = (*func)(u, param);
        }

        SHFT(bracket.a, bracket.b, bracket.c, u);
        SHFT(fa, fb, fc, fu);
    }

    return bracket;
}

double f1dim(double x, struct f1DimParam param) {
    double* xt = malloc(param.ncom * sizeof(double));

    for (int i = 0; i < param.ncom; i++) {
        xt[i] = param.pcom[i] + x * param.xicom[i];
    }

    double f = (*(param.nrfunc))(xt);

    free(xt);
    
    return f;
}