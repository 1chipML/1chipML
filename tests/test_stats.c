#include "stats.h"
#include <stdio.h>

int testVariance(real_number* input, vec_size size, real_number expected) {

    real_number output = variance(input, size);
    if (fabs(output - expected) > EPSILON) {
        printf("Fail Test Sample Variance : Expected %f, got %f\n", expected, output);
        return 1;
    }

    printf("Success Test Sample Variance\n");

    return 0;
};

int testMean(real_number* input, vec_size size, real_number expected) {

    real_number output = mean(input, size);
    if (fabs(output - expected) > EPSILON) {
        printf("Fail Test Mean : Expected %f, got %f\n", expected, output);
        return 1;
    }

    printf("Success Test Mean\n");

    return 0;
};

int testAnalyzeData(real_number* array, vec_size size, DataSummary expected) {

    DataSummary output;
    analyzeData(array, size, &output);

    if (fabs(output.max - expected.max) > EPSILON) {
        printf("Fail Test Analyze Data : max is expected to be %f, but is %f\n", expected.max, output.max);
        return 1;
    } 
    if (fabs(output.min - expected.min) > EPSILON) {
        printf("Fail Test Analyze Data : min is expected to be %f, but is %f\n", expected.min, output.min);
        return 1;
    } 
    if (fabs(output.mean - expected.mean) > EPSILON) {
        printf("Fail Test Analyze Data : mean is expected to be %f, but is %f\n", expected.mean, output.mean);
        return 1;
    } 
    if (fabs(output.absAverageDeviation - expected.absAverageDeviation) > EPSILON) {
        printf("Fail Test Analyze Data : absAverageDeviation is expected to be %f, but is %f\n", expected.absAverageDeviation, output.absAverageDeviation);
        return 1;
    } 
    if (fabs(output.standardDeviation - expected.standardDeviation) > EPSILON) {
        printf("Fail Test Analyze Data : standardDeviation is expected to be %f, but is %f\n", expected.standardDeviation, output.standardDeviation);
        return 1;
    } 
    if (fabs(output.variance - expected.variance) > EPSILON) {
        printf("Fail Test Analyze Data : variance is expected to be %f, but is %f\n", expected.variance, output.variance);
        return 1;
    } 
    if (fabs(output.skewness - expected.skewness) > EPSILON) {
        printf("Fail Test Analyze Data : skewness is expected to be %f, but is %f\n", expected.skewness, output.skewness);
        return 1;
    } 
    if (fabs(output.kurtosis - expected.kurtosis) > EPSILON) {
        printf("Fail Test Analyze Data : kurtosis is expected to be %f, but is %f\n", expected.kurtosis, output.kurtosis);
        return 1;
    } 

    printf("Success Test Analyze Data\n");
    return 0;

}

int testCovariance(real_number* x, real_number* y, vec_size size, real_number expected) {

    real_number output = covariance(x, y, size);
    if (fabs(output - expected) > EPSILON) {
        printf("Fail Test Covariance : Expected %f, got %f\n", expected, output);
        return 1;
    }

    printf("Success Test Covariance\n");
    return 0;
}

int testSimpleLinearRegression(real_number* x, real_number* y, vec_size size, real_number expectedA, real_number expectedB) {

    real_number a;
    real_number b;

    simpleLinearRegression(x, y, size, &a, &b);

    if (fabs(a - expectedA) > EPSILON) {
        printf("Fail Test Simple Linear Regression : Expected slope to be %f, got %f\n", expectedA, a);
        return 1;
    }

    if (fabs(b - expectedB) > EPSILON) {
        printf("Fail Test Simple Linear Regression : Expected y-intercept to be %f, got %f\n", expectedB, b);
        return 1;
    }

    printf("Success Test Simple Linear Regression\n");
    return 0;
}

int testKMeans() {

    real_number data[12] = {
        0, 0,
        1, 0,
        10, 10,
        11, 10,
        -5, 5,
        -5.5, 5
    };

    real_number centroids[6];
    vec_size assignations[6];

    kmeans(data, 6, 2, 3, centroids, assignations);

    for (vec_size i = 0; i < 6; ++i) {
        printf("assignations (%f, %f) : %d -> %d\n", data[i * 2], data[i * 2 + 1], i, assignations[i]);
    }

    for (vec_size i = 0; i < 3; ++i) {
        printf("centroids : %f, %f\n", centroids[i * 2], centroids[i * 2 + 1]);
    }

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

    real_number xTest[ARRAY_SIZE] = {1, 2, 3, 5, 6, 9, 10, 13, 15, 20};
    real_number yTest[ARRAY_SIZE] = {20, 10, 30, 40, 100, 150, 200, 180, 240, 300};

    result |= testSimpleLinearRegression(xTest, yTest, ARRAY_SIZE, 16.12079, -8.41463);


    result |= testKMeans();

    return result;
}
