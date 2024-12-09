CC = gcc
CFLAGS = `pkg-config --cflags sdl2 SDL2_image` -Wall -Wextra -std=c99
LDFLAGS = `pkg-config --libs sdl2 SDL2_image` -lm

TARGET1 = image_editor
TARGET2 = visionary
TARGET3 = ini

SOURCES1 = image_editor.c
SOURCES2 = visionary.c
SOURCES3 = ini.c

all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(SOURCES1)
	    $(CC) $(CFLAGS) -o $(TARGET1) $(SOURCES1) $(LDFLAGS)

$(TARGET2): $(SOURCES2)
	    $(CC) $(CFLAGS) -o $(TARGET2) $(SOURCES2) $(LDFLAGS)

$(TARGET3): $(SOURCES3)
	    $(CC) $(CFLAGS) -o $(TARGET3) $(SOURCES3) $(LDFLAGS)

clean:
	    rm -f $(TARGET1) $(TARGET2) $(TARGET3)
	    rm -rf temp temp.png config.ini

