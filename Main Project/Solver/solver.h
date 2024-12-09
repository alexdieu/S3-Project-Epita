#ifndef SOLVER_H
#define SOLVER_H

//Structure permettant de contenir les coordonnes du mot
typedef struct {
  int x0;
  int y0;
  int x1;
  int y1;
} Coordinates;

Coordinates wordfinder(const char * textfile, const char * wordtofind);

Coordinates wordfinderdebug(const char * textfile, const char * wordtofind);

#endif
