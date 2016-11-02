/**
 * Pour compiler :  gcc *.c -o prog `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags cairo`
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <errno.h>
 #include <string.h>
 #include "data.h"
 #include "parcours_list.h"
 #include "traitement-donnees.h"


 int main (int argc, char * argv[]){
    dataPoint *m1,*m2,* m;
    parcours * L;
    FILE * data;

    //---lecture des données geolog-----
    data=fopen("geolog.txt","r");

    if(data == NULL){
       perror("Fail open data");
       exit(EXIT_FAILURE);
    }

    L=readData(data);

    printf("Affichage d'un point d'interet:\n");
    displayData(L->next->pt);

    printf("---- Affichage liste\n");
    displayList(L);
    printf("---- FIN LISTE\n");

    printf("Distance entre les 2 1ers points : %lf \n", distanceBtwnPoints(L->next->pt, L->next->next->pt));

    fclose(data);
    destroyList(L);

    LambertToGPS(620130,6681057);

    return 0;
 }
