# Makefile for OCR Neural Network in C

CC = gcc
CFLAGS = -Wall -O2 `sdl2-config --cflags` -I/usr/include/SDL2 -D_REENTRANT -lm
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

# Targets
TARGET = NeuralNetwork
PREPROCESS_TARGET = preprocess

# Source files
SRCS = NeuralNetwork.c
PREPROCESS_SRCS = preprocess.c

all: $(TARGET) $(PREPROCESS_TARGET)

$(TARGET): $(SRCS)
	    $(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)
	    mv NeuralNetwork ../Gui

$(PREPROCESS_TARGET): $(PREPROCESS_SRCS)
	    $(CC) $(CFLAGS) -o $(PREPROCESS_TARGET) $(PREPROCESS_SRCS) $(LDFLAGS)

clean:
	    rm -f $(TARGET) $(PREPROCESS_TARGET) model.dat

