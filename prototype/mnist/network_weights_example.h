#ifndef _WEIGHTS_
#define _WEIGHTS_

#define NUM_OF_INPUTS 4
#define NUM_OF_OUTPUTS 2
#define NUM_OF_HIDDEN_LAYERS 2

/* 4 rows 2 columns {1, 2} is first row */
double layer_1_weights[] = {0.11, 0.21, 0.31, 0.41, 0.51, 0.61, 0.71, -0.81}; 
double layer_1_biases[] = {0.1, 0.2};
unsigned int layer_1_neuron = 2;

double layer_2_weights[] = {0.12, 0.22, 0.32, 0.42}; 
double layer_2_biases[] = {0.3, 0.4};
unsigned int layer_2_neuron = 2;

double layer_3_weights[] = {0.43, 0.33, 0.23, 0.13}; 
double layer_3_biases[] = {0.5, 0.6};
unsigned int layer_3_neuron = NUM_OF_OUTPUTS;

double *weights[] = {layer_1_weights, layer_2_weights, layer_3_weights};
double *biases[] = {layer_1_biases, layer_2_biases, layer_3_biases};
unsigned int neurons[] = {2, 2, NUM_OF_OUTPUTS};

/* Data below just for unit test. */
double inputs[] = {0.1, 0.2, 0.3, 0.4}; // Result 1
// double inputs[] = { 0.8, 0.6, 0.2, 0.1}; // Result 0
unsigned int label = 1;

#endif
