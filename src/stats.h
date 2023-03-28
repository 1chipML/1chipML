#ifndef _STATS_LIB_
#define _STATS_LIB_

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef EPISLON
    #define EPSILON 0.01
#endif

#ifndef REAL_NUMBER
    #define REAL_NUMBER double
#endif

#ifndef INTEGER
    #define INTEGER unsigned int
#endif

typedef REAL_NUMBER real_number;
typedef INTEGER vec_size;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    real_number max;
    real_number min;
    real_number mean;
    real_number absAverageDeviation;
    real_number standardDeviation;
    real_number variance;
    real_number skewness;
    real_number kurtosis; // The formula for the excess kurtosis was used
} DataSummary;

// Simple analyze of data
real_number max(real_number* array, vec_size size);
real_number min(real_number* array, vec_size size);
real_number mean(real_number* array, vec_size size);
real_number varianceSample(real_number* array, vec_size size);
real_number variancePopulation(real_number* array, vec_size size);
real_number standardDeviationSample(real_number* array, vec_size size);
real_number standardDeviationPopulation(real_number* array, vec_size size);
real_number absAverageDeviationSample(real_number* array, vec_size size);
real_number absAverageDeviationPopulation(real_number* array, vec_size size);
real_number covarianceSample(real_number* x, real_number* y, vec_size size);
real_number covariancePopulation(real_number* x, real_number* y, vec_size size);
vec_size analyzeData(real_number* array, vec_size size, DataSummary* moments, vec_size sample);

// Fitting curves
void simpleLinearRegression(real_number* x, real_number* y, vec_size size, real_number* a, real_number* b);

// Clustering
void kmeans(real_number* data, vec_size size, vec_size dimensions, vec_size nbClusters, real_number* centroids, vec_size* assignations);
vec_size closest(real_number* data, vec_size dimensions, real_number* points, vec_size nbPoints);

#ifdef __cplusplus
}
#endif

#endif
