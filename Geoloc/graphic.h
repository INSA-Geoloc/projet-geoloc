#ifndef   _GRAPHIC_H
#define   _GRAPHIC_H

#include <math.h>

struct {
    int count;
    double coordx[100];
    double coordy[100];
} glob;

//#define M_PI 3.14

void do_drawing(cairo_t *);

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);

gboolean setPoint(GtkWidget *widget, double xp, double yp, int isLambert);

gboolean setCircle(GtkWidget *widget, double xc, double yc, double taille);

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

void enter_button(GtkWidget *widget, gpointer data);

void adaptLocation(double longitude, double latitude);

#endif