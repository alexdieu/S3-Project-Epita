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

#define M_PI 3.14159265358979323846

int tolerance = 200;
int step_size = 5;
int overlapping_tolerance = 5;

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

RectangleNode* createNode(Rectangle rect, int y) {
    RectangleNode *newNode = (RectangleNode*)malloc(sizeof(RectangleNode));
    newNode->rect = rect;
    newNode->y = y;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void insertNode(RectangleNode **head, Rectangle rect, int y) {
    RectangleNode *newNode = createNode(rect, y);
    if (*head == NULL) {
        *head = newNode;
    } else {
        RectangleNode *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }
}

int isBlack(SDL_Surface *surface, int x, int y) {
    Uint32 pixel = ((Uint32 *)surface->pixels)[(y * surface->w) + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    return (r < tolerance && g < tolerance && b < tolerance);
}

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

int isVisited(SDL_Surface *surface, int x, int y) {
    Uint32 pixel = ((Uint32 *)surface->pixels)[(y * surface->w) + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    return (r == 128 && g == 128 && b == 128);
}

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

int isOverlappingOrClose(Rectangle a, Rectangle b) {
    return (abs(a.x + a.longueur - b.x) <= overlapping_tolerance);
}

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

RectangleNode* sortList(RectangleNode* head) {
    RectangleNode *sorted = NULL;
    RectangleNode *current = head;

    while (current != NULL) {
        RectangleNode *next = current->next;
        current->prev = current->next = NULL;
        sorted = sortedInsert(sorted, current);
        current = next;
    }

    printf("Sorted list:\n");
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

void convert_png_to_ini(char *filename) {
    char *pos = strstr(filename, ".png");
    if (pos) {
        strcpy(pos, ".ini");
    }
}

int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

void calculate_dimensions(RectangleNode *head, int *avg_length, int *avg_width, int *min_length, int *max_length, int *min_width, int *max_width) {
    int total_length = 0, total_width = 0;
    int count = 0;
    *min_length = INT_MAX;
    *max_length = INT_MIN;
    *min_width = INT_MAX;
    *max_width = INT_MIN;

    RectangleNode *current = head;

    while (current != NULL) {
        int length = current->rect.longueur;
        int width = current->rect.largeur;

        total_length += length;
        total_width += width;

        if (length < *min_length) *min_length = length;
        if (length > *max_length) *max_length = length;
        if (width < *min_width) *min_width = width;
        if (width > *max_width) *max_width = width;

        count++;
        current = current->next;
    }

    if (count > 0) {
        *avg_length = total_length / count;
        *avg_width = total_width / count;
    } else {
        *avg_length = 0;
        *avg_width = 0;
    }
}

void generate_config_ini(int avg_length, int avg_width, int min_length, int max_length, int min_width, int max_width) {
    FILE *file = fopen("config.ini", "w");
    if (file == NULL) {
        fprintf(stderr, "Error creating config.ini file\n");
        return;
    }
    fprintf(file, "TOLERANCE : %d\n", tolerance);
    fprintf(file, "MIN_LENGTH : %d\n", min_length);
    fprintf(file, "MAX_LENGTH : %d\n", max_length);
    fprintf(file, "MIN_WIDTH : %d\n", min_width);
    fprintf(file, "MAX_WIDTH : %d\n", max_width);
    fprintf(file, "STEP_SIZE : %d\n", step_size);
    fprintf(file, "OVERLAPPING_TOLERANCE : %d\n", overlapping_tolerance);
    fclose(file);
}

void calculate_step_size(RectangleNode *head, int *step_size) {
    int total_diff = 0;
    int count = 0;
    RectangleNode *current = head;
    RectangleNode *next = NULL;

    while (current != NULL) {
        next = current->next;
        while (next != NULL && next->y == current->y) {
            next = next->next;
        }
        if (next != NULL) {
            total_diff += abs(next->y - current->y);
            count++;
        }
        current = next;
    }

    if (count > 0) {
        *step_size = total_diff / count;
    } else {
        *step_size = 10; // Default step size if no valid calculation
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        printf("Usage: %s <Path to the file> <Optional INI file>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init ERROR: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        printf("IMG_Load ERROR: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Convert the image to grayscale
    convertToGrayscale(image);
    RectangleNode *head = NULL;

    // Detect rectangles in the image
    for (int y = 0; y < image->h; y += step_size) {
        for (int x = 0; x < image->w; x++) {
            if (isBlack(image, x, y) && !isVisited(image, x, y)) {
                int minX = x, maxX = x, minY = y, maxY = y;
                findObject(image, x, y, &minX, &maxX, &minY, &maxY);
                int longueur = maxX - minX;
                int largeur = maxY - minY;
                if (longueur >= 0 && longueur <= 50 && largeur >= 10 && largeur <= 50) {
                    Rectangle rect = {minX, minY, longueur, largeur};
                    insertNode(&head, rect, y);
                }
            }
        }
    }

    // Sort the detected rectangles
    head = sortList(head);

    // Calculate the average, minimum, and maximum dimensions
    int avg_length, avg_width, min_length, max_length, min_width, max_width;
    calculate_dimensions(head, &avg_length, &avg_width, &min_length, &max_length, &min_width, &max_width);

    // Calculate the step size
    calculate_step_size(head, &step_size);
    step_size /= 2;

    // Print the dimensions
    printf("Average Length: %d\n", avg_length);
    printf("Average Width: %d\n", avg_width);
    printf("Minimum Length: %d\n", min_length);
    printf("Maximum Length: %d\n", max_length);
    printf("Minimum Width: %d\n", min_width);
    printf("Maximum Width: %d\n", max_width);
    printf("Calculated Step Size: %d\n", step_size);

    // Generate config.ini file
    generate_config_ini(avg_length, avg_width, min_length, max_length, min_width, max_width);

    SDL_FreeSurface(image);
    SDL_Quit();
    return 0;
}

