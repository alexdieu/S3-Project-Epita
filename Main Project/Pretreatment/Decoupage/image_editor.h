#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>

#include <stdio.h>

#include <stdlib.h>

#include <sys/stat.h>

#include <sys/types.h>

#include <dirent.h>

#include <unistd.h>

#include <string.h>

#include <math.h>

#include <limits.h>

#define CONFIGS_FOLDER "configs/"

// Structures
typedef struct {
    int x, y;
    int longueur, largeur;
} Rectangle;

typedef struct RectangleNode {
    Rectangle rect;
    int y;
    struct RectangleNode *prev;
    struct RectangleNode *next;
} RectangleNode;

// Function prototypes BECAUOUP TROP OUI
void convert_png_to_ini(char *filename); // SERT A QUE DALLE
void convert_ini_to_png(char *filename); // PAREIL
int file_exists(const char *filename); // NOM EXPLICITE +18
void generate_ini_for_image(const char *filename); //Ca c'est utile
void generate_config_ini(int avg_length, int avg_width, int min_length, int max_length, int min_width, int max_width); //Y a vraiment besoin d'expliquer ce nom
void calculate_step_size(RectangleNode *head, int *step_size); // STEP SISTER HELLP ME
void convertToGrayscale(SDL_Surface *surface); // Du noir , je broie du noir
int isBlack(SDL_Surface *surface, int x, int y); // c noir
int isVisited(SDL_Surface *surface, int x, int y); // c visité theo je peux venir chez toi vendredi soir ? STPPPPPP / Niet previet -theo
void findObject(SDL_Surface *surface, int x, int y, int *minX, int *maxX, int *minY, int *maxY); // TROUVE
void drawRectangle(SDL_Surface *surface, Rectangle rect, Uint32 color); // DESSINE
int isOverlappingOrClose(Rectangle a, Rectangle b); // OVERLAPPPPPPPPPPPP (si ils sont plus ou moins en collision)
RectangleNode* sortedInsert(RectangleNode* head, RectangleNode* newNode); //Noeuds
RectangleNode* sortList(RectangleNode* head); // A LA
RectangleNode* createNode(Rectangle rect, int y); //TETE PTN CES FONCTIONS DE MORT
void insertNode(RectangleNode **head, Rectangle rect, int y); // INSERER 
void calculate_dimensions(RectangleNode *head, int *avg_length, int *avg_width, int *min_length, int *max_length, int *min_width, int *max_width); //*calcule calcule*
void saveObject(SDL_Surface *image, int minX, int minY, int maxX, int maxY, const char *filename); // LEOPOLD PTN LES ;
void createTempDirectory(); // Explicite +21
void read_ini_file(const char *filename); // LIS MOI CA VASYs
int use_config_if_exists(const char *filename); // if SEURLEUTMENT SI
void process_image(const char *input_filename, const char *optional_ini_filename, const char *output_dir); // Process c le main quoi

#endif // PROCESS_IMAGE_H

