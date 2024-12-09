
#ifndef AUTOROTATE_H

#define AUTOROTATE_H


#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>

#include <gtk/gtk.h>

// Déclaration des fonctions
void pivoter_image(SDL_Surface* surface, char *fichier, double angle);

int** detecter_lignes(SDL_Surface* surface, int* taille, int* seuil);

void rotation_auto(char* fichier, char* fichier_sortie);


void sauvegarder_rotation();

void mettre_a_jour_image(GtkRange *range);

void creer_fenetre_rotation(char *fichier);


#endif // AUTOROTATE_H

