#include "stats.h"

/**
 * @brief Calculates the maximum element of an array
 * @param array array containing the values needed to calculate the maximum
 * @param size  size of the array
 */
real_number maxOfArray(real_number* array, vec_size size) {

  real_number max = MIN_REAL_NUMBER;
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
real_number minOfArray(real_number* array, vec_size size) {

  real_number min = MAX_REAL_NUMBER;
  for (vec_size i = 0; i < size; ++i) {
    if (array[i] < min) {
      min = array[i];
    }
  }
  return min;
}

/**
 * @brief Calculates the mean of a given array
 * @param data  array containing the values needed to calculate the mean
 * @param size  size of the array
 */
real_number mean(real_number* data, vec_size size) {

  real_number sum = 0.0;
  for (vec_size i = 0; i < size; ++i) {
    sum += data[i];
  }
  return sum / size;
}

/**
 * @brief Computes the variance of a sample
 * (divides by [size - 1] since the given values are considered to be a sample
 * of the population)
 * @param data  array containing the values of the sample
 * @param size  number of samples
 * @return      variance of sample
 */
real_number variance(real_number* data, vec_size size) {

  real_number avg = mean(data, size);
  real_number var = 0.0;
  real_number adujstingFactor = 0.0;
  for (vec_size i = 0; i < size; ++i) {
    real_number diffElem = data[i] - avg;
    adujstingFactor += diffElem;
    var += diffElem * diffElem;
  }

  return (var - adujstingFactor * adujstingFactor / size) / (size - 1);
}

/**
 * @brief Computes the standard deviation of a sample
 * (divides by [size - 1] since the given values are considered to be a sample
 * of the population)
 * @param array array containing the values of the sample
 * @param size  number of samples
 * @return standard deviation of sample
 */
real_number standardDeviation(real_number* array, vec_size size) {
  return sqrt(variance(array, size));
}

/**
 * @brief Computes the absolute average deviation of a sample
 * (divides by [size - 1] since the given values are considered to be a sample
 * of the population)
 * @param array array containing the values of the sample
 * @param size  number of samples
 * @return absolute average deviation of sample
 */
real_number absAverageDeviation(real_number* array, vec_size size) {
  real_number avg = mean(array, size);
  real_number result = 0.0;
  for (vec_size i = 0; i < size; ++i) {
    real_number elem = array[i];
    real_number diffElem = elem - avg;
    result += fabs(diffElem);
  }

  return result / (size - 1);
}

/**
 * @brief Computes the absolute average deviation of a sample
 * (divides by [size - 1] since the given values are considered to be a sample
 * of the population)
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
 * @param sample    whether to consider the data as a sample or as the
 * population when computing the metrics (0 = Population, 1 = Sample)
 * @return Returns the error code of the function (0 = Success, 1 = Size of data
 * is less than 2, 2 = Variance is null, can't compute skewness and kurtosis,
 * but other metrics are computed)
 */
vec_size analyzeData(real_number* array, vec_size size, DataSummary* moments) {

  if (size < 2)
    return 1;

  real_number avg = mean(array, size);
  real_number max = array[0], min = array[0], absAverageDeviation = 0.0,
              var = 0.0, skew = 0.0, kurtosis = 0.0;
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

  real_number dividingFactor = 1.0 / (size - 1);
  ;

  moments->absAverageDeviation = absAverageDeviation * dividingFactor;
  moments->variance =
      (var - adujstingFactor * adujstingFactor / size) * dividingFactor;
  moments->standardDeviation = sqrt(moments->variance);

  if (fabs(moments->variance) < EPSILON) {
    return 2; // Cannot compute skewness and kurtosis if variance is 0
  }

  real_number skewnessFactor = ((real_number)size) / ((size - 1) * (size - 2));

  moments->skewness =
      skewnessFactor * skew / (moments->variance * moments->standardDeviation);

  real_number kurtosisFactor = (skewnessFactor * (size + 1)) / (size - 3);
  ;
  real_number kurtosisFactorAdjustment =
      3.0 * ((real_number)((size - 1) * (size - 1))) /
      ((size - 2) * (size - 3));
  moments->kurtosis =
      (kurtosisFactor * kurtosis / (moments->variance * moments->variance)) -
      kurtosisFactorAdjustment;

  return 0;
}

/**
 * @brief Fits a straight line into 2d data (form is y = ax + b)
 * @param x     x values
 * @param y     y values
 * @param size  number of data points
 * @param a     slope of the line to fit
 * @param b     y-intercept of the line to fit
 */
void simpleLinearRegression(real_number* x, real_number* y, vec_size size,
                            real_number* a, real_number* b) {

  real_number sxx = 0.0; // sum of x squared
  real_number sxy = 0.0; // sum of x * y
  real_number sx = 0.0;  // sum of x
  real_number sy = 0.0;  // sum of y

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

/**
 * @brief Finds the euclidiant distance between two points
 *
 * @param p1    first point
 * @param p2    second point
 * @param size  dimensions of the points
 * @return euclidian distance
 */
static real_number distance(real_number* p1, real_number* p2, vec_size size) {

  real_number currentDistance = 0.0;
  for (vec_size i = 0; i < size; ++i) {
    real_number dataP1 = p1[i];
    real_number dataP2 = p2[i];
    real_number subP1P2 = dataP1 - dataP2;
    currentDistance += subP1P2 * subP1P2;
  }

  return sqrt(currentDistance);
}

/**
 * @brief Given a point 'P' and an array of points, finds the closest point of
 * the array to the point 'P' using the euclidiant distance
 * @param point         point 'P'
 * @param dimensions    number of dimensions of the points
 * @param data          array of points
 * @param nbPoints      number of points in the array
 * @return the index of the closest point in the array to 'P'
 */
static vec_size closest(real_number* point, vec_size dimensions, real_number* data,
                 vec_size nbPoints) {

  real_number minDistance = MAX_REAL_NUMBER;
  vec_size minIndex = 0;
  for (vec_size i = 0; i < nbPoints; ++i) {
    real_number currentDistance =
        distance(point, &data[i * dimensions], dimensions);
    if (currentDistance < minDistance) {
      minDistance = currentDistance;
      minIndex = i;
    }
  }

  return minIndex;
}

/**
 * @brief Runs the K-Means clustering algorithm on a set of data points.
 * @warning The euclidian distance is used to compute the distance between the
 * points
 * @param data          data to cluster (the length must be [size] *
 * [dimensions])
 * @param size          number of data points
 * @param dimensions    dimensions of the data points
 * @param nbClusters    number of clusters to group the data points
 * @param centroids     array where the coordinates of the centroids of clusters
 * will be stored (the length must be [nbCluster] * [dimensions])
 * @param assignations  array containing the index of the cluster to which a
 * point was assigned to (the length must be [size])
 */
void kmeans(real_number* data, vec_size size, vec_size dimensions,
            vec_size nbClusters, real_number* centroids,
            vec_size* assignations) {

  // Initialize centroids to random positions
  {
#define min(a, b) ((a) < (b) ? (a) : (b))

    for (vec_size i = 0; i < nbClusters; ++i) {
      vec_size randomPointIndex =
          min(size - 1, (linear_congruential_random_generator() * size));
      for (vec_size dim = 0; dim < dimensions; ++dim) {
        centroids[i * dimensions + dim] =
            data[randomPointIndex * dimensions + dim];
      }
    }
  }

  // We initialize every point to the first cluster
  memset(assignations, 0, sizeof(vec_size) * size);

  // Array containing the number of points in each cluster (needed for
  // optimization)
  vec_size clustersSize[nbClusters];
  memset(clustersSize, 0, sizeof(vec_size) * nbClusters);
  clustersSize[0] = size;

  bool clustersHaveChanged = true;

  // As long as clusters get updated
  while (clustersHaveChanged) {
    clustersHaveChanged = false;

    // Assign group to every point
    for (vec_size i = 0; i < size; ++i) {
      vec_size cluster =
          closest(&data[i * dimensions], dimensions, centroids, nbClusters);
      vec_size oldCluster = assignations[i];

      if (oldCluster != cluster) {
        clustersHaveChanged = true;
      }

      assignations[i] = cluster;

      // Update the cluster size
      clustersSize[cluster]++;
      clustersSize[oldCluster]--;
    }

    // Prevents unecessary computations if clusters have not changed
    if (!clustersHaveChanged)
      break;

    // Calculate new centroids
    real_number newCentroids[nbClusters * dimensions];
    memset(newCentroids, 0, sizeof(real_number) * nbClusters * dimensions);

    for (vec_size i = 0; i < size; ++i) {
      vec_size clusterAssignation = assignations[i];

      for (vec_size dim = 0; dim < dimensions; ++dim) {
        newCentroids[clusterAssignation * dimensions + dim] +=
            data[i * dimensions + dim];
      }
    }

    // Divide by cluster size to get the average of every positions
    for (vec_size i = 0; i < nbClusters; ++i) {

      // If no points are assigned to this cluster, do not recalculate the mean
      if (clustersSize[i] != 0) {
        for (vec_size dim = 0; dim < dimensions; ++dim) {
          centroids[i * dimensions + dim] =
              newCentroids[i * dimensions + dim] / clustersSize[i];
        }
      }
    }
  }
}
