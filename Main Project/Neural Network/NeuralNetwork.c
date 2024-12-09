#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INPUT_SIZE 2352  // 28*28*3
#define HIDDEN_SIZE 256
#define OUTPUT_SIZE 26
//La tete du chef
void load_weights(const char* filename, float* weights, int size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "PTN TROUVER PAS FICHIER OUGA OUGA: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++) {
        if (fscanf(file, "%f", &weights[i]) != 1) {
            fprintf(stderr, "G pas lu: %s\n", filename);
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}
//RELOU SANS O
void relu(float* input, int size) {
    for (int i = 0; i < size; i++) {
        input[i] = input[i] > 0 ? input[i] : 0;
    }
}
//DYNAMX CHARIZARD VASY
void softmax(float* input, int size) {
    float max = input[0];
    for (int i = 1; i < size; i++) {
        if (input[i] > max) {
            max = input[i];
        }
    }
    float sum = 0.0;
    for (int i = 0; i < size; i++) {
        input[i] = exp(input[i] - max);
        sum += input[i];
    }
    for (int i = 0; i < size; i++) {
        input[i] /= sum;
    }
}
//Illisible ce ptn de code
//Charge el imagio dans el repertorio
void load_image(const char* filename, float* image) {
    SDL_Surface* loaded_image = IMG_Load(filename);
    if (loaded_image == NULL) {
        fprintf(stderr, "Charge cette ptn d'ilage: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Surface* formatted_image = SDL_ConvertSurfaceFormat(loaded_image, SDL_PIXELFORMAT_RGB24, 0);
    if (formatted_image == NULL) {
        fprintf(stderr, "ARRIVE PAS A CONVERTIR COMMENT C POSSIBLE JE SUIS CHRETIEN MOI %s\n", SDL_GetError());
        SDL_FreeSurface(loaded_image);
        exit(EXIT_FAILURE);
    }
    SDL_Surface* resized_image = SDL_CreateRGBSurface(0, 28, 28, 24, 0, 0, 0, 0);
    if (resized_image == NULL) {
        fprintf(stderr, "CA VEUX PAS CREER JSP POIRQUOI: %s\n", SDL_GetError());
        SDL_FreeSurface(formatted_image);
        SDL_FreeSurface(loaded_image);
        exit(EXIT_FAILURE);
    }
    SDL_BlitScaled(formatted_image, NULL, resized_image, NULL);
    SDL_LockSurface(resized_image);
    Uint8* pixels = (Uint8*)resized_image->pixels;
    for (int y = 0; y < 28; y++) {
        for (int x = 0; x < 28; x++) {
            int index = (y * 28 + x) * 3;
            image[index] = pixels[index] / 255.0;
            image[index + 1] = pixels[index + 1] / 255.0;
            image[index + 2] = pixels[index + 2] / 255.0;
        }
    }
    SDL_UnlockSurface(resized_image);

    SDL_FreeSurface(resized_image);
    SDL_FreeSurface(formatted_image);
    SDL_FreeSurface(loaded_image);
}

char predict_letter(const char* image_file) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL DE MORT QUI CRASH MANQUAIT PLUS QUE CA A 2H DU MATHS: %s\n", SDL_GetError());
        return '\0';
    }

    float hidden_weights[INPUT_SIZE * HIDDEN_SIZE];
    float hidden_biases[HIDDEN_SIZE];
    float output_weights[HIDDEN_SIZE * OUTPUT_SIZE];
    float output_biases[OUTPUT_SIZE];

    load_weights("IADATA/weights.txt", hidden_weights, INPUT_SIZE * HIDDEN_SIZE);
    load_weights("IADATA/hidden_biases.txt", hidden_biases, HIDDEN_SIZE);
    load_weights("IADATA/output_weights.txt", output_weights, HIDDEN_SIZE * OUTPUT_SIZE);
    load_weights("IADATA/output_biases.txt", output_biases, OUTPUT_SIZE);

    float input_image[INPUT_SIZE];
    load_image(image_file, input_image);

    float hidden_output[HIDDEN_SIZE] = {0};
    float output[OUTPUT_SIZE] = {0};

    for (int i = 0; i < HIDDEN_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            hidden_output[i] += input_image[j] * hidden_weights[j * HIDDEN_SIZE + i];
        }
        hidden_output[i] += hidden_biases[i];
    }
    relu(hidden_output, HIDDEN_SIZE);

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            output[i] += hidden_output[j] * output_weights[j * OUTPUT_SIZE + i];
        }
        output[i] += output_biases[i];
    }
    softmax(output, OUTPUT_SIZE);
//PREDIS MOI TOUT CA
                                                          int predicted_class = 0;
                                                          float max_prob = output[0];
                                                          for (int i = 1; i < OUTPUT_SIZE; i++) {
                                                              if (output[i] > max_prob) {
                                                                  max_prob = output[i];
                                                                  predicted_class = i;
                                                              }
                                                          }

    SDL_Quit();

    return 'A' + predicted_class;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image du fichier>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* image_file = argv[1];

    // azy je vais deviner la lettre c Z
    char predicted_letter = predict_letter(image_file);
    printf("%c\n", predicted_letter);

    return EXIT_SUCCESS;
}
