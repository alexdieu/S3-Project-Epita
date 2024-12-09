#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define M_PI 3.14159265358979323846
#define SEUIL 1000


// Fonction pour appliquer le filtre de Sobel
SDL_Surface* appliquer_filtre_sobel(SDL_Surface* surface) {
    //Algo piqué a wikipedia
    int largeur = surface->w;
    int hauteur = surface->h;
    SDL_Surface* surface_sobel = SDL_CreateRGBSurfaceWithFormat(0, largeur, hauteur, 32, SDL_PIXELFORMAT_ARGB8888);
    if (surface_sobel == NULL) {
   printf("Erreur de création de la surface Sobel: %s\n", SDL_GetError());
          return NULL;
    }
    //Locjer la surface
          SDL_LockSurface(surface);
          
          SDL_LockSurface(surface_sobel);
          //pixelllllll le film
          Uint32* pixels = (Uint32*)surface->pixels;
          Uint32* pixels_sobel = (Uint32*)surface_sobel->pixels;
    
    //OPERA GXXX
    int gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
  };
    //MAtrice trouvé sur wikipedia.org j'adore ce site
    int gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
  };
    
      for (int y = 1; y < hauteur - 1; y++) {
          for (int x = 1; x < largeur - 1; x++) {
               int sumX = 0, sumY = 0; 
          
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            Uint8 r, g, b;
                            SDL_GetRGB(pixels[(y + i) * largeur + (x + j)], surface->format, &r, &g, &b);
                            int gray = 0.299 * r + 0.587 * g + 0.114 * b;

                            sumX += gx[i + 1][j + 1] * gray;
                            sumY += gy[i + 1][j + 1] * gray;
       }
            }

            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            if (magnitude > 255) magnitude = 255;
            if (magnitude < 0) magnitude = 0;

            Uint32 pixel = SDL_MapRGB(surface_sobel->format, magnitude, magnitude, magnitude);
            pixels_sobel[y * largeur + x] = pixel;
        }
    }
    //On debloque comme au chiottes
          SDL_UnlockSurface(surface);
    SDL_UnlockSurface(surface_sobel);

    return surface_sobel;
}

//Ca rotate beiiiiin
void pivoter_image(SDL_Surface* surface, char *fichier, double angle) {
    SDL_Window *fenetre = SDL_CreateWindow("Rotation de l'Image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, surface->w, surface->h, 0);
     if (fenetre == NULL) { printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
         SDL_Quit();
        return;
    }
    //RENDU
    SDL_Renderer *renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_TARGETTEXTURE);
    if (renderer == NULL) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return;
    }
    //MINECRAFT
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        printf("Erreur de création de la texture: %s\n", SDL_GetError());
         SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(fenetre);
         SDL_Quit();
        return;
    }

    //RENDER DISTANCE MAX CHUNKS
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, SDL_FLIP_NONE);

    SDL_Surface* image_surface = SDL_CreateRGBSurfaceWithFormat(0, surface->w, surface->h, 32, SDL_PIXELFORMAT_ARGB8888);
    if (image_surface == NULL) {
                printf("Erreur de création de la surface: %s\n", SDL_GetError());
                SDL_DestroyTexture(texture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(fenetre);
                SDL_Quit();
                return;
            }

    SDL_RenderReadPixels(renderer, NULL, image_surface->format->format, image_surface->pixels, image_surface->pitch);

    if (IMG_SavePNG(image_surface, fichier) != 0) {
        printf("Erreur de sauvegarde de l'image: %s\n", IMG_GetError());
    } else {
        printf("L'image a été pivotée avec succès de %d° et sauvegardée sous '%s'.\n", (int)angle, fichier);
    }

    SDL_FreeSurface(image_surface);
      SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(fenetre);
    SDL_Quit();
}

// Fonction pour détecter les lignes dans l'image
int** detecter_lignes(SDL_Surface* surface, int* taille, int* seuil) {
    int largeur = surface->w - 2;
     int hauteur = surface->h - 2;
    *taille = (int)sqrt(pow((double)hauteur, 2) + pow((double)largeur, 2)) + 1;
    Uint32* pixels = (Uint32*)surface->pixels;
          double** Canicule = malloc(hauteur * sizeof(double*));
          //Y FAIT CHAUD BOUD DE DIEU
         for (int y = 0; y < hauteur; y++) { // CHAUD CACAO
                  Canicule[y] = malloc(largeur * sizeof(double));
            for (int x = 0; x < largeur; x++) {
            Uint8 r, g, b; // G AIME L ESPAGNE
               SDL_GetRGB(pixels[y * largeur + x], surface->format, &r, &g, &b);
             Canicule[y][x] = 0.299 * r + 0.587 * g + 0.114 * b;
        }
    }
    //LA CAF VOUS REMERCIE
      int** aidessociales = malloc((*taille) * 2 * sizeof(int*));
     for (int i = 0; i < (*taille) * 2; i++) {
          aidessociales[i] = malloc(180 * sizeof(int));
        for (int j = 0; j < 180; j++) {
            aidessociales[i][j] = 0; // CA C EST 0 MON COCO
        }
    }
 
     for (int y = 0; y < hauteur; y++) {
          for (int x = 0; x < largeur; x++) {
              if (Canicule[y][x] <= 50) continue;
              for (int theta = 0; theta < 180; theta++) {
                int rho = x * cos(theta * M_PI / 180) + y * sin(theta * M_PI / 180);
                  if (++aidessociales[rho + *taille][theta] > *seuil) {
                    *seuil = aidessociales[rho + *taille][theta];
                }
              }
        }
    }

    for (int i = 0; i < hauteur; i++) {
        free(Canicule[i]);
    }
    free(Canicule);
    return aidessociales;
}

// Fonction pour détecter automatiquement la rotation optimale
void rotation_auto(char* fichier, char* fichier_sortie) {
    if (fichier == NULL || fichier_sortie == NULL) {
        puts("Chemin de fichier invalide");
        return;
    }
    //SURFACE DE L4EAAAAUUUU
    SDL_Surface* surface = IMG_Load(fichier);
    if (surface == NULL) {
    
        printf("Erreur de chargement de l'image: %s\n", IMG_GetError());
 SDL_Quit();
        
        return;
    }

    // Appliquer le filtre de Sobel pour détecter les contours
    SDL_Surface* surface_sobel = appliquer_filtre_sobel(surface);
    if (surface_sobel == NULL) {
    
    
        SDL_FreeSurface(surface);
        
        
        return;
    }

    // Détecter les lignes dans l'image avec le filtre de Sobel
    int taille = 0, seuil = SEUIL;
    int** lignedeviedejeanmichelapathie = detecter_lignes(surface_sobel, &taille, &seuil);
    seuil /= 2; // SEUIL

    int max_votes = 0, angle_detecté = 0; //G voté le Z
    for (int angle = 0; angle < 180; angle++) {
        int votes = 0;
        for (int rho = 0; rho < taille * 2; rho++) {
        
            if (lignedeviedejeanmichelapathie[rho][angle] > seuil) {
                votes++;
            
        }
            
            
        }
        if (votes > max_votes) {
            max_votes = votes;
            angle_detecté = angle;
        }
}

    // Libérer les ressources
 for (int rho = 0; rho < taille * 2; rho++) {
       
       
       free(lignedeviedejeanmichelapathie[rho]);
        
        
    }
    free(lignedeviedejeanmichelapathie);
    SDL_FreeSurface(surface_sobel);

    // Appliquer la rotation sur l'image d'origine
      if (angle_detecté % 90 != 0) {
          if (angle_detecté > 90) {
               pivoter_image(surface, fichier_sortie, 98 - angle_detecté);
        } else {
   pivoter_image(surface, fichier_sortie, -angle_detecté);
          }
     } else {
           pivoter_image(surface, fichier_sortie, 0); // Pas de rotation nécessaire, mais sauvegarder le fichier de sortie
     }
 
     SDL_FreeSurface(surface);//FRRRRRRRRRRRRRRRRRRREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEERRRRE
 }

