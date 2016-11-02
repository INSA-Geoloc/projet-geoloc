#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "data.h"
#include "parcours_list.h"
#include "graphic.h"

#define M_PI 3.14


int main(int argc, char *argv[]){
    GtkWidget *window;
    GtkWidget *darea;

    dataPoint *m1,*m2,* m;
    parcours * L;
    FILE * data;

    glob.count = 0;

    /*
     * Traitement des données
     */

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



    /*
     * Partie graphique
     */


    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);
    
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Lines");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

