
#include "genetic.h"

// We declare these constants to accelerate the execution of the algorithm
#define UINT16_MAX_INVERSE 1 / UINT16_MAX
#define UINT16_DIGIT_COUNT 5

/**
 * This method initialises the array with random unsigned integers between 0 and
 * UINT16_MAX
 *
 * @param population This array stores all the values of the population and will
 * be filled
 */
static void fillTable(uint16_t* population) {

  const unsigned int populationArraySize = populationSize * dimensions;

  for (unsigned int i = 0; i < populationArraySize; i++) {

    population[i] = linear_congruential_random_generator() * UINT16_MAX;
  }
}

/**
 * This function is used to choose two parents based off of a tourney approach
 *
 * @param population  an array that stores all of the current solutions
 * @param firstParentIndex a return parameter for storing the index of the first
 * chosen parent
 * @param secondParentIndex a return parameter for storing the index of the
 * second chosen parent
 * @param evaluationFunction the function that is used to evaluate each solution
 */
static void tourney(uint16_t* population, unsigned int* firstParentIndex,
                    unsigned int* secondParentIndex,
                    float (*evaluationFunction)(float*)) {

  float chosenIndexes[populationSize];
  float bestFitness = 0;
  float secondbestFitness = 0;

  for (unsigned int i = 0; i < tournamentSelectionsSize; i++) {

    unsigned int index =
        (unsigned int)(linear_congruential_random_generator() * populationSize);
    int isAlreadyChosen = 1;

    chosenIndexes[i] = index;
    float parameters[dimensions];

    if (i != 0) {

      for (unsigned int j = 0; j < i; j++) {

        if (chosenIndexes[j] == index) {
          i--;
          isAlreadyChosen = 0;
          break;
        }
      }
    }
    unsigned int populationIndex = index * dimensions;
    unsigned int limit = populationIndex + dimensions;

    for (unsigned int j = populationIndex; j < limit; j++) {
      parameters[j - populationIndex] =
          ((float)population[j] * UINT16_MAX_INVERSE);
    }

    float fitness = evaluationFunction(parameters);

    if (i == 0 || (isAlreadyChosen && bestFitness > fitness)) {

      *secondParentIndex = *firstParentIndex;

      *firstParentIndex = index;
      bestFitness = fitness;

    } else if (i == 1 || secondbestFitness > fitness) {
      *secondParentIndex = index;
      secondbestFitness = fitness;
    }
  }
}

/**
 * This function simulates the mutation genetic operation on our created child
 *
 * @param gene  this char array represents the created child and at each index
 * is a digit
 * @param geneLength the length of the created child
 */
static void mutate(char* gene, unsigned int geneLength) {

  float mutation =
      (unsigned int)(linear_congruential_random_generator() * populationSize) %
      geneLength;

  if (mutation <= mutationRate) {

    unsigned int mutatedIndex =
        (unsigned int)(linear_congruential_random_generator() *
                       populationSize) %
        geneLength;

    // We then generate a number between 0 and 9 to replace the chosen digit
    unsigned int newValue =
        (unsigned int)(linear_congruential_random_generator() *
                       populationSize) %
        10;

    // We handle the last digit
    if (mutatedIndex % UINT16_DIGIT_COUNT == 0) {
      newValue = (unsigned int)(linear_congruential_random_generator() *
                                populationSize) %
                 7;
    }
    // Converts int to char
    gene[mutatedIndex] = newValue + '0';
  }
}

/**
 * This function divides the created child array in the respective parameters of
 * the solution before adding the created child solution to the next generation
 *
 * @param nextGeneration  the array containing all of the created chilren
 * @param nextGenerationSize the amount of values in the array of created
 * children
 * @param child the encoded string of the new child
 */
static void decodeAndAddChild(uint16_t* nextGeneration,
                              unsigned int* nextGenerationSize, char* child) {

  unsigned int dimensionIndex = 0;

  char parameter[UINT16_DIGIT_COUNT + 1];
  parameter[UINT16_DIGIT_COUNT] = '\0';

  unsigned int basePositionning = *(nextGenerationSize)*dimensions;

  for (unsigned int i = 0; i < dimensions; i++) {

    for (unsigned int j = 0; j < UINT16_DIGIT_COUNT; j++) {

      parameter[j] = child[j + dimensionIndex];
    }

    dimensionIndex += UINT16_DIGIT_COUNT;

    unsigned int value = atoi(parameter);

    if (value > UINT16_MAX) {
      value = UINT16_MAX;
    }

    nextGeneration[basePositionning + i] = value;
  }
  (*nextGenerationSize)++;
  basePositionning++;
}

/**
 *
 * We create the children based off of a uniform crossover with two parents and
 * apply the genetic operators
 *
 * @param firstParent  the first selected parent solution
 * @param secondParent  the second selected parent solution
 * @param nextGeneration the array containing all of the created chilren
 * @param nextGenerationSize the amount of values in the array of created
 * children
 */
static void createChildren(char* firstParent, char* secondParent,
                           uint16_t* nextGeneration,
                           unsigned int* nextGenerationSize) {

  const unsigned int arraySize = UINT16_DIGIT_COUNT * dimensions;
  const unsigned int encodedChildStringSize = arraySize + 1;

  char firstChildString[encodedChildStringSize];
  char secondChildString[encodedChildStringSize];

  firstChildString[arraySize] = '\0';
  secondChildString[arraySize] = '\0';

  for (unsigned int i = 0; i < (UINT16_DIGIT_COUNT * dimensions); i++) {

    if (linear_congruential_random_generator() <= 0.5) {

      firstChildString[i] = firstParent[i];
      secondChildString[i] = secondParent[i];
    } else {
      firstChildString[i] = secondParent[i];
      secondChildString[i] = firstParent[i];
    }
  }

  // We apply the mutation operator to both children
  mutate(firstChildString, arraySize);
  mutate(secondChildString, arraySize);

  // We decode both children and add them to the next generation
  decodeAndAddChild(nextGeneration, nextGenerationSize, firstChildString);
  decodeAndAddChild(nextGeneration, nextGenerationSize, secondChildString);
}

/**
 * We encode a solution as a string , the given string will always be the same
 * length
 *
 * @param combinedValue the created string containing the encoded parent
 * @param parent an array containing the parameters of the parent
 */
static void encode(char* combinedValue, uint16_t* parent) {

  unsigned int arrIndex = 0;
  unsigned int encodedParameterSize = UINT16_DIGIT_COUNT + 1;

  for (unsigned int i = 0; i < dimensions; i++) {

    char stringValue[encodedParameterSize];
    char otherString[encodedParameterSize];

    stringValue[UINT16_DIGIT_COUNT] = '\0';
    otherString[UINT16_DIGIT_COUNT] = '\0';

    unsigned int otherStringLength = 0;
    for (int j = (UINT16_DIGIT_COUNT - 1); j >= 0; j--) {

      if (parent[i] < pow(10, j)) {
        otherString[otherStringLength] = '0';
        otherStringLength++;
      } else {
        break;
      }
    }

    sprintf(stringValue, "%hu", parent[i]);

    if (otherStringLength != 0) {
      for (unsigned int j = 0; j < otherStringLength; j++) {
        combinedValue[j + arrIndex] = otherString[j];
      }
    }

    for (unsigned int j = otherStringLength; j < UINT16_DIGIT_COUNT; j++) {

      combinedValue[j + arrIndex] = stringValue[j - otherStringLength];
    }
    arrIndex += UINT16_DIGIT_COUNT;
  }
}

/**
 * This function takes care of creating the next generation and selecting the
 * parents
 *
 * @param population  this array stores all the values of the population
 * @param nextGeneration this array is used to store the parametres of the
 * created children
 * @param evaluationFunction the function that is used to evaluate each solution
 * in the population
 */
static void createNextGeneration(uint16_t* population, uint16_t* nextGeneration,

                                 float (*evaluationFunction)(float*)) {

  unsigned int currentNextGenerationSize = 2;
  const unsigned int mergedParentsLength =
      (dimensions * UINT16_DIGIT_COUNT) + 1;
  while (currentNextGenerationSize < (populationSize)) {

    unsigned int parent1Index, parent2Index = 0;
    tourney(population, &parent1Index, &parent2Index, evaluationFunction);

    uint16_t parent1[dimensions];
    uint16_t parent2[dimensions];

    for (int i = 0; i < dimensions; i++) {

      parent1[i] = population[(parent1Index * dimensions) + i];
      parent2[i] = population[(parent2Index * dimensions) + i];
    }

    char mergedParents1[mergedParentsLength];
    encode(mergedParents1, parent1);
    char mergedParents2[mergedParentsLength];
    encode(mergedParents2, parent2);

    createChildren(mergedParents1, mergedParents2, nextGeneration,
                   &currentNextGenerationSize);
  }
}

/**
 * This algorithm uses the elitism principal and this function helps us keep
 * track of the current most optimal solution
 *
 *
 * @param population this array stores all the values of the population
 * @param bestValuesCoordinates this array stores the parameters of the current
 * best solution
 * @param index  this represents the index of the new best solution
 * @param storedValue this represents the current most optimal fitness
 * @param newValue this represents the new superior fitness
 */
static void replaceEliteValue(uint16_t* population,
                              uint16_t* bestValuesCoordinates,
                              unsigned int index, float* storedValue,
                              float newValue) {

  *storedValue = newValue;
  for (unsigned int i = 0; i < dimensions; i++) {
    bestValuesCoordinates[i] = population[index + i];
  }
}

/**
 * This function calculates and stores the two best solutions of the population
 *
 * @param population this array stores the values of eaech parameter of the
 * population
 * @param bestFit this is the best fitness that was calculated
 * @param bestFitCoord these are the parameters of the best solution
 * @param evaluationFunction this function is used to evaluate each solution
 * @param secondBestValues  this is the second best fitness that was calculated
 * @param secondBestFitness  these are the parameters of the second solution
 */
static void calculateFitness(uint16_t* population, float* bestFit,
                             uint16_t* bestFitCoord,
                             float (*evaluationFunction)(float*),
                             uint16_t* secondBestValues,
                             float* secondBestFitness) {

  for (unsigned int i = 0; i < populationSize; i++) {

    float parameters[dimensions];
    const unsigned int baseIndex = i * dimensions;

    for (unsigned int j = 0; j < dimensions; j++) {
      parameters[j] = ((float)population[baseIndex + j] * UINT16_MAX_INVERSE);
    }

    const float strength = evaluationFunction(parameters);

    if (strength < *(bestFit)) {

      *secondBestFitness = *bestFit;
      memcpy(secondBestValues, bestFitCoord, dimensions * sizeof(uint16_t));

      replaceEliteValue(population, bestFitCoord, (i * dimensions), bestFit,
                        strength);
    }

    else if (strength < *(secondBestFitness)) {
      replaceEliteValue(population, secondBestValues, (i * dimensions),
                        secondBestFitness, strength);
    }
  }
}

/**
 * This function replaces the current population by its children and follows the
 * principles of a genetic algorithm
 *
 *
 * @param population this array stores all the values of the population
 * @param newGeneration this array stores all of the created children
 * @param bestFitValues this array stores the parameters of the current best
 * solution
 * @param secondBestValues this array stores the parameters of the current
 * second best solution
 * @param arraySize this indicates the size of the population
 */
static void replacePopulation(uint16_t* population, uint16_t* newGeneration,
                              uint16_t* bestFitValues,
                              uint16_t* secondBestValues,
                              unsigned int arraySize) {

  unsigned int secondEliteValueIndex = dimensions;

  for (unsigned int i = 0; i < dimensions; i++) {
    newGeneration[i] = bestFitValues[i];
    newGeneration[secondEliteValueIndex + i] = secondBestValues[i];
  }

  memcpy(population, newGeneration, arraySize * sizeof(uint16_t));
}

/**
 * This function runs a genetic algorithm to minimize a function by finding the
 * best parameters possible the epsilon is the goal fitness and the execution
 * stops once that it is attained the other cutoff is the maximumIterationCount
 * parameter
 *
 * @param bestFitValues this array stores the parameters of the best solution
 * @param parameterCount this function indicates the number of parameters in the
 * efunction to optimize
 * @param epsilon the target fitness to achieve
 * @param mutationChance the chance that a created child will have a mutation
 * @param generationSize the amount of solutions to add into the population
 * @param maximumIterationCount the maximum amount of generations that will be
 * created
 * @param tourneySize the number of solutions that are chosen in the tourney ,
 * must be smallere than the population size
 * @param evaluationFunction the function that is used to
 * @return float the fitness of the best solution
 */
float geneticAlgorithm(float* bestFitValues, const unsigned int parameterCount,
                       const float epsilon, const float mutationChance,
                       const unsigned int generationSize,
                       const unsigned int generations,
                       const unsigned int tourneySize,
                       float (*evaluationFunction)(float*)) {

  mutationRate = mutationChance;
  populationSize = generationSize;
  numberOfGenerations = generations;
  dimensions = parameterCount;
  tournamentSelectionsSize = tourneySize;

  if (populationSize % 2)
    populationSize += 1;

  if (tournamentSelectionsSize > populationSize)
    exit(1);
  float bestFit = FLT_MAX;

  const unsigned int arraySize = populationSize * dimensions;

  uint16_t population[arraySize];
  uint16_t nextGeneration[arraySize];

  uint16_t bestValues[dimensions];
  uint16_t secondBestValues[dimensions];
  float secondBestFitness = FLT_MAX;

  fillTable(population);

  for (unsigned int i = 0; i < numberOfGenerations; i++) {

    calculateFitness(population, &bestFit, bestValues, evaluationFunction,
                     secondBestValues, &secondBestFitness);

    createNextGeneration(population, nextGeneration, evaluationFunction);

    replacePopulation(population, nextGeneration, bestValues, secondBestValues,
                      arraySize);

    if (bestFit <= epsilon) {
      break;
    }
  }
  for (unsigned int j = 0; j < dimensions; j++) {
    bestFitValues[j] = bestValues[j] * UINT16_MAX_INVERSE;
  }
  return bestFit;
}
