#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "../Pretreatment/Decoupage/image_editor.h"
#include "../Pretreatment/AutoRotate/autorotate.h"
#include <pthread.h>
#include <dirent.h> 
// Variables globales pour etre genral t as capteeeeeeeeeeeeee ou pas 
static bool image_loaded = false;
static bool image_rotated = false;
static bool image_cut = false;

// Pointeurs pour les boutons eeeeee oe mtn meme les boutons s y mettent 
static GtkWidget *rotate_button;
static GtkWidget *decoupage_button;
static GtkWidget *reponse_button;
// Déclaration d amour a mes fonctions cheries je les aimeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
void reponse_image();
void cleanup_temp_files();
int init_SDL();
void load_image_callback(gpointer data);
void rotate_image();
void refresh_display(const char *file_path);
void on_gtk_window_destroy(gpointer data);
void save_current_image(const char *output_file);
void save_image_callback(GtkWidget *widget);
void decoupage_image();
void handle_sdl_event(SDL_Event *event);
int running = 1;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Rect rect;
SDL_Surface *current_image = NULL;  
int level=0 ;

// ca c est le personnagge principale
int main(int argc, char *argv[]) {
    bool running  = true;

    // j initialise un truck  GTK
    gtk_init(&argc, &argv);

    // la je cree un tutruturtrutr gtk
    GtkWidget *gtk_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gtk_window), "Theo terminator 2000");
    gtk_window_set_default_size(GTK_WINDOW(gtk_window), 800, 600);
    g_signal_connect(gtk_window, "destroy", G_CALLBACK(on_gtk_window_destroy), &running);

    // Créer les boutons d acnee d un jeune puber
    GtkWidget *load_button = gtk_button_new_with_label("Charger l'image");
    g_signal_connect(load_button, "clicked", G_CALLBACK(load_image_callback), gtk_window);

    rotate_button = gtk_button_new_with_label("Auto-Rotate");
    g_signal_connect(rotate_button, "clicked", G_CALLBACK(rotate_image), NULL);
    gtk_widget_set_sensitive(rotate_button, FALSE);

    decoupage_button = gtk_button_new_with_label("Decoupage");
    g_signal_connect(decoupage_button, "clicked", G_CALLBACK(decoupage_image), NULL);
    gtk_widget_set_sensitive(decoupage_button, FALSE);

    reponse_button = gtk_button_new_with_label("Reponse");
    g_signal_connect(reponse_button, "clicked", G_CALLBACK(reponse_image), NULL);
    gtk_widget_set_sensitive(reponse_button, FALSE);

    GtkWidget *save_button = gtk_button_new_with_label("Sauvegarder l'image");
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_image_callback), NULL);

    // j ajoute dees boutons à la fenêtre gtk comme ca c est careee le ssssssss
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), load_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), rotate_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), decoupage_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), reponse_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), save_button, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(gtk_window), box);

    // Afficher la fenêtre GTK  le sang  de  la veine 
    gtk_widget_show_all(gtk_window);

    // Boucle principale GTK c est la ou ca commence bb 
    while (running) {
        gtk_main_iteration();
    }
    cleanup_temp_files();
    return EXIT_SUCCESS;
}
void save_image_callback(GtkWidget *widget) {
    if (!current_image) {
        printf("Erreur : aucune image chargée pour sauvegarde.\n");
        return;
    }

    //  une boîte de dialogue gtk magique pour choisir l'image à sauvegarder et oe on fait les chose bien ici
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Sélectionner l'image à sauvegarder",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Original", GTK_RESPONSE_ACCEPT,
        "_Rotate", GTK_RESPONSE_YES,
        "_Decoupage", GTK_RESPONSE_NO,
        "_Reponse", GTK_RESPONSE_APPLY, 
        NULL
    );

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    const char *file_path = NULL;
    SDL_Surface *image_to_save = NULL;
    //le choix del profesorrrrrrrrrrrrr	
    switch (response) {
        case GTK_RESPONSE_ACCEPT:
            file_path = "temp.png"; 
            image_to_save = current_image;
            break;

        case GTK_RESPONSE_YES:
            file_path = "rotate_temp.png";  
            image_to_save = IMG_Load(file_path);
            if (!image_to_save) {
                printf("Erreur de chargement de l'image  après rotation.\n");
                gtk_widget_destroy(dialog);
                return;
            }
            break;

        case GTK_RESPONSE_NO:
            file_path = "decoupage_temp/temp.png";  
            image_to_save = IMG_Load(file_path); 
            if (!image_to_save) {
                printf("Erreur de chargement  de l'image après découpage.\n");
                gtk_widget_destroy(dialog);
                return;
            }
            break;

        case GTK_RESPONSE_APPLY:
            file_path = "decoupage_temp/temp2.png";  
            image_to_save = IMG_Load(file_path);  
            if (!image_to_save) {
                printf("Erreur de  chargement de l'image après le deuxième découpage.\n");
                gtk_widget_destroy(dialog);
                return;
            }
            break;

        default:
            // fin du spectacle
            gtk_widget_destroy(dialog);
            return;
    }

    // et oeee tu choisis ce que tu veux c est un buffet
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
        
        // Vérification du cbef .png
        if (!g_str_has_suffix(save_path, ".png")) {
            // Ajouter l'extension .png au chemin du fichier
            char *save_path_with_png = g_strdup_printf("%s.png", save_path);
            g_free(save_path);
            save_path = save_path_with_png;
        }

        // Sauvegarder l'image appppprouvedddddddd
        if (SDL_SaveBMP(image_to_save, save_path) != 0) {
            printf("Erreur : impossible de  sauvegarder l'image sous '%s': %s\n", save_path, SDL_GetError());
        } else {
            printf("Image sauvegardée avec  succès sous '%s'.\n", save_path);
        }

        g_free(save_path);
    }

    gtk_widget_destroy(save_dialog);
    // Libérer la cream 
    if (image_to_save && image_to_save != current_image) {
        SDL_FreeSurface(image_to_save);
    }

    gtk_widget_destroy(dialog);
}

// j ini un truck de SDL
int init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL  could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Fenêtre SDL Noire", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window  could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer  could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}
//je ne comprend plus ou je suis aide moi petitt bggggg et aide moiiiiiiiiiii'
void traiter_level(char *str) {
    char *start = strstr(str, "level_");
    int *res = &level;
    if (start != NULL) {
        start += 6; 
        if (start[0] >= '0' && start[0] <= '9') {
            *res = start[0] - '0'; 
	    return;
        }
        *res = -1;
	}
}

// Fonction pour charger et commencer le traviallll 
void load_image_callback(gpointer data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Choose Image", GTK_WINDOW(data),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT, 
        NULL
    );
    image_loaded = true;
    image_rotated = false;
    image_cut = false;
    // hop la rota qui arrive apres 
    gtk_widget_set_sensitive(rotate_button, TRUE);

    // et nonnonnonoonoono
    gtk_widget_set_sensitive(decoupage_button, FALSE);
    gtk_widget_set_sensitive(reponse_button, FALSE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        SDL_Surface *loaded_surface = IMG_Load(file_path);
        traiter_level(file_path);
        if (!loaded_surface) {
            printf("COULD  NOT LOAD THE PICTURE: %s\n", IMG_GetError());
            g_free(file_path);
            gtk_widget_destroy(dialog);
            return;
        }

        // Sauvegarder l aboutissement d une vie  "temp.png"
        if (SDL_SaveBMP(loaded_surface, "temp.png") != 0) {
            printf("Failed to save image  as temp.png: %s\n", SDL_GetError());
            SDL_FreeSurface(loaded_surface);
            g_free(file_path);
            gtk_widget_destroy(dialog);
            return;
        }

        printf("Image successfully saved  as temp.png\n");

        // Rafraîchir l'affichage tellement elle est fraiche 
        refresh_display("temp.png");

        // Sauvegarder la surface de l'image actuelle tktktkkkttkk on fait les choses bien 
        current_image = loaded_surface;

        g_free(file_path);
    }

    gtk_widget_destroy(dialog);
}

// Fonction pour appliquer la rotation à l'image ca tourme tourne 
void rotate_image() {
    if (!image_loaded) return;

    // Simuler la rotation de l'image et oeeoeoeo on accepte ou passsss
    image_rotated = true;
    image_cut = false;

    // Activer le bouton de découpage et oe il est majeur 
    gtk_widget_set_sensitive(decoupage_button, TRUE);

    // Désactiver le bouton de réponse et nonnon au mineur
    gtk_widget_set_sensitive(reponse_button, FALSE);

    char *input_file = "temp.png";
    char *output_file = "rotate_temp.png";

    rotation_auto(input_file, output_file);

    if (access(output_file, F_OK) != 0) {
        printf("Error: File '%s' does not exist after  rotation.\n", output_file);
        return;
    }

    printf("Rotation successful,  refreshing display with '%s'.\n", output_file);
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

    // Vérification du fichier de sortie la question d existance 
    const char *output_file = "decoupage_temp/temp.png";
    if (access(output_file, F_OK) != 0) {
        fprintf(stderr, "Erreur : le fichier '%s' n'a pas été  généré.\n", output_file);
    } else {
        printf("Découpage réussi, mise à jour de l'affichage  avec '%s'.\n", output_file);
        refresh_display(output_file);
    }

    free(decoupage_args); // Libération tous mes memoires 
    return NULL;
}
void decoupage_image() {
    if (!image_rotated) return;

    // Simuler le découpage de l'image et simuler ca me connait
    image_cut = true;

    // Activer le bouton de réponse et opopopop on active
    gtk_widget_set_sensitive(reponse_button, TRUE);

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
	if (!image_cut) return;
	refresh_display("decoupage_temp/temp2.png");
}

	

// Fonction pour rafraîchir l'affichage avec l'image tellement qu elle est fraiche 
void refresh_display(const char *file_path) {
    printf("Refreshing display with file: %s\n", file_path);

    // Fermer la fenêtre et le contexte de rendu existants et aurevoir petit
    if (texture) { 
        SDL_DestroyTexture(texture); 
        texture = NULL;
    }
    if (renderer)  {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window)  { 
        SDL_DestroyWindow(window);
        window = NULL; 
    }

    // Créer une nouvelle fenêtre SDL elle est newwwwwwwwwwwwwwwwwwwwwwww
    window = SDL_CreateWindow("Image Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, SDL_WINDOW_SHOWN);
    if (!window) { 
        printf("Error: Could not create  window: %s\n", SDL_GetError());
        return;
    }

    // Créer un nouveau renderer pour cette fenêtre alllerllrlerlllaleller
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Error: Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        window = NULL;
        return;
    }

    // Charger l'image sélectionnée charge comme jamais 
    SDL_Surface *loaded_surface = IMG_Load(file_path);
    if (!loaded_surface) {
        printf("Error: Could not  load the image %s: %s\n", file_path, IMG_GetError());
        return;
    }

    // Créer une texture à partir de la surface chargée creation tktkkttktkktkt
    texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);

    if (!texture) {
        printf("Error: Could not create texture from surface: %s\n", SDL_GetError());
        return;
    }

    // Calculer le ratio de redimensionnement je suis einshtein 
    float width_ratio = (float)1280 / loaded_surface->w;
    float height_ratio = (float)960 / loaded_surface->h;
    float ratio = (width_ratio < height_ratio) ? width_ratio : height_ratio;

    rect.w = (int)(loaded_surface->w * ratio);
    rect.h = (int)(loaded_surface->h * ratio);
    rect.x = (1280 - rect.w) / 2;
    rect.y = (960 - rect.h) / 2;

    // Effacer l'écran et hop y a air 
    SDL_RenderClear(renderer);

    // Afficher la texture hop y a quelque chose 
    if (SDL_RenderCopy(renderer, texture, NULL, &rect) != 0) {
        printf("Error: Could not render texture: %s\n", SDL_GetError());
        return;
    }

    // Afficher l'image hop une image 
    SDL_RenderPresent(renderer);
    printf("Display refreshed successfully.\n");
}
void handle_sdl_event(SDL_Event *event) {
    if (event->type == SDL_QUIT) {
        running = 0;  // stop de quoi tu me parles j en perd les mots 
    }
}
// Fonction pour fermer proprement l'application ici on est civiliseeeeeeee
void on_gtk_window_destroy(gpointer data) {
    int *running = (int *)data;
    *running = 0;  // Arrête la boucle SDL c est la fin 

    // on meurt tousssssssssssss
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    // le dernier des haricots 
    if (gtk_main_level() > 0) {
        gtk_main_quit();
    }
}


void cleanup_temp_files() {
    // Supprimer les fichiers temporaires individuels ici on lave deriere sois
    const char *temp_files[] = {"temp.png", "rotate_temp.png","config.ini"};
    for (long unsigned int i = 0; i < sizeof(temp_files) / sizeof(temp_files[0]); i++) {
        if (access(temp_files[i], F_OK) == 0) {
            if (remove(temp_files[i]) != 0) {
                perror("Erreur lors de la suppression d'un fichier temporaire");
            } else {
                printf("Fichier temporaire supprimé : %s\n", temp_files[i]);
            }
        }
    }

    // Supprimer les fichiers dans le répertoire `decoupage_temp` et hop le nettoyage 
    const char *temp_dir = "decoupage_temp";
    DIR *dir = opendir(temp_dir);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char file_path[1024];
                snprintf(file_path, sizeof(file_path), "%s/%s", temp_dir, entry->d_name);
                if (remove(file_path) != 0) {
                    perror("Erreur lors de la  suppression d'un fichier dans le dossier temporaire");
                } else {
                    printf("Fichier supprimé  : %s\n", file_path);
                }
            }
        }
        closedir(dir);

        // Supprimer le dossier `decoupage_temp`bybybybbby decou[
        if (rmdir(temp_dir) != 0) {
            perror("Erreur lors de la suppression du dossier temporaire");
        } else {
            printf("Dossier temporaire supprimé : %s\n", temp_dir);
        }
    } else if (errno != ENOENT) {
        perror("Erreur lors de l'ouverture du dossier temporaire");
    }
}

