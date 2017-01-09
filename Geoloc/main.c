/**
 * Pour compiler :  gcc *.c -o prog `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags cairo`
 */
#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "data.h"
#include "parcours_list.h"
#include "traitement-donnees.h"
#include "graphic.h"


//#define M_PI 3.14

void traitementDonnees(){
    /*
     * Traitement des données
     */
    parcours * L;

    dataPoint *m1,*m2,* m;
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

    //LambertToGPS(620130,6681057);
}

void graphic(){

    /*
     * Partie graphique
     */


    


}


int main(int argc, char *argv[]){

    parcours * L;

    dataPoint *m1, *m2, *m;
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

    printf("Test adapt 1er pt\n");
    adaptLocation(L->next->pt->longitude, L->next->pt->latitude);
    printf("Adapt 2 2eme point \n");
    adaptLocation(L->next->next->pt->longitude, L->next->next->pt->latitude);

    fclose(data);
    //destroyList(L);

    //LambertToGPS(620130,6681057);

    m1 = newPoint(0, 47.087869, 2.382652);

    setPosition(L->next->next->pt, m1);

    //traitementDonnees(); //Init structure et test

    GtkWidget *window;
    GtkWidget *darea;

    glob.count = 0;

    gtk_init(&argc, &argv);

    graphic(); //Partie graphique

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);
    
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    displayList(L); 

    g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), L);


    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Geoloc");

    gtk_widget_show_all(window);

    gtk_main();

    destroyList(L);


    return 0;
}

