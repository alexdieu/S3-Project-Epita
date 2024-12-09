#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "../Pretreatment/Decoupage/image_editor.h"
#include "../Pretreatment/AutoRotate/autorotate.h"  // Inclure le fichier rotate.h
#include <pthread.h>
#include <dirent.h> // Pour parcourir les fichiers du répertoire

// Déclaration des fonctions
void reponse_image();
void cleanup_temp_files();
int init_SDL();
void load_image_callback(GtkWidget *widget, gpointer data);
void rotate_image();
void refresh_display(const char *file_path);
void on_gtk_window_destroy(GtkWidget *widget, gpointer data);
void save_current_image(const char *output_file);
void save_image_callback(GtkWidget *widget, gpointer data);
void decoupage_image();
void handle_sdl_event(SDL_Event *event);
int running = 1;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Rect rect;
SDL_Surface *current_image = NULL;  // Ajouter une surface pour stocker l'image actuelle
int level=0 ;

// Fonction principale
int main(int argc, char *argv[]) {
    // Initialisation de GTK
    gtk_init(&argc, &argv);

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Initialisation de SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur d'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Créer une fenêtre SDL noire
    window = SDL_CreateWindow("SDL Black Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur de création de la fenêtre SDL: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Créer un renderer pour la fenêtre SDL
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Erreur de création du renderer SDL: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Remplir la fenêtre avec une couleur noire
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Créer une fenêtre GTK
    GtkWidget *gtk_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gtk_window), "GTK & SDL Example");
    gtk_window_set_default_size(GTK_WINDOW(gtk_window), 800, 600);
    g_signal_connect(gtk_window, "destroy", G_CALLBACK(on_gtk_window_destroy), &running);

    // Créer un bouton pour charger une image
    GtkWidget *load_button = gtk_button_new_with_label("Charger l'image");
    g_signal_connect(load_button, "clicked", G_CALLBACK(load_image_callback), gtk_window);

    // Créer un bouton pour appliquer l'auto-rotation
    GtkWidget *rotate_button = gtk_button_new_with_label("Auto-Rotate");
    g_signal_connect(rotate_button, "clicked", G_CALLBACK(rotate_image), NULL);

    // Créer un bouton pour appliquer Decoupage
    GtkWidget *decoupage_button = gtk_button_new_with_label("Decoupage");
    g_signal_connect(decoupage_button, "clicked", G_CALLBACK(decoupage_image), NULL);

    // Créer un bouton pour appliquer Reponse
    GtkWidget *reponse_button = gtk_button_new_with_label("Reponse");
    g_signal_connect(reponse_button, "clicked", G_CALLBACK(reponse_image), NULL);



    // Créer un bouton pour sauvegarder l'image
    GtkWidget *save_button = gtk_button_new_with_label("Sauvegarder l'image");
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_image_callback), NULL);

    // Ajouter les boutons à la fenêtre GTK
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), load_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), rotate_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), decoupage_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), reponse_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), save_button, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(gtk_window), box);

    // Afficher la fenêtre GTK
    gtk_widget_show_all(gtk_window);

    // Boucle principale GTK et SDL
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            handle_sdl_event(&e);  // Gérer les événements SDL
        }
        // Lancer la boucle GTK pour gérer les événements
        gtk_main_iteration();
    }
    // Libérer les ressources

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
    SDL_DestroyWindow(window);
    }

    IMG_Quit();

    SDL_Quit();
    cleanup_temp_files();
    return EXIT_SUCCESS;
}
void save_image_callback(GtkWidget *widget, gpointer data) {
    if (!current_image) {
        printf("Erreur : aucune image chargée pour sauvegarde.\n");
        return;
    }

    // Créer une boîte de dialogue GTK pour choisir l'image à sauvegarder
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Sélectionner l'image à sauvegarder",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Original", GTK_RESPONSE_ACCEPT,
        "_Rotate", GTK_RESPONSE_YES,
        "_Decoupage", GTK_RESPONSE_NO,
        "_Reponse", GTK_RESPONSE_APPLY, // Nouveau bouton
        NULL
    );

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    const char *file_path = NULL;
    SDL_Surface *image_to_save = NULL;

    switch (response) {
        case GTK_RESPONSE_ACCEPT:
            // Sauvegarder l'image originale
            file_path = "temp.png";  // L'image originale
            image_to_save = current_image;
            break;

        case GTK_RESPONSE_YES:
            // Sauvegarder l'image après rotation
            file_path = "rotate_temp.png";  // L'image après rotation
            image_to_save = IMG_Load(file_path);  // Charger l'image après rotation
            if (!image_to_save) {
                printf("Erreur de chargement de l'image après rotation.\n");
                gtk_widget_destroy(dialog);
                return;
            }
            break;

        case GTK_RESPONSE_NO:
            // Sauvegarder l'image après découpage
            file_path = "decoupage_temp/temp.png";  // L'image après découpage
            image_to_save = IMG_Load(file_path);  // Charger l'image après découpage
            if (!image_to_save) {
                printf("Erreur de chargement de l'image après découpage.\n");
                gtk_widget_destroy(dialog);
                return;
            }
            break;

        case GTK_RESPONSE_APPLY:
            // Sauvegarder l'image après le deuxième découpage
            file_path = "decoupage_temp/temp2.png";  // L'image après le deuxième découpage
            image_to_save = IMG_Load(file_path);  // Charger l'image après le deuxième découpage
            if (!image_to_save) {
                printf("Erreur de chargement de l'image après le deuxième découpage.\n");
                gtk_widget_destroy(dialog);
                return;
            }
            break;

        default:
            // Annulation, fermer la boîte de dialogue
            gtk_widget_destroy(dialog);
            return;
    }

    // Ouvrir une boîte de dialogue GTK pour choisir l'emplacement de sauvegarde
    GtkWidget *save_dialog = gtk_file_chooser_dialog_new(
        "Sauvegarder l'image", NULL,
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save", GTK_RESPONSE_ACCEPT,
        NULL
    );

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(save_dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(save_dialog)) == GTK_RESPONSE_ACCEPT) {
        char *save_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(save_dialog));
        
        // Vérifier si le chemin du fichier se termine par .png
        if (!g_str_has_suffix(save_path, ".png")) {
            // Ajouter l'extension .png au chemin du fichier
            char *save_path_with_png = g_strdup_printf("%s.png", save_path);
            g_free(save_path);
            save_path = save_path_with_png;
        }

        // Sauvegarder l'image
        if (SDL_SaveBMP(image_to_save, save_path) != 0) {
            printf("Erreur : impossible de sauvegarder l'image sous '%s': %s\n", save_path, SDL_GetError());
        } else {
            printf("Image sauvegardée avec succès sous '%s'.\n", save_path);
        }

        g_free(save_path);
    }

    gtk_widget_destroy(save_dialog);
    // Libérer l'image chargée si nécessaire
    if (image_to_save && image_to_save != current_image) {
        SDL_FreeSurface(image_to_save);
    }

    gtk_widget_destroy(dialog);
}

// Initialisation de SDL
int init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Fenêtre SDL Noire", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}
void traiter_level(char *str) {
    char *start = strstr(str, "level_");
    int *res = &level;
    if (start != NULL) {
        start += 6; // Déplacer après "level_"
        if (start[0] >= '0' && start[0] <= '9') {
            *res = start[0] - '0'; // Convertir le caractère en entier
        } else {
            printf("Erreur : niveau invalide dans le chemin du fichier.\n");
            *res = -1; // Valeur de défaut ou erreur
        }
    }
}

// Fonction pour charger l'image et l'afficher
void load_image_callback(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Choose Image", GTK_WINDOW(data),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT, 
        NULL
    );

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        SDL_Surface *loaded_surface = IMG_Load(file_path);
        traiter_level(file_path);
        if (!loaded_surface) {
            printf("COULD NOT LOAD THE PICTURE: %s\n", IMG_GetError());
            g_free(file_path);
            gtk_widget_destroy(dialog);
            return;
        }

        // Sauvegarder l'image sous le nom "temp.png"
        if (SDL_SaveBMP(loaded_surface, "temp.png") != 0) {
            printf("Failed to save image as temp.png: %s\n", SDL_GetError());
            SDL_FreeSurface(loaded_surface);
            g_free(file_path);
            gtk_widget_destroy(dialog);
            return;
        }

        printf("Image successfully saved as temp.png\n");

        // Rafraîchir l'affichage
        refresh_display("temp.png");

        // Sauvegarder la surface de l'image actuelle
        current_image = loaded_surface;

        g_free(file_path);
    }

    gtk_widget_destroy(dialog);
}

// Fonction pour appliquer la rotation à l'image
void rotate_image() {
    char *input_file = "temp.png";
    char *output_file = "rotate_temp.png";

    rotation_auto(input_file, output_file);

    if (access(output_file, F_OK) != 0) {
        printf("Error: File '%s' does not exist after rotation.\n", output_file);
        return;
    }

    printf("Rotation successful, refreshing display with '%s'.\n", output_file);
    refresh_display(output_file);
}
typedef struct {
    const char *input_file;
    const char *config_file;
    const char *directory_name;
} DecoupageArgs;

void *decoupage_thread(void *args) {
    DecoupageArgs *decoupage_args = (DecoupageArgs *)args;

    process_image(decoupage_args->input_file, decoupage_args->config_file, decoupage_args->directory_name);

    // Vérification du fichier de sortie
    const char *output_file = "decoupage_temp/temp.png";
    if (access(output_file, F_OK) != 0) {
        fprintf(stderr, "Erreur : le fichier '%s' n'a pas été généré.\n", output_file);
    } else {
        printf("Découpage réussi, mise à jour de l'affichage avec '%s'.\n", output_file);
        refresh_display(output_file);
    }

    free(decoupage_args); // Libération de la mémoire allouée pour les arguments
    return NULL;
}
void decoupage_image() {
    const char *directory_name = "decoupage_temp";
    const char *input_file = "rotate_temp.png";
    const char *config_file = NULL;
    if (level == 1) {
        config_file = "../Pretreatment/New Decoup/level_1.ini";
    } else if (level == 2) {
        config_file = "../Pretreatment/New Decoup/level_2.ini";
    } else if (level == 3) {
        config_file = "../Pretreatment/New Decoup/level_3.ini";
    } else if (level == 4) {
        config_file = "../Pretreatment/New Decoup/level_4.ini";
    }


    if (mkdir(directory_name, 0755) != 0 && errno != EEXIST) {
        perror("Erreur lors de la création du dossier");
        return;
    }

    DecoupageArgs *args = malloc(sizeof(DecoupageArgs));
    if (!args) {
        perror("Erreur d'allocation mémoire");
        return;
    }
    args->input_file = input_file;
    args->config_file = config_file;
    args->directory_name = directory_name;

    pthread_t thread;
    if (pthread_create(&thread, NULL, decoupage_thread, args) != 0) {
        perror("Erreur lors de la création du thread");
        free(args);
        return;
    }

    pthread_detach(thread);
}
void reponse_image()
{
	refresh_display("decoupage_temp/temp2.png");
}

	

// Fonction pour rafraîchir l'affichage avec l'image
void refresh_display(const char *file_path) {
    printf("Refreshing display with file: %s\n", file_path);

    // Fermer la fenêtre et le contexte de rendu existants
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    // Créer une nouvelle fenêtre SDL
    window = SDL_CreateWindow("Image Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Error: Could not create window: %s\n", SDL_GetError());
        return;
    }

    // Créer un nouveau renderer pour cette fenêtre
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Error: Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        window = NULL;
        return;
    }

    // Charger l'image sélectionnée
    SDL_Surface *loaded_surface = IMG_Load(file_path);
    if (!loaded_surface) {
        printf("Error: Could not load the image %s: %s\n", file_path, IMG_GetError());
        return;
    }

    // Créer une texture à partir de la surface chargée
    texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);

    if (!texture) {
        printf("Error: Could not create texture from surface: %s\n", SDL_GetError());
        return;
    }

    // Calculer le ratio de redimensionnement
    float width_ratio = (float)1280 / loaded_surface->w;
    float height_ratio = (float)960 / loaded_surface->h;
    float ratio = (width_ratio < height_ratio) ? width_ratio : height_ratio;

    rect.w = (int)(loaded_surface->w * ratio);
    rect.h = (int)(loaded_surface->h * ratio);
    rect.x = (1280 - rect.w) / 2;
    rect.y = (960 - rect.h) / 2;

    // Effacer l'écran
    SDL_RenderClear(renderer);

    // Afficher la texture
    if (SDL_RenderCopy(renderer, texture, NULL, &rect) != 0) {
        printf("Error: Could not render texture: %s\n", SDL_GetError());
        return;
    }

    // Afficher l'image
    SDL_RenderPresent(renderer);
    printf("Display refreshed successfully.\n");
}
void handle_sdl_event(SDL_Event *event) {
    if (event->type == SDL_QUIT) {
        running = 0;  // Arrête la boucle principale
    }
}
// Fonction pour fermer proprement l'application
void on_gtk_window_destroy(GtkWidget *widget, gpointer data) {
    int *running = (int *)data;
    *running = 0;  // Arrête la boucle SDL en réglant "running" à 0

    // Fermer proprement la fenêtre SDL
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    // Quitter GTK uniquement si la boucle principale est encore en cours
    if (gtk_main_level() > 0) {
        gtk_main_quit();
    }
}

#include <dirent.h> // Pour parcourir les fichiers du répertoire

void cleanup_temp_files() {
    // Supprimer les fichiers temporaires individuels
    const char *temp_files[] = {"temp.png", "rotate_temp.png","config.ini"};
    for (int i = 0; i < sizeof(temp_files) / sizeof(temp_files[0]); i++) {
        if (access(temp_files[i], F_OK) == 0) {
            if (remove(temp_files[i]) != 0) {
                perror("Erreur lors de la suppression d'un fichier temporaire");
            } else {
                printf("Fichier temporaire supprimé : %s\n", temp_files[i]);
            }
        }
    }

    // Supprimer les fichiers dans le répertoire `decoupage_temp`
    const char *temp_dir = "decoupage_temp";
    DIR *dir = opendir(temp_dir);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            // Ignorer "." et ".."
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char file_path[1024];
                snprintf(file_path, sizeof(file_path), "%s/%s", temp_dir, entry->d_name);
                if (remove(file_path) != 0) {
                    perror("Erreur lors de la suppression d'un fichier dans le dossier temporaire");
                } else {
                    printf("Fichier supprimé : %s\n", file_path);
                }
            }
        }
        closedir(dir);

        // Supprimer le dossier `decoupage_temp`
        if (rmdir(temp_dir) != 0) {
            perror("Erreur lors de la suppression du dossier temporaire");
        } else {
            printf("Dossier temporaire supprimé : %s\n", temp_dir);
        }
    } else if (errno != ENOENT) {
        perror("Erreur lors de l'ouverture du dossier temporaire");
    }
}

