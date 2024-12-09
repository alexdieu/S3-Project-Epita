# COMPILATEUR
GCC = gcc
# TARGETS
TARGET = solver
# SOURCES
SOURCES = main.c solver.c
# REGLE ALL
all: $(TARGET)

#FLAGS DU COMPILEUR
CFLAGS = -Wextra -Wall

$(TARGET): $(SOURCES)
	$(GCC) $(CFLAGS) -o $(TARGET) $(SOURCES)
	cp solver ../Gui/solver
# NETTOYAGE
clean:
	rm -f $(TARGET) ../Gui/Solver

