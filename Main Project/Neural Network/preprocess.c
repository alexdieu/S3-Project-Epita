#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>  // Include for mkdir
#include <sys/types.h> // Include for mode_t

void resize_and_binarize(const char *input_path, const char *output_path) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    SDL_Surface *image = IMG_Load(input_path);
    if (!image) {
        printf("IMG_Load: %s\n", IMG_GetError());
        return;
    }

    SDL_Surface *resized_image = SDL_CreateRGBSurface(0, 28, 28, 32, 0, 0, 0, 0);
    SDL_BlitScaled(image, NULL, resized_image, NULL);

    Uint32 *pixels = (Uint32 *)resized_image->pixels;
    for (int i = 0; i < resized_image->w * resized_image->h; ++i) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], resized_image->format, &r, &g, &b);
        Uint8 gray = 0.299 * r + 0.587 * g + 0.114 * b;
        Uint8 binarized = gray > 127 ? 255 : 0;
        pixels[i] = SDL_MapRGB(resized_image->format, binarized, binarized, binarized);
    }

    IMG_SavePNG(resized_image, output_path);

    SDL_FreeSurface(image);
    SDL_FreeSurface(resized_image);
    IMG_Quit();
    SDL_Quit();
}

void process_directory(const char *input_dir, const char *output_dir) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(input_dir)) == NULL) {
        perror("opendir() error");
    } else {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char sub_input_dir[512];
                char sub_output_dir[512];
                snprintf(sub_input_dir, sizeof(sub_input_dir), "%s/%s", input_dir, entry->d_name);
                snprintf(sub_output_dir, sizeof(sub_output_dir), "%s/%s", output_dir, entry->d_name);
                
                // Create subdirectory
                mkdir(sub_output_dir, 0777);

                process_directory(sub_input_dir, sub_output_dir);
            } else if (entry->d_type == DT_REG) {
                char input_path[512];
                char output_path[512];
                snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, entry->d_name);
                snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, entry->d_name);
                resize_and_binarize(input_path, output_path);
            }
        }
        closedir(dir);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_directory> <output_directory>\n", argv[0]);
        return 1;
    }

    const char *input_dir = argv[1];
    const char *output_dir = argv[2];

    process_directory(input_dir, output_dir);

    return 0;
}

