#ifndef   _GRAPHIC_H
#define   _GRAPHIC_H

#include <math.h>

struct {
    int count;
    double coordx[100];
    double coordy[100];
} glob;

typedef struct {
	/*! Affichage de points*/
	int displayPoints;
	/*! Affichage de routes*/
	int displayRoutes;
	/*! Affichage de points d'interet*/
	int displayIPoints;
	/*! Affichage de points supprimes*/
	int displayDeletedPoints;
	/*! Mode pour la suppression de points*/
	int editionMode;
	/*! Element de confirmation pour l'arret de l'animation*/
	int stopAnimation;
}menuFilters;

//#define M_PI 3.14

void do_drawing(cairo_t *);

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);
gboolean animatePath();

gboolean setPoint(GtkWidget *widget, double xp, double yp, int isLambert);

gboolean setCircle(GtkWidget *widget, double xc, double yc, double taille);

gboolean setLabel(GtkWidget *widget, double xl, double yl, char* text);

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

#endif