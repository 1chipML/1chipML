#ifndef _MCU_ANN_
#define _MCU_ANN_

/* Define activation function enum. */
typedef enum _activation {relu, softmax} activation;

/* Define layer. */
typedef struct _layer {
  unsigned int num_of_inputs;
  unsigned int num_of_neurons;   // number of neurons in the layers
  double *weights; // pointer to the weights, dimension num_of_inputs X num_of_neurons
  double *biases; // pointer to the biases, dimension num_of_neurons
  activation act_function; // activation function.
  double *output; // pointer to the output, dimension num_of_neurons
} layer;

/* Define network. */
typedef struct _network{
  unsigned int num_of_inputs; // number of inputs
  unsigned int num_of_outputs; // number of outputs
  unsigned int num_of_hidden_layers; // total number of hidden layers
  layer **layers; // layer array
} network;

/*
 * Load NN model from network_weights.h. 
 */
network *load_model();

/*
 * Inference one data sample. 
 */
double *feedforward(network *ann, double *inputs);

/*
 * Caculate argmax. 
 */
unsigned int argmax(double *data, unsigned int size);

/* 
 * Release network memory. 
 */
void release_model(network *ann);

/*
 * Display network.
 */
void display_model(network *ann);

#endif