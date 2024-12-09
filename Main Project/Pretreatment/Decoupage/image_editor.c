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

//PI GOOGLE .COM C4EST LUI
#define M_PI 3.14159265358979323846
//config gnéreré dedans
#define CONFIGS_FOLDER "configs/"

//Params de base de l'aGLOPOOO
int tolerance = 200;
int min_length = 10;
int max_length = 30;
int min_heigth = 10;
int max_heigth = 30;
int step_size = 10;
int overlapping_tolerance = 5;
//La on lis des bails de configs
void read_ini_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
	printf("hello\n");
        file = fopen(filename, "w");
        fprintf(file, "TOLERANCE : %i\n", tolerance);
        fprintf(file, "MIN_LENTGH : %i\n", min_length);
        fprintf(file, "MAX_LENTGH : %i\n", max_length);
        fprintf(file, "MIN_HEIGTH : %i\n", min_heigth);
        fprintf(file, "MAX_HEIGTH : %i\n", max_heigth);
        fprintf(file, "STEP_SIZE : %i\n", step_size);
        fprintf(file, "OVERLAPPING_TOLERANCE : %i\n", overlapping_tolerance);
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[50];
        int value;

        if (sscanf(line, "%49s : %d", key, &value) == 2) {
            if (strcmp(key, "TOLERANCE") == 0) {
                tolerance = value;
            } else if (strcmp(key, "MIN_LENTGH") == 0) {
                min_length = value;
            } else if (strcmp(key, "MAX_LENTGH") == 0) {
                max_length = value;
            } else if (strcmp(key, "MIN_HEIGTH") == 0) {
                min_heigth = value;
            } else if (strcmp(key, "MAX_HEIGTH") == 0) {
                max_heigth = value;
            } else if (strcmp(key, "STEP_SIZE") == 0) {
                step_size = value;
            } else if (strcmp(key, "OVERLAPPING_TOLERANCE") == 0) {
                overlapping_tolerance = value;
            }
        }
    }

    fclose(file);
}

//Nettoie le dossier temp dans lequel vont nos images
void resetTempDirectory(const char *path) {
      DIR *dir = opendir(path);
      struct dirent *entry;
      char filePath[256];
        //If ?????????
        if (dir != NULL) {
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    if (strlen(path) + strlen(entry->d_name) + 2 <= sizeof(filePath)) {
                        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);
                    } else {
                        fprintf(stderr, "Error: Path too long for buffer.\n");
                        continue;
                    }
                    remove(filePath);
            }    }
    closedir(dir);
}
}

//créer le dossier temp
void createTempDirectory() {
    struct stat st = {0};
    if (stat("temp", &st) == -1) {
        mkdir("temp", 0700);
    } else {
        resetTempDirectory("temp");
    }
}

//structure rectangle
typedef struct {
    int x, y;
    int longueur, largeur;
} Rectangle;

//stocke le y auquel le rectangle a été trouvé + RATIO +
//aussi le rectangle suivant
typedef struct RectangleNode {
    Rectangle rect;
    int y;
    struct RectangleNode *prev;
    struct RectangleNode *next;
} RectangleNode;
//Rectangle + char 
typedef struct { 
Rectangle rect; 
char ch; 
} CharRect;
//dessine la ligne 3 DE LARGUEUR 
void drawLine(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 color) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;// Algo de machin chose 

    for (int i = -1; i <= 1; i++) { // 3 PTN DE PIXELS CT CA LE PROBLEME JE VAS ME TUER AAAAAAAAAAAAAAAAAAAAAAAAAAAAH
        int x1w = x1 + i;
        int y1w = y1 + i;
        int x2w = x2 + i;
        int y2w = y2 + i;
        int errw = err;
        
        for (int j = 0; j < 1500; j++) { // Keep block at 1500 iterations
            // Set the pixel
            if (x1w >= 0 && x1w < surface->w && y1w >= 0 && y1w < surface->h) {
                Uint32 *pixels = (Uint32 *)surface->pixels;
                pixels[(y1w * surface->w) + x1w] = color;
            }
            // Si on a atteint on se casse 
            if (x1w == x2w && y1w == y2w) {
                break;
            }
                      e2 = errw;
                      if (e2 > -dx) {
                          errw -= dy;
                          x1w += sx;
                      }
                      if (e2 < dy) {
                          errw += dx;
                          y1w += sy;
                      }
            //BISMILLAH
            if (j == 2000) {
                //C EN DUR DANS LE CODE Y4A FORCEMENT UN PB SI ON DEPASSE CA
                printf("PROBLEMES DE COORDONNEES");
                break;
            }
        }
    }
}


void drawLineBetweenRects(SDL_Surface *surface, Rectangle rect1, Rectangle rect2, Uint32 color) {
    // CCentre des rectangles moi je connauis que le centre des AA : alcooliques anonymes
    int centerX1 = rect1.x + rect1.longueur / 2;
    int centerY1 = rect1.y + rect1.largeur / 2;
    int centerX2 = rect2.x + rect2.longueur / 2;
    int centerY2 = rect2.y + rect2.largeur / 2;

    // g bu du rhum cette soiréééeee laaaaaaaa
    drawLine(surface, centerX1, centerY1, centerX2, centerY2, color);
}

//pr créer la liste chainé de rectangles
RectangleNode* createNode(Rectangle rect, int y) {
    RectangleNode *newNode = (RectangleNode*)malloc(sizeof(RectangleNode));
    //Malloc j'en ai fait des cauchemar j'ai des
    //flash de la guerre avec ADRESSE SANITIZER
    newNode->rect = rect;
    newNode->y = y;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

//insere un rectangle dans la chaine
void insertNode(RectangleNode **head, Rectangle rect, int y) {
    RectangleNode *newNode = createNode(rect, y);
    if (*head == NULL) {
        *head = newNode;
    } else {
        //si ca marche ca marche cherche pas trop
          RectangleNode *temp = *head;
          while (temp->next != NULL) {
              temp = temp->next;
          }
          temp->next = newNode;
          newNode->prev = temp;
    }
}

//Un peu explicite le nom nan ?
int isBlack(SDL_Surface *surface, int x, int y) {
    Uint32 pixel = ((Uint32 *)surface->pixels)[(y * surface->w) + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    return (r < tolerance && g < tolerance && b < tolerance);
}

//J'ai toujours préféré le cinéma couleur d'facon
void convertToGrayscale(SDL_Surface *surface) {
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[(y * surface->w) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = 0.3 * r + 0.59 * g + 0.11 * b;
            pixel = SDL_MapRGB(surface->format, gray, gray, gray);
            ((Uint32 *)surface->pixels)[(y * surface->w) + x] = pixel;
        }
    }
}
//Visité = Gris 
int isVisited(SDL_Surface *surface, int x, int y) {
    Uint32 pixel = ((Uint32 *)surface->pixels)[(y * surface->w) + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    return (r == 128 && g == 128 && b == 128);
}

//WAF WAF CHERCHE CHIEN ROBOT CHERCHE
void findObject(SDL_Surface *surface, int x, int y, int *minX, int *maxX, int *minY, int *maxY) {
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;
    if (!isBlack(surface, x, y) || isVisited(surface, x, y)) return;
    ((Uint32 *)surface->pixels)[(y * surface->w) + x] = SDL_MapRGB(surface->format, 128, 128, 128);
    if (x < *minX) *minX = x;
    if (x > *maxX) *maxX = x;
    if (y < *minY) *minY = y;
    if (y > *maxY) *maxY = y;
    findObject(surface, x+1, y, minX, maxX, minY, maxY);
    findObject(surface, x-1, y, minX, maxX, minY, maxY);
    findObject(surface, x, y+1, minX, maxX, minY, maxY);
    findObject(surface, x, y-1, minX, maxX, minY, maxY);
    findObject(surface, x+1, y+1, minX, maxX, minY, maxY);
    findObject(surface, x-1, y-1, minX, maxX, minY, maxY);
    findObject(surface, x+1, y-1, minX, maxX, minY, maxY);
    findObject(surface, x-1, y+1, minX, maxX, minY, maxY);
}
//Visuel
void drawRectangle(SDL_Surface *surface, Rectangle rect, Uint32 color) {
    int startX = rect.x - 1;
    int startY = rect.y - 1;
    int endX = rect.x + rect.longueur;
    int endY = rect.y + rect.largeur;

    for (int x = startX; x <= endX; x++) {
        ((Uint32 *)surface->pixels)[(startY * surface->w) + x] = color;
        ((Uint32 *)surface->pixels)[(endY * surface->w) + x] = color;
    }
    for (int y = startY; y <= endY; y++) {
        ((Uint32 *)surface->pixels)[(y * surface->w) + startX] = color;
        ((Uint32 *)surface->pixels)[(y * surface->w) + endX] = color;
    }
}
//Des zooms et des Cuts (ref de 2000 ca)
void saveObject(SDL_Surface *surface, int minX, int minY, int maxX, int maxY, const char *filename) {
    int longueur = maxX - minX;
    int largeur = maxY - minY;
    if (longueur <= 0 || largeur <= 0) {
        printf("Error: INVALID dimensions for saveObject: longueur=%d, largeur=%d\n", longueur, largeur);
        return;
    }
    printf("CUTTING THROUGH: minX=%d, minY=%d, maxX=%d, maxY=%d, longueur=%d, largeur=%d\n", minX, minY, maxX, maxY, longueur, largeur);
    SDL_Rect srcRect = {minX, minY, longueur, largeur};
    SDL_Surface *object = SDL_CreateRGBSurface(0, longueur, largeur, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!object) {
        printf("ERRORR: SDL_CreateRGBSurface FAIL NOOOOOOO: %s\n", SDL_GetError());
        return;
    }
    if (SDL_BlitSurface(surface, &srcRect, object, NULL) != 0) {
        printf("ERRORR: SDL_BlitSurface normaly doesn't appear anymore: %s\n", SDL_GetError());
        SDL_FreeSurface(object);
        return;
    }
    if (IMG_SavePNG(object, filename) != 0) {
        printf("ERRORR: IMG_SavePNG errno: %s\n", IMG_GetError());
    }
    SDL_FreeSurface(object);
}
//chevauche OU ALORS si c'est proche
int isOverlappingOrClose(Rectangle a, Rectangle b) {
    return (abs(a.x + a.longueur - b.x) <= overlapping_tolerance);
}

//On trie cette liste chainé JE HAIS BUBBLE SORT
RectangleNode* sortedInsert(RectangleNode* head, RectangleNode* newNode) {
    if (head == NULL || head->y > newNode->y || (head->y == newNode->y && head->rect.x > newNode->rect.x)) {
        newNode->next = head;
        if (head != NULL) {
            head->prev = newNode;
        }
        return newNode;
    }
    RectangleNode *current = head;
    while (current->next != NULL && (current->next->y < newNode->y || (current->next->y == newNode->y && current->next->rect.x < newNode->rect.x))) {
        current = current->next;
    }
    newNode->next = current->next;
    if (current->next != NULL) {
        current->next->prev = newNode;
    }
    current->next = newNode;
    newNode->prev = current;
    return head;
}
//TRIE TOUT 

RectangleNode* sortList(RectangleNode* head) {
    RectangleNode *sorted = NULL;
    RectangleNode *current = head;

    while (current != NULL) {
        RectangleNode *next = current->next;
        current->prev = current->next = NULL;
        sorted = sortedInsert(sorted, current);
        current = next;
    }

    printf("Tried list:\n");
    RectangleNode *temp = sorted;
    int lastY = -1;
    while (temp != NULL) {
        if (temp->y != lastY) {
            printf("\n--- NEW Y: %d ---\n", temp->y);
            lastY = temp->y;
        }
        printf("Rect: x=%d, y=%d, largeur=%d, longueur=%d\n", temp->rect.x, temp->y, temp->rect.largeur, temp->rect.longueur);
        temp = temp->next;
    }
    return sorted;
}
//PNG TO INI c trs simple 
void convert_png_to_ini(char *filename) {
  char *pos = strstr(filename, ".png");
  if (pos) {
    strcpy(pos, ".ini");
  }
}
// INI TO PNG
void convert_ini_to_png(char *filename) {
  char *pos = strstr(filename, ".ini");
  if (pos) {
    strcpy(pos, ".png");
  }
}
//CA EXISTE ALORS HEIN DIS LA VERITAS
int file_exists(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file) {
    fclose(file);
    return 1;
  }
  return 0;
}
//Encore la commmande pour générer la config du fichier quoi
void generate_ini_for_image(const char *filename) {
  char command[256];
  snprintf(command, sizeof(command), "ini %s", filename); 
  system(command); 
}
//TOURNE LE RESEAUX DE NEURONESSSS
//G en ai plus qu'un seul qui me sert à boire
char runNeuralNetwork(const char* filename) {
    char command[512];
    snprintf(command, sizeof(command), "./NeuralNetwork %s", filename);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "LE reseaux de neurones est trop con\n");
        exit(EXIT_FAILURE);
    }
    // APPAREMMENT C4EST CE CHARACTERE LA
    char predictedChar;
    fscanf(fp, " %c", &predictedChar);
    pclose(fp);
    //FIABILITE 99% MESSIEURS
    return predictedChar;
}

void linefix(int cols, CharRect row[cols]) {
    // Répare la ligne en longueur ces ptn de '\0'
    CharRect temp[cols];
    int tempIndex = 0;

    // si y a un pb on va le trouver COLONEL
    for (int i = 0; i < cols; i++) {
        if (row[i].ch != '\0') {
            temp[tempIndex++] = row[i];
        }
    }
    // NULLLLLLLLL GERMAIN NULLLLL
              for (int i = tempIndex; i < cols; i++) {
                  temp[i].ch = '\0';
              }
    // ptn cette commande de mort nique la q_7 de l'exam
    memcpy(row, temp, cols * sizeof(CharRect));
}

void printGrid(int rows, int cols, CharRect grid[rows][cols], const char* gridName) {
    printf("%s :\n", gridName);
    // cree le nom du fichier
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.txt", gridName);
    
    // Ouvre en mode w pour Wagon non writing
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Peut pas ouvrir le fichier: %s\n", filename);
        return;
    }
    //TROUVE PERMET DE SAVOIR SI LA LIGNE EST NON VIDE
    int found = 0;
    for (int i = 0; i < rows; i++) {
        found = 0;
        linefix(cols, grid[i]);
        for (int j = 0; j < cols; j++) {
            if (grid[i][j].ch != '\0') {
                found = 1;
                printf("%c", grid[i][j].ch);
                fprintf(file, "%c", grid[i][j].ch);  // Write to file
            }
        }
        if (found) {
            printf("\n");
            fprintf(file, "\n");  // Ecris dans le fichier
        }
    }
    // ferme moi ca
    fclose(file);
}

void printGridCorrected(int rows, int cols, CharRect grid[rows][cols], CharRect correctedGrid[rows][cols], const char* gridName) {
    printf("%s :\n", gridName);

    // tab de nom
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.txt", gridName);
    
    // PAREIL QU'AU DESSUS LIRE
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Ouvrir a pété: %s\n", filename);
        return;
    }
    
    int found = 0;
    for (int i = 0; i < rows; i++) {
        found = 0;
        linefix(cols, grid[i]);
        int correctedCol = 0;
        for (int j = 0; j < cols; j++) {
            if (grid[i][j].ch != '\0') {
                found = 1;
                correctedGrid[i][correctedCol++] = grid[i][j];
                printf("%c", grid[i][j].ch);
                fprintf(file, "%c", grid[i][j].ch);  //Ecris
            }
        }
        //Cols en 0
              while (correctedCol < cols) {
                  correctedGrid[i][correctedCol++].ch = '0';
              }
        if (found) {
            printf("\n");
            fprintf(file, "\n");  // ecris
        }
    }
    // ferme
    fclose(file);
}

char* ProcessAnswer(const char *filename, const char *answer) {
    //RESULTAT
    static char result[512];
    char command[512];
    snprintf(command, sizeof(command), "./solver %s %s", filename, answer);
        //fp come FRANCOIS PICHARD AHAHAHAHAHAHAHAH JE SUIS LE JOKER
        FILE *fp = popen(command, "r");
        if (fp == NULL) {
            fprintf(stderr, "PEUT PAS RUN PTNN C LE SOLVEUR LE PROBLEME\n");
            exit(EXIT_FAILURE);
        }
    //Humhum
    if (fgets(result, sizeof(result), fp) == NULL) {
        fprintf(stderr, "Solver renvoie nimp bip boup C BUGGE PTN JESP\n");
        pclose(fp);
        exit(EXIT_FAILURE);
    }
    pclose(fp);

    // Si y'a r ca dégage
    size_t len = strlen(result);
    if (len > 0 && result[len - 1] == '\n') {
        result[len - 1] = '\0';
    }
    return result;
}

void removeEmptyLines(int rows, int cols, CharRect grid[rows][cols], CharRect newGrid[rows][cols], int *newRows) {
    int corback = 0;
    for (int i = 0; i < rows; i++) {
        int isEmpty = 1;
        for (int j = 0; j < cols; j++) {
            if (grid[i][j].ch != '0') {
                isEmpty = 0;
                break;
            }
        }
            if (!isEmpty) {
                for (int j = 0; j < cols; j++) {
                    newGrid[corback][j] = grid[i][j];
                }
                corback++;
            }
    }
    *newRows = corback;  // Nouvrau nombre de rowsssss
}

//TROUVE LES REPONSES
void processAnswers(const char *gridFile, const char *reponsesFile, SDL_Surface* surface, CharRect grid[100][100]) {
    Uint32 color = SDL_MapRGB(surface->format, 0, 255, 0);  // c du vert comme le grinch 
    FILE *file = fopen(reponsesFile, "r");
    if (file == NULL) {
        fprintf(stderr, "CA OUVRE PASSSS: %s\n", reponsesFile);
        return;
    }
          char answer[512];
          while (fgets(answer, sizeof(answer), file) != NULL) {
              printf("Treaite la réponse...\n");
              // ENLEVE CETTE PTN DE LIGNE
              size_t len = strlen(answer);
              if (len > 0 && answer[len - 1] == '\n') {
                  answer[len - 1] = '\0';
              }
        char *result = ProcessAnswer(gridFile, answer);
        printf("Rep: %s - Res: %s\n", answer, result);
        if (strcmp(result, "Not found") != 0) {
            printf("Drawin' Answer line to be able to see it\n");
            int startX, startY, endX, endY;
            sscanf(result, "(%d,%d)(%d,%d)", &startY, &startX, &endY, &endX);
            printf("coordonnates: startX=%d, startY=%d, endX=%d, endY=%d\n", startX, startY, endX, endY);
            Rectangle rect1 = grid[startX][startY].rect;
            Rectangle rect2 = grid[endX][endY].rect;
            // Debug info for rect1 and rect2
            printf("Rect1: x=%d, y=%d, longueur=%d, largeur=%d\n", rect1.x, rect1.y, rect1.longueur, rect1.largeur);
            printf("Rect2: x=%d, y=%d, longueur=%d, largeur=%d\n", rect2.x, rect2.y, rect2.longueur, rect2.largeur);

            // Draw a line from the center of rect1 to the center of rect2
            int centerX1 = rect1.x + rect1.longueur / 2;
            int centerY1 = rect1.y + rect1.largeur / 2;
            int centerX2 = rect2.x + rect2.longueur / 2;
            int centerY2 = rect2.y + rect2.largeur / 2;

            // Debug info for center coordinates
            printf("Drawing line from (%d, %d) to (%d, %d)\n", centerX1, centerY1, centerX2, centerY2);
            drawLine(surface, centerX1, centerY1, centerX2, centerY2, color);
        }
    }

    fclose(file);
}
// input_filename : fichier d'entrée, optional_ini_filename fichier de config, puis output_dir : le 
//dossier de sortie : là ou le dossier temp et temp.png seront crées.
void process_image(char *input_filename, const char *optional_ini_filename, const char *output_dir) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init ERROR: %s\n", SDL_GetError());
        return;
    }
    SDL_Surface *image = IMG_Load(input_filename);
    if (!image) {
        printf("IMG_Load ERROR: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }
    char config_filename[256];
    snprintf(config_filename, sizeof(config_filename), "%s%s", CONFIGS_FOLDER, input_filename);
    //Simple comme bonjour chef
    if (optional_ini_filename) {
        read_ini_file(optional_ini_filename);
    } else if (file_exists(config_filename)) {
        read_ini_file(config_filename);
    } else {
        convert_ini_to_png(input_filename);
        generate_ini_for_image(input_filename);
        read_ini_file("config.ini");
    }
    printf("TOLERANCE: %d\n", tolerance);
    printf("MIN_LENGTH: %d\n", min_length);
    printf("MAX_LENGTH: %d\n", max_length);
    printf("MIN_HEIGHT: %d\n", min_heigth);
    printf("MAX_HEIGHT: %d\n", max_heigth);
    printf("STEP_SIZE: %d\n", step_size);
    printf("OVERLAPPING_TOLERANCE: %d\n", overlapping_tolerance);
    convertToGrayscale(image);
    RectangleNode *head = NULL;
    for (int y = 0; y < image->h; y += step_size) {
                  for (int x = 0; x < image->w; x++) {
                      if (isBlack(image, x, y) && !isVisited(image, x, y)) {
                          int minX = x, maxX = x, minY = y, maxY = y;
                          findObject(image, x, y, &minX, &maxX, &minY, &maxY);
                          int longueur = maxX - minX;
                          int largeur = maxY - minY;
                          if (longueur >= min_length && longueur <= max_length && largeur >= min_heigth && largeur <= max_heigth) {
                              Rectangle rect = {minX, minY, longueur + 1, largeur + 1};
                              insertNode(&head, rect, y);
       }
       }
        }
    }
      //Okkk
           head = sortList(head);
          RectangleNode *current = head;
          int currentLine = 0, currentAnswer = 0, currentChar = 0;
          int answercharcount = 0;
          int linecharcount = 0;
          int answer = 0;
          int foundline = 0;
          RectangleNode *prev = NULL;
          char filename[256];
          
    CharRect grid[100][100];
    CharRect answerList[100][100];

        // Initialize the grid with '\0'
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                grid[i][j].ch = '\0';
            }
        }

    // Initialize the answer list with '\0'
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            answerList[i][j].ch = '\0';
        }
    }
    //Vrmnt c la boucle des enfers seul dieu la comprends
    //Touchez à rien les bgs
    while (current != NULL) {
        int y = current->y;
        foundline = 0;
        int isAnswer = 0;

        while (current != NULL && current->y == y) {
            printf("Takin' care of this one y: %d, x: %d\n", current->rect.y, current->rect.x);
            if (current->next != NULL && current->next->y == y && isOverlappingOrClose(current->rect, current->next->rect)) {
                answercharcount++;
                isAnswer = 1;
                snprintf(filename, sizeof(filename), "%s/reponse%d-%d.jpg", output_dir, currentAnswer, currentChar);
                saveObject(image, current->rect.x, current->rect.y, current->rect.x + current->rect.longueur, current->rect.y + current->rect.largeur, filename);
                answerList[currentAnswer][currentChar].rect = current->rect;
                answerList[currentAnswer][currentChar].ch = runNeuralNetwork(filename);
                printf("reponse%d-%d.jpg is a %c\n", currentAnswer, currentChar, answerList[currentLine][currentChar].ch );
            } else {
                if (answercharcount > 0 || (current->prev != NULL && isOverlappingOrClose(current->rect, current->prev->rect))) {
                    answercharcount = 0;
                    answer++;
                    snprintf(filename, sizeof(filename), "%s/reponse%d-%d.jpg", output_dir, currentAnswer, currentChar);
                    saveObject(image, current->rect.x, current->rect.y, current->rect.x + current->rect.longueur, current->rect.y + current->rect.largeur, filename);
                    answerList[currentAnswer][currentChar].rect = current->rect;
                    answerList[currentAnswer][currentChar].ch = runNeuralNetwork(filename);
                    printf("reponse%d-%d.jpg is a %c\n", currentAnswer, currentChar, answerList[currentLine][currentChar].ch );
                } else {
                    foundline = 1;
                    linecharcount++;
                    snprintf(filename, sizeof(filename), "%s/y%d-x%d.jpg", output_dir, currentLine, currentChar);
                    saveObject(image, current->rect.x, current->rect.y, current->rect.x + current->rect.longueur, current->rect.y + current->rect.largeur, filename);
                    grid[currentLine][currentChar].rect = current->rect;
                    grid[currentLine][currentChar].ch = runNeuralNetwork(filename);
                    printf("y%d-x%d.jpg is a %c\n", currentLine, currentChar, grid[currentLine][currentChar].ch );
                }
            }
            
            drawRectangle(image, current->rect, SDL_MapRGB(image->format, 255, 0, 0));
            currentChar++;

            RectangleNode *toDelete = current;
            if (prev == NULL) {
                head = current->next;
            } else {
                prev->next = current->next;
            }
            current = current->next;
            free(toDelete);
        }

        printf("END %d: isAnswer = %d\n", y, isAnswer);
        currentAnswer += isAnswer;
        currentLine += foundline;
        currentChar = 0;

        if (current != NULL) {
            current = head;
        }
    }
    //PERSONNE SAIT CODER DANS CE BAHUT
    CharRect corrected[100][100];
    //LEOPOLD PTN ON AVAIT DIS INIT AU DEBUT
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            corrected[i][j].ch = '\0';
        }
    }
    printGrid(100,100, answerList, "reponses");
    printGridCorrected(100,100, grid, corrected, "grille");
    //T'as vraiment chié dans la colle theo
    char temp_output_path[256];
    snprintf(temp_output_path, sizeof(temp_output_path), "%s/temp.png", output_dir);
    IMG_SavePNG(image, temp_output_path);
    //Sauve une première fois
    const char *gridFile = "grille.txt";
    const char *reponsesFile = "reponses.txt";
    //Vzy c niqué la grille est full buggé
    int newRows; 
    CharRect correctedagain[100][100];
    removeEmptyLines(100, 100, corrected, correctedagain, &newRows);
    processAnswers(gridFile, reponsesFile, image, correctedagain);
    //Process le bail c long
    char tpmp[256];
    snprintf(tpmp, sizeof(tpmp), "%s/temp2.png", output_dir);
    printf("saving final...\n");
    IMG_SavePNG(image, tpmp);
    return;
}
