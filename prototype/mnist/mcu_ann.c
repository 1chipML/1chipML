#include "mcu_ann.h"
#include "network_weights.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*
 * Load NN model from network_weights.h. 
 */
network *load_model() {
  /* Create and initialize network. */
  network *ann = (network *)malloc(sizeof(network));

  ann->num_of_inputs = NUM_OF_INPUTS;
  ann->num_of_outputs = NUM_OF_OUTPUTS;
  ann->num_of_hidden_layers = NUM_OF_HIDDEN_LAYERS;
  ann->layers = (layer **)malloc(sizeof(layer *) * (NUM_OF_HIDDEN_LAYERS + 1));

  /* Define 1st hidden layer */
  layer *layer1 = (layer *)malloc(sizeof(layer));
  layer1->num_of_inputs = NUM_OF_INPUTS;
  layer1->num_of_neurons = neurons[0];
  layer1->act_function = relu;
  int layer1_weights_size = layer1->num_of_inputs * layer1->num_of_neurons;
  layer1->weights = (double *)malloc(layer1_weights_size * sizeof(double));
  memcpy(layer1->weights , weights[0], layer1_weights_size * sizeof(double));
  layer1->biases = (double *)malloc(layer1->num_of_neurons * sizeof(double));
  memcpy(layer1->biases, biases[0], layer1->num_of_neurons * sizeof(double));
  layer1->output = (double *)malloc(layer1->num_of_neurons * sizeof(double));
  ann->layers[0] = layer1;

  /* Define other hidden layers */
  for (int i=1; i<ann->num_of_hidden_layers; i++) {
    layer *the_layer = (layer *)malloc(sizeof(layer));
    the_layer->num_of_inputs = neurons[i-1];
    the_layer->num_of_neurons = neurons[i];
    
    int layer_weights_size = the_layer->num_of_inputs * the_layer->num_of_neurons * sizeof(double);
    the_layer->weights = (double *)malloc(layer_weights_size);
    memcpy(the_layer->weights , weights[i], layer_weights_size);
    the_layer->biases = (double *)malloc(the_layer->num_of_neurons * sizeof(double));
    memcpy(the_layer->biases, biases[i], the_layer->num_of_neurons * sizeof(double));
    the_layer->output = (double *)malloc(the_layer->num_of_neurons * sizeof(double));

    the_layer->act_function = relu;
    
    ann->layers[i] = the_layer;
  }

  /* Define output layers */
  layer *output_layer = (layer *)malloc(sizeof(layer));
  output_layer->num_of_inputs = neurons[NUM_OF_HIDDEN_LAYERS-1];
  output_layer->num_of_neurons = NUM_OF_OUTPUTS;
  int output_layer_weights_size = 
      output_layer->num_of_inputs * output_layer->num_of_neurons * sizeof(double);
  output_layer->weights = (double *)malloc(output_layer_weights_size);
  memcpy(output_layer->weights , weights[NUM_OF_HIDDEN_LAYERS], output_layer_weights_size);
  output_layer->biases = (double *)malloc(output_layer->num_of_neurons * sizeof(double));
  memcpy(output_layer->biases, biases[NUM_OF_HIDDEN_LAYERS], 
        output_layer->num_of_neurons * sizeof(double));
  output_layer->output = (double *)malloc(output_layer->num_of_neurons * sizeof(double));
  output_layer->act_function = softmax;
  ann->layers[NUM_OF_HIDDEN_LAYERS] = output_layer;

  /* Return network */
  return ann; 

}

/*
 * Activation function. 
 */
void activation_func(layer *layer) {
  printf("Input of activation function:\n");
  for (unsigned int i = 0; i<layer->num_of_neurons; i++)
      printf("%f ", layer->output[i]);
  printf("\n");

  if (layer->act_function == relu) {
    for (unsigned int n = 0; n < layer->num_of_neurons; n++)
      if (layer->output[n] < 0) layer->output[n] = 0;
  } else if (layer->act_function == softmax) {
    double sum = 0.;
    for (unsigned int n = 0; n < layer->num_of_neurons; n++) {
      sum += exp(layer->output[n]);
    }
    for (unsigned int n = 0; n < layer->num_of_neurons; n++) {
      layer->output[n] = layer->output[n] / sum;
    }
  } else {
    /* Default is linear */
  }

  printf("Output of activation function:\n");
  for (unsigned int i = 0; i<layer->num_of_neurons; i++)
      printf("%f ", layer->output[i]);
  printf("\n");
}

/*
 * Inference one data sample. 
 */
double *feedforward(network *ann, double *inputs) {
  /* scan all the layers */
  for (unsigned int l = 0; l < ann->num_of_hidden_layers+1; l++){
    double *previous_output = l == 0 ? inputs : ann->layers[l-1]->output;

    /* scan all the neurons in layer 'l' */
    for (unsigned int n = 0; n < ann->layers[l]->num_of_neurons; n++) {
      double y = 0.;
      /* linear product between w[] and x[] + b */
      for (unsigned int i = 0; i < ann->layers[l]->num_of_inputs; i++) {
        // printf("%f x %f ", previous_output[i], ann->layers[l]->weights[i*ann->layers[l]->num_of_neurons+n]);
        y += previous_output[i] * ann->layers[l]->weights[i*ann->layers[l]->num_of_neurons+n]; 
      }
      // printf("\n");
      ann->layers[l]->output[n] = y + ann->layers[l]->biases[n];
    }

    /* Apply activation function on y. */
    activation_func(ann->layers[l]);

  }
  return ann->layers[ann->num_of_hidden_layers]->output;
}

/*
 * Caculate argmax. 
 */
unsigned int argmax(double *data, unsigned int size) {
  for (unsigned int i = 0; i<size; i++)
      printf("%f ", data[i]);
  printf("\n");

  double max = data[0];
  unsigned int result = 0;
  for (unsigned int i=1; i<size; i++) {
    if (data[i] > max) {
      max = data[i]; 
      result = i;
    }   
  }
  return result;
}

/* 
 * Release network memory. 
 */
void release_model(network *ann) {
  for (unsigned int l = 0; l < ann->num_of_hidden_layers+1; l++){
    free(ann->layers[l]->output);
    free(ann->layers[l]->weights);
    free(ann->layers[l]->biases);
    free(ann->layers[l]);
  }
  free(ann->layers);
}

/*
 * Display network.
 */
void display_model(network *ann) {

  printf("network summary:\n");
  printf("num_of_inputs = %d\n", ann->num_of_inputs);
  printf("num_of_outputs = %d\n", ann->num_of_outputs);
  printf("num_of_hidden_layers = %d\n", ann->num_of_hidden_layers);
  printf("num_of_output_layer = 1\n");
  for (unsigned int i=0; i<ann->num_of_hidden_layers+1; i++) {
    if (i == ann->num_of_hidden_layers) {
      printf("\noutput layer\n");
    } else {
      printf("\nhidden layer %d\n",  i);
    }

    printf("num_of_inputs = %d\n",  ann->layers[i]->num_of_inputs);
    printf("num_of_neurons = %d\n",  ann->layers[i]->num_of_neurons);
    printf("activation function = %d (0: relu, 1:softmax)\n",  ann->layers[i]->act_function);
    
    double *layer_weights = ann->layers[i]->weights;
    double *layer_biases = ann->layers[i]->biases;

    printf("weights:\n");
    for (unsigned int r=0; r<ann->layers[i]->num_of_inputs; r++) {
        for (unsigned int c = 0; c<ann->layers[i]->num_of_neurons; c++)
            printf("%f ", layer_weights[r * ann->layers[i]->num_of_neurons + c]);
        printf("\n");
    }

    printf("biases:\n");
    for (unsigned int c = 0; c<ann->layers[i]->num_of_neurons; c++)
      printf("%f ", layer_biases[c]);
    printf("\n");
  }
  printf("\n");
}

/*
 * Normalize input data.
 */
void normalize(double *inputs, int size) {
  printf("Inputs:\n");
  for (unsigned int i = 0; i < size; i++) {
    printf("%f ", inputs[i]);
    // inputs[i] = inputs[i] / 255;
  }
  printf("\n");
}

/*
 * Main function.
 * Compile: gcc mcu_ann.c -lm -std=c99 -Wall -Ofast
 * Run: ./a.out
 */
int main(int argc,char *argv[]) {
  network *nn = load_model();

  // display_model(nn);

  normalize(test_2_input, 784);

  double *outuputs = feedforward(nn, test_2_input);

  unsigned int result = argmax(outuputs, nn->num_of_outputs);

  printf("predict = %d\n", result);
  printf("label = %d\n", test_2_label);
 
  release_model(nn);

}