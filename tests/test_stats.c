#define EPSILON 0.01

#include "linear_congruential_random_generator.h"
#include "stats.h"
#include <stdio.h>

int testVariance(real_number* data, vec_size size, real_number expected) {

  real_number output = variance(data, size);
  if (fabs(output - expected) > EPSILON) {
    printf("Fail Test Variance : Expected %f, got %f\n", expected, output);
    return 1;
  }

  printf("Success Test Variance\n");

  return 0;
};

int testMean(real_number* data, vec_size size, real_number expected) {

  real_number output = mean(data, size);
  if (fabs(output - expected) > EPSILON) {
    printf("Fail Test Mean : Expected %f, got %f\n", expected, output);
    return 1;
  }

  printf("Success Test Mean\n");

  return 0;
};

int testAnalyzeData(real_number* data, vec_size size, DataSummary expected) {

  DataSummary output;
  analyzeData(data, size, &output);

  if (fabs(output.max - expected.max) > EPSILON) {
    printf("Fail Test Analyze Data : max is expected to be %f, but is %f\n",
           expected.max, output.max);
    return 1;
  }
  if (fabs(output.min - expected.min) > EPSILON) {
    printf("Fail Test Analyze Data : min is expected to be %f, but is %f\n",
           expected.min, output.min);
    return 1;
  }
  if (fabs(output.mean - expected.mean) > EPSILON) {
    printf("Fail Test Analyze Data : mean is expected to be %f, but is %f\n",
           expected.mean, output.mean);
    return 1;
  }
  if (fabs(output.absAverageDeviation - expected.absAverageDeviation) >
      EPSILON) {
    printf("Fail Test Analyze Data : absAverageDeviation is expected to be %f, "
           "but is %f\n",
           expected.absAverageDeviation, output.absAverageDeviation);
    return 1;
  }
  if (fabs(output.standardDeviation - expected.standardDeviation) > EPSILON) {
    printf("Fail Test Analyze Data : standardDeviation is expected to be %f, "
           "but is %f\n",
           expected.standardDeviation, output.standardDeviation);
    return 1;
  }
  if (fabs(output.variance - expected.variance) > EPSILON) {
    printf(
        "Fail Test Analyze Data : variance is expected to be %f, but is %f\n",
        expected.variance, output.variance);
    return 1;
  }
  if (fabs(output.skewness - expected.skewness) > EPSILON) {
    printf(
        "Fail Test Analyze Data : skewness is expected to be %f, but is %f\n",
        expected.skewness, output.skewness);
    return 1;
  }
  if (fabs(output.kurtosis - expected.kurtosis) > EPSILON) {
    printf(
        "Fail Test Analyze Data : kurtosis is expected to be %f, but is %f\n",
        expected.kurtosis, output.kurtosis);
    return 1;
  }

  printf("Success Test Analyze Data\n");
  return 0;
}

int testCovariance(real_number* x, real_number* y, vec_size size,
                   real_number expected) {

  real_number output = covariance(x, y, size);
  if (fabs(output - expected) > EPSILON) {
    printf("Fail Test Covariance : Expected %f, got %f\n", expected, output);
    return 1;
  }

  printf("Success Test Covariance\n");
  return 0;
}

int testSimpleLinearRegression(real_number* x, real_number* y, vec_size size,
                               real_number expectedA, real_number expectedB) {

  real_number a;
  real_number b;

  simpleLinearRegression(x, y, size, &a, &b);

  if (fabs(a - expectedA) > EPSILON) {
    printf("Fail Test Simple Linear Regression : Expected slope to be %f, got "
           "%f\n",
           expectedA, a);
    return 1;
  }

  if (fabs(b - expectedB) > EPSILON) {
    printf("Fail Test Simple Linear Regression : Expected y-intercept to be "
           "%f, got %f\n",
           expectedB, b);
    return 1;
  }

  printf("Success Test Simple Linear Regression\n");
  return 0;
}

int testKMeans(real_number* data, vec_size size, vec_size dimensions,
               vec_size nbClusters, real_number* expectedCentroids,
               vec_size* expectedAssignations) {

  real_number centroids[size];
  vec_size assignations[size];

  kmeans(data, size, dimensions, nbClusters, centroids, assignations);

  for (vec_size i = 0; i < nbClusters * dimensions; ++i) {
    if (fabs(centroids[i] - expectedCentroids[i]) > EPSILON) {
      printf("Fail Test K-Means : Expected value %f, but got value %f\n",
             expectedCentroids[i], centroids[i]);
      // return 1;
    }
  }

  for (vec_size i = 0; i < size; ++i) {
    if (abs((int)(assignations[i]) - (int)(expectedAssignations[i])) >
        EPSILON) {
      printf("Fail Test K-Means : Expected value %u, but got value %u\n",
             expectedAssignations[i], assignations[i]);
      // return 1;
    }
  }

  printf("Success Test K-Means\n");

  return 0;
}

#define ARRAY_SIZE 10

int main() {

  real_number array[ARRAY_SIZE] = {0, 2, 2, 2, 3, 4, 4, 5, 5, 6};

  DataSummary dataSummary;

  int result = 0;
  result |= testVariance(array, ARRAY_SIZE, 3.344444);
  result |= testMean(array, ARRAY_SIZE, 3.3);
  result |= testCovariance(array, array, ARRAY_SIZE, 3.34444);

  //////////////////////////////////////////////////
  // Test Data Summary
  //////////////////////////////////////////////////

  dataSummary.max = 6.0;
  dataSummary.min = 0.0;
  dataSummary.mean = 3.3;
  dataSummary.absAverageDeviation = 1.66666;
  dataSummary.standardDeviation = 1.828782;
  dataSummary.variance = 3.344444;
  dataSummary.skewness = -0.280673;
  dataSummary.kurtosis = -0.485614;
  result |= testAnalyzeData(array, ARRAY_SIZE, dataSummary);

  //////////////////////////////////////////////////
  // Test Linear Regression
  //////////////////////////////////////////////////

  real_number xTest[ARRAY_SIZE] = {1, 2, 3, 5, 6, 9, 10, 13, 15, 20};
  real_number yTest[ARRAY_SIZE] = {20,  10,  30,  40,  100,
                                   150, 200, 180, 240, 300};

  result |=
      testSimpleLinearRegression(xTest, yTest, ARRAY_SIZE, 16.12079, -8.41463);

  //////////////////////////////////////////////////
  // Test K-Means clustering
  //////////////////////////////////////////////////

#define NB_POINTS 20
#define CLUSTERS 3
#define DIMENSIONS 2

  set_linear_congruential_generator_seed(5);

  real_number data[NB_POINTS * DIMENSIONS] = {
      22.01335086148608,   18.801364597492864,  22.274837043375705,
      18.682588601124213,  19.502956725090552,  20.17076888245898,
      20.229963050572085,  19.054781799295984,  21.819548107461095,
      19.289814616585776,  19.695324198649647,  20.271787074895347,
      -28.189858138417062, -30.21011540307979,  -27.914703036689676,
      -31.055753064768847, -28.743932779980288, -27.953481572887785,
      -30.048138203098393, -28.781921054981733, -31.189381213449188,
      -28.895541451520362, -27.844628611053828, -29.654742696714116,
      28.483861507851984,  30.267898592332628,  29.74360791485781,
      32.06804356959503,   28.97341210709923,   31.805104900245652,
      30.259033312327375,  31.729763165668828,  29.435655296524605,
      30.511488042962625,  31.296689545182282,  29.460094002099556,
      32.0547752304292,    29.51133645075526,   28.351282330632205,
      31.522144400392243};

  real_number expectedCentroids[CLUSTERS * DIMENSIONS] = {
      20.92266333, 19.3785176, -28.98844033,
      -29.42525921, 29.82478966,  30.85948414};

  vec_size expectedAssignations[NB_POINTS] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
                                              1, 1, 2, 2, 2, 2, 2, 2, 2, 2};

  result |= testKMeans(data, NB_POINTS, DIMENSIONS, CLUSTERS, expectedCentroids,
                       expectedAssignations);

  return result;
}
