#include "stats.h"
#include "linear_congruential_random_generator.h"

/**
 * @brief Calculates the maximum element of an array
 * @param array array containing the values needed to calculate the maximum
 * @param size  size of the array
*/
real_number max(real_number* array, vec_size size) {

    real_number max = -__DBL_MIN__;
    for (vec_size i = 0; i < size; ++i) {
        if (array[i] > max) {
            max = array[i];        
        }
    }
    return max;
}

/**
 * @brief Calculates the minimum element of an array
 * @param array array containing the values needed to calculate the minimum
 * @param size  size of the array
*/
real_number min(real_number* array, vec_size size) {

    real_number max = __DBL_MAX__;
    for (vec_size i = 0; i < size; ++i) {
        if (array[i] < max) {
            max = array[i];        
        }
    }
    return max;
}

/**
 * @brief Calculates the mean of a given array
 * @param array array containing the values needed to calculate the mean
 * @param size  size of the array
*/
real_number mean(real_number* array, vec_size size) {

    real_number sum = 0.0;
    for (vec_size i = 0; i < size; ++i) {
        sum += array[i];
    }
    return sum / size;
}

/**
 * @brief Computes the variance of a sample 
 * (divides by [size - 1] since the given values are considered to be a sample of the population)
 * @param array array containing the values of the sample
 * @param size  number of samples
 * @return variance of sample
*/
real_number variance(real_number* array, vec_size size) {
    
    real_number avg = mean(array, size);
    real_number var = 0.0;
    real_number adujstingFactor = 0.0;
    for (vec_size i = 0; i < size; ++i) {
        real_number diffElem = array[i] - avg;
        adujstingFactor += diffElem;
        var += diffElem * diffElem;
    } 

    return (var - adujstingFactor * adujstingFactor / size) / (size - 1);
}


/**
 * @brief Computes the standard deviation of a sample 
 * (divides by [size - 1] since the given values are considered to be a sample of the population)
 * @param array array containing the values of the sample
 * @param size  number of samples
 * @return standard deviation of sample
*/
real_number standardDeviation(real_number* array, vec_size size) {
    return sqrt(variance(array, size));
}

/**
 * @brief Computes the absolute average deviation of a sample 
 * (divides by [size - 1] since the given values are considered to be a sample of the population)
 * @param array array containing the values of the sample
 * @param size  number of samples
 * @return absolute average deviation of sample
*/
real_number absAverageDeviation(real_number* array, vec_size size) {
    real_number avg = mean(array, size);
    real_number absAverageDeviation = 0.0;
    for (vec_size i = 0; i < size; ++i) {
        real_number elem = array[i];
        real_number diffElem = elem - avg;
        absAverageDeviation += fabs(diffElem);
    }

    return absAverageDeviation / (size - 1);
}

/**
 * @brief Computes the absolute average deviation of a sample 
 * (divides by [size - 1] since the given values are considered to be a sample of the population)
 * @param array array containing the values of the sample
 * @param size  number of samples
 * @return absolute average deviation of sample
*/
real_number covariance(real_number* x, real_number* y, vec_size size) {
    real_number avgX = mean(x, size);
    real_number avgY = mean(y, size);

    real_number sum = 0.0;
    for (vec_size i = 0; i < size; ++i) {
        sum += (x[i] - avgX) * (y[i] - avgY);
    }

    return sum / (size - 1);
}

/**
 * @brief Makes a statistical analysis of an array of numbers
 * @param array     array containing the data
 * @param size      size of the array
 * @param sample    whether to consider the data as a sample or as the population when computing the metrics (0 = Population, 1 = Sample)
 * @return Returns the error code of the function (0 = Success, 1 = Size of data is less than 2, can't compute metrics, 2 = Variance is null, can't compute skewness and kurtosis)
*/
vec_size analyzeData(real_number* array, vec_size size, DataSummary* moments) {
    
    if (size < 2) return 1;

    real_number avg = mean(array, size);
    real_number max = array[0], min = array[0], absAverageDeviation = 0.0, var = 0.0, skew = 0.0, kurtosis = 0.0;
    real_number adujstingFactor = 0.0;
    for (vec_size i = 0; i < size; ++i) {
        real_number elem = array[i];

        if (max < elem)
            max = elem;
        if (min > elem)
            min = elem;
        
        real_number diffElem = elem - avg;
        real_number diffElemSquared = diffElem * diffElem;
        absAverageDeviation += fabs(diffElem);
        adujstingFactor += diffElem;
        var += diffElemSquared;
        skew += diffElemSquared * diffElem;
        kurtosis += diffElemSquared * diffElemSquared;
    }

    moments->min = min;
    moments->max = max;
    moments->mean = avg;


    real_number dividingFactor = 1.0 / (size - 1);;
    
    moments->absAverageDeviation = absAverageDeviation * dividingFactor;
    moments->variance = (var - adujstingFactor * adujstingFactor / size) * dividingFactor;
    moments->standardDeviation = sqrt(moments->variance);

    if (fabs(moments->variance) < EPSILON) {
        return 2; // Cannot compute skewness and kurtosis if variance is 0
    }

    real_number skewnessFactor = ((real_number) size) / ((size - 1) * (size - 2));
    
    moments->skewness = skewnessFactor * skew / (moments->variance * moments->standardDeviation);
    
    real_number kurtosisFactor = (skewnessFactor * (size + 1)) / (size - 3);;
    real_number kurtosisFactorAdjustment = 3.0 * ((real_number)((size - 1) * (size - 1))) / ((size - 2) * (size - 3));
    moments->kurtosis = (kurtosisFactor * kurtosis / (moments->variance * moments->variance)) - kurtosisFactorAdjustment; // Formula of kurtosis in Numerical Recipes

    return 0;
}

/**
 * @brief Fits a straight line into 2d data (form is y = ax + b) 
 * @param x x values
 * @param y y values
 * @param size  number of data points
 * @param a     slope of the line to fit
 * @param b     y-intercept of the line to fit
*/
void simpleLinearRegression(real_number* x, real_number* y, vec_size size, real_number* a, real_number* b) {
    
    real_number sxx = 0.0; // sum of x squared
    real_number sxy = 0.0; // sum of x * y
    real_number sx = 0.0; // sum of x
    real_number sy = 0.0; // sum of y

    for (vec_size i = 0; i < size; ++i) {
        real_number currentX = x[i]; 
        real_number currentY = y[i]; 
        sxx += currentX * currentX;
        sxy += currentX * currentY;
        sx += currentX;
        sy += currentY;
    }

    real_number denominator = 1.0 / (size * sxx - sx * sx);
    *a = denominator * (size * sxy - sx * sy);
    *b = denominator * (sy * sxx - sx * sxy);
}


// Finds the euclidian distance between two points
static real_number distance(real_number* p1,  real_number* p2, vec_size size) {

    real_number currentDistance = 0.0;
    for (vec_size i = 0; i < size; ++i) {
        real_number dataP1 = p1[i];
        real_number dataP2 = p2[i];
        real_number subP1P2 = dataP1 - dataP2;
        currentDistance += subP1P2 * subP1P2;
    }

    return sqrt(currentDistance);
}

// Finds closest point to data
static vec_size closest(real_number* data, vec_size dimensions, real_number* points, vec_size nbPoints) {

    real_number maxDistance = __FLT_MAX__;
    vec_size maxIndex = 0;
    for (vec_size i = 0; i < nbPoints; ++i) {
        real_number currentDistance = distance(data, &points[i * dimensions], dimensions);
        if (currentDistance < maxDistance) {
            maxDistance = currentDistance;
            maxIndex = i;
        }
    }

    return maxIndex;
}

void kmeans(real_number* data, vec_size size, vec_size dimensions, vec_size nbClusters, real_number* centroids, vec_size* assignations) {
    // centroids is expected to be an array of size : nbClusters * dimensions
    // data is expected to be an array of size : size * dimensions
    // assignations is expected to be an array of size : size

    // Initialize centroids to random positions
    real_number maxPoint = max(data, size * dimensions);
    real_number minPoint = min(data, size * dimensions);
    real_number range = (maxPoint - minPoint);  
    for (vec_size i = 0; i < nbClusters; ++i) {
        // vec_size initCentroid = (vec_size) fmax(0, (randomNumber * size) - 1);
        for (vec_size j = 0; j < dimensions; ++j) {
            real_number randomNumber = (linear_congruential_random_generator() * range) + minPoint;        
            centroids[i * dimensions + j] = randomNumber;
        }
    }

    // Array containing the number of points in each cluster (needed for optimization)
    vec_size clustersSize[nbClusters];

    for (vec_size i = 0; i < size; ++i) {
        // We initialize every point to the first cluster
        assignations[i] = 0;
    }

    clustersSize[0] = size;
    bool clustersChanged = false;
    
    // As long as groups have not changed
    while (!clustersChanged) {

        // Assign group to every point
        for (vec_size i = 0; i < size; ++i) {
            vec_size cluster = closest(&data[i * dimensions], dimensions, centroids, nbClusters);
            
            if (assignations[i] != cluster) {
                clustersChanged = true;
            }

            vec_size oldCluster = assignations[i];
            assignations[i] = cluster;

            clustersSize[cluster]++;
            clustersSize[oldCluster]--;
        }

        real_number newCentroids[nbClusters * dimensions];
        for (vec_size i = 0; i < size; ++i) {
            vec_size clusterAssignation = assignations[i];

            for (vec_size dim = 0; dim < dimensions; ++dim) {
                newCentroids[clusterAssignation + dim] += data[i * dimensions + dim];
            }
        }

        // Calculate new centroids
        for (vec_size i = 0; i < nbClusters; ++i) {

            // If no points were assigned to this cluster, do not recalculate the mean
            if (clustersSize[i] != 0) {
                for (vec_size dim = 0; dim < dimensions; ++dim) {
                    centroids[i * dimensions + dim] = newCentroids[i * dimensions + dim] / clustersSize[i];
                }
            }
        }

    }

}
