#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define IMAGE_WIDTH 50
#define IMAGE_HEIGHT 50
#define SPACING 10

void displayImage(SDL_Renderer *renderer, const char *filePath, int x, int y) {
    SDL_Surface *image = IMG_Load(filePath);
    if (!image) {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    if (!texture) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        return;
    }
    SDL_Rect dst = {x, y, IMAGE_WIDTH, IMAGE_HEIGHT};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

void displayImages(const char *windowTitle, const char *prefix, int maxCol, int maxRow) {
    SDL_Window *window = SDL_CreateWindow(windowTitle,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return;
    }

    DIR *dir = opendir("temp");
    if (!dir) {
        printf("Could not open temp directory\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return;
    }

    struct dirent *entry;
    int x = SPACING, y = SPACING;

    char reponse[100][100][256] = {0}; // Assuming a max of 50 answers with 50 characters each y en aurait pas plus frr c'est
    //plus des mots mélés à ce point
    char grid[100][100][256] = {0}; // Assuming a max of 50 lines with 50 columns each

    while ((entry = readdir(dir)) != NULL) {
        if ((entry->d_type == SDLK_UNKNOWN || entry->d_type == 8) && strncmp(entry->d_name, prefix, strlen(prefix)) == 0) {
            char filePath[256];
            snprintf(filePath, sizeof(filePath), "temp/%s", entry->d_name);

            int first, second;
            if (strncmp(prefix, "reponse", 7) == 0) {
                sscanf(entry->d_name, "reponse%d-%d.jpg", &first, &second);
                strcpy(reponse[first-1][second-1], filePath);
            } else if (strncmp(prefix, "y", 1) == 0) {
                sscanf(entry->d_name, "y%d-x%d.jpg", &first, &second);
                strcpy(grid[first-1][second-1], filePath);
            }
        }
    }
    closedir(dir);
    //FERME MOI CE REPERTOIRE DE MORT
    if (strncmp(prefix, "reponse", 7) == 0) {
        for (int i = 0; i < maxRow; i++) {
            for (int j = 0; j < maxCol; j++) {
                if (strlen(reponse[i][j]) > 0) {
                    displayImage(renderer, reponse[i][j], x, y);
                    x += IMAGE_WIDTH + SPACING;
                } else {
                    break;
                }
            }
            x = SPACING;
            y += IMAGE_HEIGHT + SPACING;
        }
    } else if (strncmp(prefix, "y", 1) == 0) {
        for (int i = 0; i < maxRow; i++) {
            for (int j = 0; j < maxCol; j++) {
                if (strlen(grid[i][j]) > 0) {
                    displayImage(renderer, grid[i][j], x, y);
                    x += IMAGE_WIDTH + SPACING;
                } else {
                    break;
                }
            }
            x = SPACING;
            y += IMAGE_HEIGHT + SPACING;
        }
    }
    //SPACING VIVE LA NAZA
    SDL_RenderPresent(renderer);

    // Event loop to keep the window open
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Display the grid with max rows and columns set to 50 (adjust as needed)
    displayImages("Grid Viewer", "y", 50, 50);

    // Display the answers with max rows and columns set to 50 (adjust as needed)
    displayImages("Answers Viewer", "reponse", 50, 50);

    SDL_Quit();

    return 0;
}

