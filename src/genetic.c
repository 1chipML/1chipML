
#include "genetic.h"

/**
 * Converts an unsigned integer digit to a char of the same number
 *
 * @param value A one digit positive integer
 * @return char
 */
static inline char intDigitToChar(const unsigned int value) {
  return value + '0';
}

/**
 * This method initialises the array with random unsigned integers between 0 and
 * UINT16_MAX
 *
 * @param population This array stores all the values of the population and will
 * be filled
 * @param populationSize The size of the population of solutions
 * @param dimensions The number of parameters of the function to optimizes
 */
static void fillTable(genetic_int* population,
                      const unsigned int populationSize,
                      const unsigned int dimensions) {

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
 * @param tournamentSelectionsSize the amount of solutions that are randomly
 * selected for a tournament
 * @param populationSize the number of solutions in the population
 */
static void tourney(void* populationStrength, unsigned int* firstParentIndex,
                    unsigned int* secondParentIndex,
                    const unsigned int tournamentSelectionsSize,
                    const unsigned int populationSize) {
  genetic_real chosenIndexes[tournamentSelectionsSize];
  genetic_real bestFitness = FLT_MAX;
  genetic_real secondbestFitness = FLT_MAX;

  for (unsigned int i = 0; i < tournamentSelectionsSize; i++) {

    unsigned int index =
        linear_congruential_random_generator() * populationSize;
    uint8_t isNotAlreadyChosen = 1;

    // We generate a real between 0 and populationSize and then convert it to
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

    const genetic_real fitness = ((genetic_real*)populationStrength)[index];

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
 * This function is used to choose two parents based off of a tourney approach
 * for the low memory version of the algorithm
 *
 * @param population  an array that stores all of the current solutions
 * @param firstParentIndex a return parameter for storing the index of the first
 * chosen parent
 * @param secondParentIndex a return parameter for storing the index of the
 * second chosen parent
 * @param evaluationFunction the function that is used to evaluate each solution
 */
static void tourneyLowMemory(genetic_int* population,
                             unsigned int* firstParentIndex,
                             unsigned int* secondParentIndex,
                             fitness_evaluation_function evaluationFunction,
                             unsigned int tournamentSelectionsSize,
                             unsigned int populationSize,
                             unsigned int dimensions) {

  genetic_real chosenIndexes[tournamentSelectionsSize];
  genetic_real bestFitness = FLT_MAX;
  genetic_real secondbestFitness = FLT_MAX;

  for (unsigned int i = 0; i < tournamentSelectionsSize; i++) {

    unsigned int index =
        linear_congruential_random_generator() * populationSize;
    uint8_t isNotAlreadyChosen = 1;

    // We generate a real between 0 and populationSize and then convert it to
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

    if (isNotAlreadyChosen) {
      chosenIndexes[i] = index;
    } else {
      continue;
    }
    const unsigned int populationIndex = index * dimensions;
    genetic_real parameters[dimensions];

    for (unsigned int j = 0; j < dimensions; ++j) {
      parameters[j] = population[populationIndex + j] * INT_MAX_INVERSE;
    }

    const genetic_real fitness = evaluationFunction(parameters);

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
 * This function generates a number based off of a Poisson generation around the
 * given lambda
 *
 * @param expectedValue  the lambda for the Poisson table
 * @return unsigned int
 */
unsigned int randomPoissonGenerator(genetic_real expectedValue) {

  unsigned int value = 0;
  genetic_real number = linear_congruential_random_generator();

  // This value is the target that the multiplication of random numbers cannot
  // go below
  genetic_real cutoff = exp(-expectedValue);

  for (; number > cutoff; value++) {
    number *= linear_congruential_random_generator();
  }
  return value;
}

/**
 * This function simulates the mutation genetic operation on our created child
 *
 * @param gene  this char array represents the created child and at each index
 * is a digit
 * @param geneLength the length of the created child
 * @param averageNumberofMutations the average number of mutations on the child
 */
static void mutate(char* gene, const unsigned int geneLength,
                   const genetic_real averageNumberofMutations) {

  int numberOfMutations = randomPoissonGenerator(averageNumberofMutations);

  for (unsigned int i = 0; i < numberOfMutations; i++) {


    const double random =  abs(linear_congruential_random_generator());
    if (random < 0){
        abort();

    }
    const unsigned int mutatedIndex =
        random * (geneLength - 1);

    

    // We then generate a number between 0 and 9 to replace the chosen digit
    unsigned int newValue = linear_congruential_random_generator() * 10;

    // We do this to get an equal chance to generate a number between 0 and 9
    while (newValue == 10) {
      newValue = linear_congruential_random_generator() * 10;
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
 * @param dimensions the number of parameters in the function to minimizes
 */
static void decodeAndAddChild(genetic_int* nextGeneration,
                              unsigned int* nextGenerationSize, char* child,
                              const unsigned int dimensions) {

  char parameter[INT_MAX_DIGIT_COUNT + 1];
  parameter[INT_MAX_DIGIT_COUNT] = '\0';

  unsigned int basePositionning = *nextGenerationSize * dimensions;

  for (unsigned int i = 0; i < dimensions; i++) {

    memcpy(parameter, child + i * INT_MAX_DIGIT_COUNT, INT_MAX_DIGIT_COUNT);

    genetic_int value = atoi(parameter);

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
 * @param firstChildString the char array that will be used to store the first
 * child
 * @param secondChildString the char array that will be used to store the second
 * child
 */
static void createChildren(char* firstParent, char* secondParent,
                           char* firstChildString, char* secondChildString,
                           const unsigned int dimensions) {

  const unsigned int arraySize = INT_MAX_DIGIT_COUNT * dimensions;

  for (unsigned int i = 0; i < arraySize; i++) {

    if (linear_congruential_random_generator() <= 0.5) {
      memcpy(firstChildString + i, firstParent + i, sizeof(char));
      memcpy(secondChildString + i, secondParent + i, sizeof(char));
    } else {
      memcpy(firstChildString + i, secondParent + i, sizeof(char));
      memcpy(secondChildString + i, firstParent + i, sizeof(char));
    }
  }
}

/**
 * We encode a solution as a string , the given string will always be the same
 * length
 *
 * @param combinedValue the created string containing the encoded parent
 * @param parent an array containing the parameters of the parent
 * @param dimensions the number of parameters in the function to minimize
 */
static void encode(char* combinedValue, genetic_int* parent,
                   const unsigned int dimensions) {

  for (int i = 0; i < dimensions; i++) {

    sprintf(combinedValue + i * INT_MAX_DIGIT_COUNT, "%.5hu", parent[i]);
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
 * @param dimensions the number of parameters in the function to minimize
 * @param tournamentSelectionsSize the number of solutions that are selected to
 * be part of the tournament
 * @param averageNumberOfMutations the average number of mutations present on
 * each created child
 */
static void createNextGeneration(genetic_int* population,
                                 genetic_int* nextGeneration,
                                 genetic_real* populationFitness,
                                 const unsigned int populationSize,
                                 const unsigned int dimensions,
                                 const unsigned int tournamentSelectionsSize,
                                 const genetic_real averageNumberOfMutations) {

  unsigned int currentNextGenerationSize = 0;

  const unsigned int nextGenerationMaxSize = populationSize - 2;
  const unsigned int mergedParentsLength = dimensions * INT_MAX_DIGIT_COUNT + 1;

  const unsigned int mergedParentsLastIndex = mergedParentsLength - 1;

  const unsigned int parentArrayByteSize = dimensions * sizeof(*population);

  while (currentNextGenerationSize < nextGenerationMaxSize) {

    unsigned int parent1Number, parent2Number;
    tourney(populationFitness, &parent1Number, &parent2Number,
            tournamentSelectionsSize, populationSize);

    genetic_int parent1[dimensions];
    genetic_int parent2[dimensions];

    const genetic_int parent1Index = parent1Number * dimensions;
    const genetic_int parent2Index = parent2Number * dimensions;

    memcpy(parent1, population + parent1Index, parentArrayByteSize);
    memcpy(parent2, population + parent2Index, parentArrayByteSize);

    char mergedParents1[mergedParentsLength];
    char mergedParents2[mergedParentsLength];

    char firstChildString[mergedParentsLength];
    char secondChildString[mergedParentsLength];

    firstChildString[mergedParentsLastIndex] = '\0';
    secondChildString[mergedParentsLastIndex] = '\0';

    encode(mergedParents1, parent1, dimensions);
    encode(mergedParents2, parent2, dimensions);

    createChildren(mergedParents1, mergedParents2, firstChildString,
                  secondChildString, dimensions);

    // We apply the mutation operator to both children
    mutate(firstChildString, mergedParentsLength, averageNumberOfMutations);
    mutate(secondChildString, mergedParentsLength, averageNumberOfMutations);

    // // We decode both children and add them to the next generation
    // decodeAndAddChild(nextGeneration, &currentNextGenerationSize,
    //                   firstChildString, dimensions);
    // decodeAndAddChild(nextGeneration, &currentNextGenerationSize,
    //                   secondChildString, dimensions);
    currentNextGenerationSize++;

  }

}

/**
 * This function takes care of creating the next generation and selecting the
 * parents for the low_memory version of the algorithm
 *
 * @param population  this array stores all the values of the population
 * @param nextGeneration this array is used to store the parametres of the
 * created children
 * @param evaluationFunction this array is used to store the fitness of each
 * solution of the population
 * @param dimensions the number of parameters in the function to minimize
 * @param tournamentSelectionsSize the number of solutions that are selected to
 * be part of the tournament
 * @param averageNumberOfMutations the average number of mutations per child
 */
static void createNextGenerationLowMemory(
    genetic_int* population, genetic_int* nextGeneration,
    fitness_evaluation_function evaluationFunction,
    const unsigned int populationSize, const unsigned int dimensions,
    const unsigned int tournamentSelectionsSize,
    const genetic_real averageNumberOfMutations) {

  unsigned int currentNextGenerationSize = 0;

  const unsigned int nextGenerationMaxSize = populationSize - 2;
  const unsigned int mergedParentsLength = dimensions * INT_MAX_DIGIT_COUNT + 1;

  const unsigned int mergedParentsLastIndex = mergedParentsLength - 1;

  const unsigned int parentArrayByteSize = dimensions * sizeof(*population);

  while (currentNextGenerationSize < nextGenerationMaxSize) {

    unsigned int parent1Number, parent2Number;
    tourneyLowMemory(population, &parent1Number, &parent2Number,
                     evaluationFunction, tournamentSelectionsSize,
                     populationSize, dimensions);

    genetic_int parent1[dimensions];
    genetic_int parent2[dimensions];

    const genetic_int parent1Index = parent1Number * dimensions;
    const genetic_int parent2Index = parent2Number * dimensions;

    memcpy(parent1, population + parent1Index, parentArrayByteSize);
    memcpy(parent2, population + parent2Index, parentArrayByteSize);

    char mergedParents1[mergedParentsLength];
    char mergedParents2[mergedParentsLength];

    char firstChildString[mergedParentsLength];
    char secondChildString[mergedParentsLength];

    firstChildString[mergedParentsLastIndex] = '\0';
    secondChildString[mergedParentsLastIndex] = '\0';

    encode(mergedParents1, parent1, dimensions);
    encode(mergedParents2, parent2, dimensions);

    createChildren(mergedParents1, mergedParents2, firstChildString,
                   secondChildString, dimensions);

    // We apply the mutation operator to both children
    mutate(firstChildString, mergedParentsLength, averageNumberOfMutations);
    mutate(secondChildString, mergedParentsLength, averageNumberOfMutations);

    // We decode both children and add them to the next generation
    decodeAndAddChild(nextGeneration, &currentNextGenerationSize,
                      firstChildString, dimensions);
    decodeAndAddChild(nextGeneration, &currentNextGenerationSize,
                      secondChildString, dimensions);
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
 * @param dimensions the number of parameters in the function to optimize
 * @param populationSize the number of solutions in the population
 */
static void calculateAndStoreFitness(
    genetic_int* population, genetic_real* populationFitness,
    genetic_real* bestFit, genetic_int* bestFitCoord,
    fitness_evaluation_function evaluationFunction,
    genetic_int* secondBestValues, genetic_real* secondBestFitness,
    const unsigned int dimensions, const unsigned int populationSize) {

  // We reset these values to prevent the same value from being chosen twice
  *bestFit = FLT_MAX;
  *secondBestFitness = FLT_MAX;

  const unsigned int coordArrayByteSize = dimensions * sizeof(*population);

  for (unsigned int i = 0; i < populationSize; i++) {

    genetic_real parameters[dimensions];
    const unsigned int baseIndex = i * dimensions;

    for (unsigned int j = 0; j < dimensions; j++) {
      parameters[j] = population[baseIndex + j] * INT_MAX_INVERSE;
    }

    const genetic_real fitness = evaluationFunction(parameters);
    populationFitness[i] = fitness;

    if (fitness < *bestFit) {

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
static void calculateFitness(genetic_int* population, genetic_real* bestFit,
                             genetic_int* bestFitCoord,
                             fitness_evaluation_function evaluationFunction,
                             genetic_int* secondBestValues,
                             genetic_real* secondBestFitness,
                             const unsigned int dimensions,
                             const unsigned int populationSize) {

  // We reset these values to prevent the same value from being chosen twice
  *bestFit = FLT_MAX;
  *secondBestFitness = FLT_MAX;

  const unsigned int coordArrayByteSize = dimensions * sizeof(*population);
  for (unsigned int i = 0; i < populationSize; i++) {

    genetic_real parameters[dimensions];
    const unsigned int baseIndex = i * dimensions;

    for (unsigned int j = 0; j < dimensions; j++) {
      parameters[j] = population[baseIndex + j] * INT_MAX_INVERSE;
    }

    const genetic_real fitness = evaluationFunction(parameters);

    if (fitness < *bestFit) {

      *secondBestFitness = *bestFit;
      *bestFit = fitness;

      memcpy(secondBestValues, bestFitCoord, coordArrayByteSize);
      memcpy(bestFitCoord, population + (i * dimensions), coordArrayByteSize);

    }

    else if (fitness < *secondBestFitness) {

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
 * @param dimensions the number of parameters in the function to optimize
 */
static void
replacePopulation(genetic_int* population, genetic_int* newGeneration,
                  genetic_int* bestFitValues, genetic_int* secondBestValues,
                  const unsigned int arraySize, const unsigned int dimensions) {

  const unsigned int nextGenerationStartingIndex = 2 * dimensions;

  // We copy the elite values before copying the rest of the values from the
  // next generation

  memcpy(population, bestFitValues, dimensions * sizeof(genetic_int));
  memcpy(population + dimensions, secondBestValues,
         dimensions * sizeof(genetic_int));
  memcpy(population + nextGenerationStartingIndex, newGeneration,
         arraySize * sizeof(genetic_int));
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
 * must be smaller than the population size
 * @param evaluationFunction the function that is used to
 * @param lowMemoryMode if set to 1 the algorithm will be slower and call the
 * evaluation function more often
 * @return real the fitness of the best solution
 */
genetic_real
geneticAlgorithm(genetic_real* bestFitValues, const unsigned int parameterCount,
                 const genetic_real epsilon, const genetic_real mutationChance,
                 unsigned int generationSize, unsigned int tourneySize,
                 const unsigned int maximumIterationCount,
                 fitness_evaluation_function evaluationFunction,
                 const unsigned int lowMemoryMode) {

  if (generationSize % 2)
    generationSize += 1;

  if (tourneySize > generationSize)
    tourneySize = generationSize;

  genetic_real averageMutationsPerChromosone =
      INT_MAX_DIGIT_COUNT * mutationChance * parameterCount;

  genetic_real bestFit = FLT_MAX;

  const unsigned int arraySize = generationSize * parameterCount;
  // We created a seperate size because the child array will be two smaller than
  // the population because of the two elite values that are reinjected
  const unsigned int childArraySize = arraySize - 2 * parameterCount;

  genetic_int population[arraySize];
  genetic_int nextGeneration[childArraySize];

  genetic_int bestValues[parameterCount];

  genetic_int secondBestValues[parameterCount];
  genetic_real secondBestFitness = FLT_MAX;

  fillTable(population, generationSize, parameterCount);

  if (lowMemoryMode == 0) {

    genetic_real populationFitness[generationSize];


    for (unsigned int i = 0; i < maximumIterationCount; i++) {

      calculateAndStoreFitness(population, populationFitness, &bestFit,
                               bestValues, evaluationFunction, secondBestValues,
                               &secondBestFitness, parameterCount,
                               generationSize);
      

      createNextGeneration(population, nextGeneration, populationFitness,
                           generationSize, parameterCount , tourneySize,
                           averageMutationsPerChromosone);
      
      

      replacePopulation(population, nextGeneration, bestValues,
                        secondBestValues, childArraySize, parameterCount);

      

      if (bestFit <= epsilon) {
        break;
      }
    }
  } else {
    for (unsigned int i = 0; i < maximumIterationCount; i++) {

      calculateFitness(population, &bestFit, bestValues, evaluationFunction,
                       secondBestValues, &secondBestFitness, parameterCount,
                       generationSize);

      createNextGenerationLowMemory(
          population, nextGeneration, evaluationFunction, generationSize,
          parameterCount, tourneySize, averageMutationsPerChromosone);



      replacePopulation(population, nextGeneration, bestValues,
                        secondBestValues, childArraySize, parameterCount);
      if (bestFit <= epsilon) {
        break;
      }

    }
  }

  for (unsigned int j = 0; j < parameterCount; j++) {
    bestFitValues[j] = bestValues[j] * INT_MAX_INVERSE;
  }
  return bestFit;
}
