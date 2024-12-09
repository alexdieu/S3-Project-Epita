#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "solver.h"

// Ce code m'a fait voir de toutes les couleurs, 
// À déboguer, une véritable horreur. 
// Comme un bouffon, j'ai fait un tour, 
// En copiant-collant sans détour (pour la fct debug).
// Pour la fonction debug, quelle blague, 
// Je rajoute des printf, ça zigzague. 
// Mais impossible pour mettre à jour
// Ce code qui m'a causé tant de détours.
//vu que quand je mets à jour l'un faut aussi que je mette à jour l'autre

Coordinates wordfinder(const char *textfile, const char *wordtofind) {
    // Ouvrir le fichier
    FILE *fichiertxt = fopen(textfile, "r");
    if (fichiertxt == NULL) {
        errx(1, "Could not open file %s!\n", textfile);
    }
    //ok ok on sauve toutes ces precieuses infos
    int lines = 0;
    int length = 0;
    char c; //coubeh AHAHAHHAAHHAHA ACHETE MON DVD
    int current_line_length = 0;

    // Boucle permettant de calculer le nombre de lignes et colonnes du tableau
    while ((c = fgetc(fichiertxt)) != EOF) {
        if (c == '\n') {
            lines++;
            if (lines != 1) {
                if (length != current_line_length) {
                    errx(1, "Error : Grid does not have same length everywhere");
                }
                current_line_length = 0;
            } else {
                length = current_line_length;
                current_line_length = 0;
            }
        } else {
            current_line_length++;
        }
    }
    
    //Krusty le vlown
    char lignes[lines][length + 1];
    // Reviens en haut du fichier
    rewind(fichiertxt);
    // Chope chaque ligne du fichier et les mets dans le tableau
    for (int i = 0; i < lines; i++) {
        fgets(lignes[i], length + 2, fichiertxt);
    }

    // Boucles pour chercher le mot
    int wordlen = strlen(wordtofind);
    Coordinates coordonnes = {-1, 0, 0, 0};
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < length; j++) {
            // Cherhcer le mot à l'horizontal // BOUE DE SAUVETAGE
            if (j <= length - wordlen) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i][j + k] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j + wordlen - 1, i};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }

            // Chercher le mot à la verticale #ECHELLE
            if (i <= lines - wordlen) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i + k][j] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j, i + wordlen - 1};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }

            // Chercher en diagonale // C LE FOU 
            if (i <= lines - wordlen && j <= length - wordlen) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i + k][j + k] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j + wordlen - 1, i + wordlen - 1};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }

            // Diagonal à l'envers // METS LA A LENVERS
            if (i >= wordlen - 1 && j <= length - wordlen) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i - k][j + k] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j + wordlen - 1, i - wordlen + 1};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }

            // Diagonal mirroir // CARTOON CAT
            if (i <= lines - wordlen && j >= wordlen - 1) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i + k][j - k] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j - wordlen + 1, i + wordlen - 1};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }

            // Diagonale à l'envers //
            if (i >= wordlen - 1 && j >= wordlen - 1) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i - k][j - k] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j - wordlen + 1, i - wordlen + 1};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }

            // Horizontal à l'envers
            if (j >= wordlen - 1) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i][j - k] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j - wordlen + 1, i};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }

            // Verical à l'envers
            if (i >= wordlen - 1) {
                int trouve = 1;
                for (int k = 0; k < wordlen; k++) {
                    if (lignes[i - k][j] != wordtofind[k]) {
                        trouve = 0;
                        break;
                    }
                }
                if (trouve) {
                    coordonnes = (Coordinates){j, i, j, i - wordlen + 1};
                    fclose(fichiertxt);
                    return coordonnes;
                }
            }
        }
    }

    fclose(fichiertxt);
    return (Coordinates){-1, 0, 0, 0};
}


Coordinates wordfinderdebug(const char * textfile, const char * wordtofind) {
    //Ouvrir le fichier
    FILE * fichiertxt = fopen(textfile, "r");
    printf("Opening the file %s\n", textfile);
    if(fichiertxt == NULL){
      //Si l'ouverture du fichier a échoué
      errx(1,"Could not open file %s!\n", textfile);
    }
    
    int lines = 0;
    int lenght = 0;
    char c;
    int current_line_lenght = 0;
    //Boucle permettant de calculer le nombre de lignes et colonnes du tableau
    while((c = fgetc(fichiertxt)) != EOF){
      if(c == '\n'){
        lines++;
        if(lines != 1){
          if(lenght != current_line_lenght){
              printf("Line %i is %i while %i is %i\n", lines-1, lenght, lines, current_line_lenght);
              errx(1,"Error : Grid does not have same length everywhere");
            }
          current_line_lenght = 0;
        }else{
          lenght = current_line_lenght;
          current_line_lenght = 0;
        }
      }else{
      current_line_lenght++;
        }
      }
    printf("Grid is a %i x %i \n", lines, lenght);
    
    char lignes[lines][lenght+1];
    printf("Transforming the file in an array\n");
    //Reviens en haut du fichier
    rewind(fichiertxt);
    //Chope chaque ligne du fichier et les mets dans le tableau
    for(int i = 0;i<lines;i++){
      fgets(lignes[i], lenght+2, fichiertxt);
    }
    printf("Array succesfully created\nHere is the array:\n");
    //CHECKING ARRAY
    printf("[");
    for(int i = 0;i<lines;i++){
      printf("[ ");
      for (int j = 0;j<lenght;j++){
        printf(" %c ", lignes[i][j]);
      }
      if(i == lines-1){printf("] ] \n");}else{printf("]\n");}
    }
    //Boucles pour chercher le mot
    int wordlen = strlen(wordtofind);
    Coordinates coordonnes = {-1, 0, 0, 0};
    for (int i = 0; i < lines; i++) {
    for (int j = 0; j < lenght; j++) {
      // Cherhcer le mot à l' horizontal
      if (j <= lenght - wordlen) {
        int trouve = 1;
        for (int k = 0; k < wordlen; k++) {
          if (lignes[i][j + k] != wordtofind[k]) {
            trouve = 0;
            break;
          }
          printf("Possible horizontal match on (%i,%i)\n",j+k,i);
        }
        if (trouve) {
          coordonnes = (Coordinates){j, i, j + wordlen - 1, i};
          printf("It's a horizontal match ! on (%i,%i)(%i,%i)\n",j, i, j + wordlen - 1, i);
          printf("Closing the file %s\n", textfile);
          fclose(fichiertxt);
          printf("Closed the file %s\n", textfile);
          return coordonnes;
          }
      }
      
      // Chercher le mot à la verticale
      if (i <= lines - wordlen) {
        int trouve = 1;
        for (int k = 0; k < wordlen; k++) {
          if (lignes[i + k][j] != wordtofind[k]) {
            trouve = 0;
            break;
          }
          printf("Possible vertical match on (%i,%i)\n",j,i+k);
        }
        if (trouve) {
          coordonnes = (Coordinates){j, i, j, i + wordlen - 1};
          printf("It's a vertical match ! on (%i,%i)(%i,%i)\n",j, i, j, i + wordlen - 1);
          printf("Closing the file %s\n", textfile);
          fclose(fichiertxt);
          printf("Closed the file %s\n", textfile);
          return coordonnes;
        }
      }
            
      // Chercher en diagonale
      if (i <= lines - wordlen && j <= lenght - wordlen) {
        int trouve = 1;
        for (int k = 0; k < wordlen; k++) {
          if (lignes[i + k][j + k] != wordtofind[k]) {
            trouve = 0;
            break;
          }
          printf("Possible diagonal match on (%i,%i)\n",j+k,i+k);
        }
        if (trouve) {
          coordonnes = (Coordinates){j, i, j + wordlen - 1, i + wordlen - 1};
          printf("It's a diagonal match ! on (%i,%i)(%i,%i)\n",j, i, j + wordlen - 1, i + wordlen - 1);
          printf("Closing the file %s\n", textfile);
          fclose(fichiertxt);
          printf("Closed the file %s\n", textfile);
          return coordonnes;
        }
      }
      
      // Diagonal à l'envers
      if (i >= wordlen - 1 && j <= lenght - wordlen) {
          int trouve = 1;
          for (int k = 0; k < wordlen; k++) {
              if (lignes[i - k][j + k] != wordtofind[k]) {
                  trouve = 0;
                  break;
              }
              printf("Possible bottom-left to top-right diagonal match on (%i,%i)\n", j + k, i - k);
          }
          if (trouve) {
              coordonnes = (Coordinates){j, i, j + wordlen - 1, i - wordlen + 1};
              printf("It's a bottom-left to top-right diagonal match! on (%i,%i)(%i,%i)\n", j, i, j + wordlen - 1, i - wordlen + 1);
              printf("Closing the file %s\n", textfile);
              fclose(fichiertxt);
              printf("Closed the file %s\n", textfile);
              return coordonnes;
          }
      }

      // Diagonal mirroir
      if (i <= lines - wordlen && j >= wordlen - 1) {
          int trouve = 1;
          for (int k = 0; k < wordlen; k++) {
              if (lignes[i + k][j - k] != wordtofind[k]) {
                  trouve = 0;
                  break;
              }
              printf("Possible top-right to bottom-left diagonal match on (%i,%i)\n", j - k, i + k);
          }
          if (trouve) {
              coordonnes = (Coordinates){j, i, j - wordlen + 1, i + wordlen - 1};
              printf("It's a top-right to bottom-left diagonal match! on (%i,%i)(%i,%i)\n", j, i, j - wordlen + 1, i + wordlen - 1);
              printf("Closing the file %s\n", textfile);
              fclose(fichiertxt);
              printf("Closed the file %s\n", textfile);
              return coordonnes;
          }
      }

      // Diagonale à l'envers
      if (i >= wordlen - 1 && j >= wordlen - 1) {
          int trouve = 1;
          for (int k = 0; k < wordlen; k++) {
              if (lignes[i - k][j - k] != wordtofind[k]) {
                  trouve = 0;
                  break;
              }
              printf("Possible bottom-right to top-left diagonal match on (%i,%i)\n", j - k, i - k);
          }
          if (trouve) {
              coordonnes = (Coordinates){j, i, j - wordlen + 1, i - wordlen + 1};
              printf("It's a bottom-right to top-left diagonal match! on (%i,%i)(%i,%i)\n", j, i, j - wordlen + 1, i - wordlen + 1);
              printf("Closing the file %s\n", textfile);
              fclose(fichiertxt);
              printf("Closed the file %s\n", textfile);
              return coordonnes;
          }
      }

      // Horizontal à l'envers
      if (j >= wordlen - 1) {
          int trouve = 1;
          for (int k = 0; k < wordlen; k++) {
              if (lignes[i][j - k] != wordtofind[k]) {
                  trouve = 0;
                  break;
              }
              printf("Possible backwards horizontal match on (%i,%i)\n", j - k, i);
          }
          if (trouve) {
              coordonnes = (Coordinates){j, i, j - wordlen + 1, i};
              printf("It's a backwards horizontal match! on (%i,%i)(%i,%i)\n", j, i, j - wordlen + 1, i);
              printf("Closing the file %s\n", textfile);
              fclose(fichiertxt);
              printf("Closed the file %s\n", textfile);
              return coordonnes;
          }
      }

      // Verical à l'envers
      if (i >= wordlen - 1) {
          int trouve = 1;
          for (int k = 0; k < wordlen; k++) {
              if (lignes[i - k][j] != wordtofind[k]) {
                  trouve = 0;
                  break;
              }
              printf("Possible backwards vertical match on (%i,%i)\n", j, i - k);
          }
          if (trouve) {
              coordonnes = (Coordinates){j, i, j, i - wordlen + 1};
              printf("It's a backwards vertical match! on (%i,%i)(%i,%i)\n", j, i, j, i - wordlen + 1);
              printf("Closing the file %s\n", textfile);
              fclose(fichiertxt);
              printf("Closed the file %s\n", textfile);
              return coordonnes;
          }
      }

    
    }
  }
    
    printf("Closing the file %s\n", textfile);
    fclose(fichiertxt);
    printf("Closed the file %s\n", textfile);
    return(Coordinates){-1, 0, 0, 0};
}

