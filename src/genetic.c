
#include "genetic.h"

// We declare these constants to accelerate the execution of the algorithm
#define UINT16_MAX_INVERSE (1.0 / UINT16_MAX)
#define UINT16_DIGIT_COUNT 5

// This variable decides the maxixmum amount of generations that can be created
static unsigned int numberOfGenerations;

// This controls the quantity of solutions per generation. This value greatly
// controls the memory used by this algorithm
static unsigned int populationSize;

// This value controls the chance that a child will have a mutation
static float mutationRate;

// The amount of parameters that each solution need
static unsigned int dimensions;

// The amount of solutions chosen for each parent-selection tourney
static unsigned int tournamentSelectionsSize;

/**
 * Converts an unsigned integer digit to a char of the same number
 *
 * @param value A one digit positive integer
 * @return char
 */
static inline char intDigitToChar(unsigned int value) { return value + '0'; }

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
 * @param populationStrength  an array that stores all the fitness of the
 * population
 * @param firstParentIndex a return parameter for storing the index of the first
 * chosen parent
 * @param secondParentIndex a return parameter for storing the index of the
 * second chosen parent
 */
static void tourney(float* populationStrength, unsigned int* firstParentIndex,
                    unsigned int* secondParentIndex) {

  float chosenIndexes[tournamentSelectionsSize];
  float bestFitness = FLT_MAX;
  float secondbestFitness = FLT_MAX;

  for (unsigned int i = 0; i < tournamentSelectionsSize; i++) {

    unsigned int index =
        linear_congruential_random_generator() * populationSize;
    uint8_t isNotAlreadyChosen = 1;

    // We generate a float between 0 and populationSize and then convert it to
    // an int. On the slight chance that the generated number by
    // linear_congruential_generator = 1.00 we loop again.
    // This solution lets us have exactly the same amount of odds to get each
    // number
    while (index == populationSize) {
      index = linear_congruential_random_generator() * populationSize;
    }

    for (unsigned int j = 0; j < i; j++) {

      if (chosenIndexes[j] == index) {
        i--;
        isNotAlreadyChosen = 0;
        break;
      }
    }

    const float fitness = populationStrength[index];

    if (isNotAlreadyChosen) {
      chosenIndexes[i] = index;
    } else {
      continue;
    }

    if (bestFitness > fitness) {

      *secondParentIndex = *firstParentIndex;
      secondbestFitness = bestFitness;
      *firstParentIndex = index;
      bestFitness = fitness;

    } else if (secondbestFitness > fitness) {
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

  const float mutation = linear_congruential_random_generator();

  if (mutation <= mutationRate) {

    const unsigned int mutatedIndex =
        linear_congruential_random_generator() * (geneLength - 1);

    // We then generate a number between 0 and 9 to replace the chosen digit
    unsigned int newValue = linear_congruential_random_generator() * 9;

    // We handle the last digits of each parameter
    // The values are stored as uint16_t so the largest number of the last digit
    // of each parameter is 6
    if (mutatedIndex % UINT16_DIGIT_COUNT == 0) {
      newValue = linear_congruential_random_generator() * 6;
    }
    // Converts int to char
    gene[mutatedIndex] = intDigitToChar(newValue);
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

  char parameter[UINT16_DIGIT_COUNT + 1];
  parameter[UINT16_DIGIT_COUNT] = '\0';

  unsigned int basePositionning = *nextGenerationSize * dimensions;

  for (unsigned int i = 0; i < dimensions; i++) {

    memcpy(parameter, child + (i * UINT16_DIGIT_COUNT),
           UINT16_DIGIT_COUNT * sizeof(char));

    uint32_t value = atoi(parameter);

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

  for (unsigned int i = 0; i < arraySize; i++) {

    if (linear_congruential_random_generator() <= 0.5) {
      memcpy(firstChildString + i, firstParent + i, sizeof(char));
      memcpy(secondChildString + i, secondParent + i, sizeof(char));
    } else {
      memcpy(firstChildString + i, secondParent + i, sizeof(char));
      memcpy(secondChildString + i, firstParent + i, sizeof(char));
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

  for (int i = 0; i < dimensions; i++) {

    sprintf(combinedValue + (i * UINT16_DIGIT_COUNT), "%.5hu", parent[i]);
  }
}
/**
 * This function takes care of creating the next generation and selecting the
 * parents
 *
 * @param population  this array stores all the values of the population
 * @param nextGeneration this array is used to store the parametres of the
 * created children
 * @param populationFitness this array is used to store the fitness of each
 * solution of the population
 */
static void createNextGeneration(uint16_t* population, uint16_t* nextGeneration,

                                 float* populationFitness) {

  unsigned int currentNextGenerationSize = 0;

  const unsigned int nextGenerationMaxSize = populationSize - 2;
  const unsigned int mergedParentsLength =
      (dimensions * UINT16_DIGIT_COUNT) + 1;

  const unsigned int parentArrayByteSize = dimensions * sizeof(*population);

  while (currentNextGenerationSize < nextGenerationMaxSize) {

    unsigned int parent1Number, parent2Number;
    tourney(populationFitness, &parent1Number, &parent2Number);
    uint16_t parent1[dimensions];
    uint16_t parent2[dimensions];

    const uint16_t parent1Index = parent1Number * dimensions;
    const uint16_t parent2Index = parent2Number * dimensions;

    memcpy(parent1, population + parent1Index, parentArrayByteSize);
    memcpy(parent2, population + parent2Index, parentArrayByteSize);

    char mergedParents1[mergedParentsLength];
    encode(mergedParents1, parent1);
    char mergedParents2[mergedParentsLength];
    encode(mergedParents2, parent2);

    createChildren(mergedParents1, mergedParents2, nextGeneration,
                   &currentNextGenerationSize);
  }
}

/**
 * This function calculates and stores the fitness of each solution of the
 * population
 *
 * @param population this array stores the values of eaech parameter of the
 * population
 * @param populationFitness this array stores the fitness of each solution
 * @param bestFit this is the best fitness that was calculated
 * @param bestFitCoord these are the parameters of the best solution
 * @param evaluationFunction this function is used to evaluate each solution
 * @param secondBestValues  this is the second best fitness that was calculated
 * @param secondBestFitness  these are the parameters of the second solution
 */
static void calculateFitness(uint16_t* population, float* populationFitness,
                             float* bestFit, uint16_t* bestFitCoord,
                             fitness_evaluation_function evaluationFunction,
                             uint16_t* secondBestValues,
                             float* secondBestFitness) {

  const unsigned int coordArrayByteSize = dimensions * sizeof(*population);

  for (unsigned int i = 0; i < populationSize; i++) {

    float parameters[dimensions];
    const unsigned int baseIndex = i * dimensions;

    for (unsigned int j = 0; j < dimensions; j++) {
      parameters[j] = population[baseIndex + j] * UINT16_MAX_INVERSE;
    }

    const float fitness = evaluationFunction(parameters);
    populationFitness[i] = fitness;

    if (fitness < *(bestFit)) {

      *secondBestFitness = *bestFit;
      *bestFit = fitness;

      memcpy(secondBestValues, bestFitCoord, coordArrayByteSize);
      memcpy(bestFitCoord, population + (i * dimensions), coordArrayByteSize);

    }

    else if (fitness < *(secondBestFitness)) {

      *secondBestFitness = fitness;
      memcpy(secondBestValues, population + (i * dimensions),
             coordArrayByteSize);
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

  const unsigned int nextGenerationStartingIndex = 2 * dimensions;

  // We copy the elite values before copying the rest of the values from the
  // next generation

  memcpy(population, bestFitValues, dimensions * sizeof(uint16_t));
  memcpy(population + dimensions, secondBestValues,
         dimensions * sizeof(uint16_t));
  memcpy(population + nextGenerationStartingIndex, newGeneration,
         arraySize * sizeof(uint16_t));
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
                       const unsigned int maximumIterationCount,
                       const unsigned int tourneySize,
                       fitness_evaluation_function evaluationFunction) {

  mutationRate = mutationChance;
  populationSize = generationSize;
  numberOfGenerations = maximumIterationCount;
  dimensions = parameterCount;
  tournamentSelectionsSize = tourneySize;

  if (populationSize % 2)
    populationSize += 1;

  if (tournamentSelectionsSize > populationSize)
    exit(1);

  float bestFit = FLT_MAX;

  const unsigned int arraySize = populationSize * dimensions;
  // We created a seperate size because the child array will be two smaller than
  // the population because of the two elite values that are reinjected
  const unsigned int childArraySize = arraySize - 2;

  uint16_t population[arraySize];
  uint16_t nextGeneration[childArraySize];
  float populationFitness[populationSize];

  uint16_t bestValues[dimensions];

  uint16_t secondBestValues[dimensions];
  float secondBestFitness = FLT_MAX;

  fillTable(population);

  for (unsigned int i = 0; i < numberOfGenerations; i++) {

    calculateFitness(population, populationFitness, &bestFit, bestValues,
                     evaluationFunction, secondBestValues, &secondBestFitness);

    createNextGeneration(population, nextGeneration, populationFitness);

    replacePopulation(population, nextGeneration, bestValues, secondBestValues,
                      childArraySize);

    if (bestFit <= epsilon) {
      break;
    }
  }

  for (unsigned int j = 0; j < dimensions; j++) {
    bestFitValues[j] = bestValues[j] * UINT16_MAX_INVERSE;
  }
  return bestFit;
}
