# Définir les variables
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 $(shell pkg-config --cflags gtk+-3.0 glib-2.0 sdl2 SDL2_image)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 glib-2.0 sdl2 SDL2_image) -lm

# Fichiers sources et cibles
SRC_INTERFACE = interface.c ../Pretreatment/AutoRotate/autorotate.c ../Pretreatment/Decoupage/image_editor.c
OBJ_INTERFACE = $(SRC_INTERFACE:.c=.o)
EXEC_INTERFACE = interface

# Règles par défaut
all: $(EXEC_INTERFACE)

# Compilation de l'exécutable
$(EXEC_INTERFACE): $(OBJ_INTERFACE)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers temporaires
clean:
	rm -rf $(OBJ_INTERFACE) $(EXEC_INTERFACE) decoupage_temp/ grille.txt reponses.txt config.ini temp.png

# Forcer une reconstruction complète
rebuild: clean all

# Ajout d'une règle pour tester rapidement
run: $(EXEC_INTERFACE)
	./$(EXEC_INTERFACE)

