#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include "solver.h"

//Simple boucle pour convertir le mot en Majuscules sinon
//ca marche pas KLMAZK?FIOMEJMFIJ3RIOFJ3l!
void WordToMaj(char *word) {
    while (*word) {
        *word = toupper((unsigned char)*word);
        word++;
    }
}

// C'est le main qui s'appellait chipper arrete de chipper
int main(int argc, char * argv[]) {
    if ((argc != 3) && (argc != 4)) {
        errx(1, "Usage: %s <Text File> <Word to find> <Debug/Optional>", argv[0]);
    }
    WordToMaj(argv[2]);
    if((argc == 4) && (*argv[3] == '1')){
      //DEBUG MODE OHOOHOHO
      printf("Welcome to solver.c 1.5.4 made by Theo and Alex [DEBUG MODE]!\n");
      printf("File to look : %s\n", argv[1]);
      printf("Word to find : %s\n", argv[2]);
      printf("Launching solver ...\n");
      Coordinates coordonnes = wordfinderdebug(argv[1], argv[2]);
      if(coordonnes.x0 == -1){
        printf("Not found\n");
      }else{
        printf("(%i,%i)(%i,%i)\n",coordonnes.x0,coordonnes.y0,coordonnes.x1,coordonnes.y1);
      }
      return 0;
    }else{
      Coordinates coordonnes = wordfinder(argv[1], argv[2]);
      if(coordonnes.x0 == -1){
        printf("Not found\n");
      }else{
        printf("(%i,%i)(%i,%i)\n",coordonnes.x0,coordonnes.y0,coordonnes.x1,coordonnes.y1);
      }
      return 0;
    }
}
