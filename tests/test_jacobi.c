#include "jacobi.h"
#include "stdio.h"

int testMaximumIndex(real_number* input, vec_size size, vec_size expectedRow,
                     vec_size expectedCol) {

  vec_size maxRow;
  vec_size maxCol;
  jacobiMaxIndex(input, size, &maxRow, &maxCol);

  if (maxRow == expectedRow && maxCol == expectedCol) {
    printf("Success : Test maximum index\n");
    return 0;
  } else {
    printf("Fail : Test maximum index. Row should be %d and column should be "
           "%d, but row is %d and column is %d\n",
           expectedRow, expectedCol, maxRow, maxCol);
    return 1;
  }
}

int testJacobi(real_number* input, real_number* expectedEigenValues,
               real_number* expectedEigenVectors, vec_size size,
               vec_size iterations, vec_size cyclic, real_number epsilon) {

  vec_size squaredSize = size * size;

  real_number mat1[squaredSize];
  memcpy(mat1, input, sizeof(real_number) * squaredSize);

  real_number output[squaredSize];

  jacobi(mat1, size, output, iterations, cyclic);

  for (vec_size i = 0; i < squaredSize; ++i) {
    real_number diff = fabs(output[i] - expectedEigenVectors[i]);
    if (diff > epsilon) {
      vec_size x = i % size;
      vec_size y = i / size;
      printf("Fail : Test jacobi with cyclic = %d. Value of eigenvector matrix "
             "at (%d, %d) should be %f, but is %f (difference is %f, but must "
             "be less than %f)\n",
             cyclic, x, y, expectedEigenVectors[i], output[i], diff, epsilon);
      return 1;
    }
  }

  for (vec_size i = 0; i < squaredSize; ++i) {
    real_number diff = fabs(mat1[i] - expectedEigenValues[i]);
    if (diff > epsilon) {
      vec_size x = i % size;
      vec_size y = i / size;
      printf("Fail : Test jacobi with cyclic = %d. Value of eigenvalues matrix "
             "at (%d, %d) should be %f, but is %f (difference is %f, but must "
             "be less than %f)\n",
             cyclic, x, y, expectedEigenValues[i], mat1[i], diff, epsilon);
      return 1;
    }
  }

  printf("Success : Test jacobi method with cyclic = %d\n", cyclic);

  return 0;
}

int main() {

  //////////////////////////////////////////////////
  // Test Maximum Index
  //////////////////////////////////////////////////

  real_number matTestMaxIndex[9] = {0, 1, 2, 8, 4, 2, 4, 5, 6};

  int fail = testMaximumIndex(matTestMaxIndex, 3, 1, 0);

  //////////////////////////////////////////////////
  // Test Jacobi
  //////////////////////////////////////////////////

#define EPSILON_CMP 0.001

  real_number matTestJacobi[9] = {1, 2, 1, 2, 1, 2, 1, 2, 1};

  real_number expectedEigenValues[9] = {4.372281, 0.00000,   0.000000,
                                        0.000000, -1.372281, 0.000000,
                                        0.000000, 0.00000,   0.000000};

  real_number expectedEigenVectors[9] = {-0.541774, 0.454401,  -0.707107,
                                         -0.642621, -0.766185, 0.0000000,
                                         -0.541774, 0.454401,  0.707107};

  // The numbers are different in the cyclic version since depending on the
  // jacobi rotations, the eigenvectors can be inverted or not.
  real_number expectedEigenVectorsCyclic[9] = {-0.541774, -0.454401, 0.707107,
                                               -0.642621, 0.766185,  0.0000000,
                                               -0.541774, -0.454401, -0.707107};

  fail |= testJacobi(matTestJacobi, expectedEigenValues,
                     expectedEigenVectorsCyclic, 3, 10, 1, EPSILON_CMP);
  fail |= testJacobi(matTestJacobi, expectedEigenValues, expectedEigenVectors,
                     3, 10, 0, EPSILON_CMP);

  return fail;
}
