#ifndef   _GRAPHIC_H
#define   _GRAPHIC_H

#include <math.h>

struct {
    int count;
    double coordx[100];
    double coordy[100];
} glob;

typedef struct {
	int displayPoints;
	int displayRoutes;
}menuFilters;

//#define M_PI 3.14

void do_drawing(cairo_t *);

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);
gboolean animatePath();

gboolean animateTest(int tps);

gboolean setPoint(GtkWidget *widget, double xp, double yp, int isLambert);

gboolean setCircle(GtkWidget *widget, double xc, double yc, double taille);

gboolean setLabel(GtkWidget *widget, double xl, double yl, char* text);

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

void enter_button(GtkWidget *widget, gpointer data);

void adaptLocation(double longitude, double latitude);

#endif