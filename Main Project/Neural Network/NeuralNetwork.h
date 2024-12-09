#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Constants
#define INPUT_SIZE 2352  // 28*28*3
#define HIDDEN_SIZE 256
#define OUTPUT_SIZE 26

// Function prototypes
void load_weights(const char* filename, float* weights, int size);
void relu(float* input, int size);
void softmax(float* input, int size);
void load_image(const char* filename, float* image);
char predict_letter(const char* image_file);

#endif // NEURAL_NETWORK_H

