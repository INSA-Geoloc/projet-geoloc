#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "data.h"
#include "parcours_list.h"
#include "traitement-donnees.h"
#include "graphic.h"

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    do_drawing(cr);

    return FALSE;
}

void do_drawing(cairo_t *cr)
{

    // HACK JFL: image draw test
    int w, h;
    cairo_surface_t *image;

    cairo_set_source_rgb(cr, 0, 0, 0);
    //cairo_set_line_width(cr, 0.5);



    image = cairo_image_surface_create_from_png ("france_wikipedia.png");
    w = cairo_image_surface_get_width (image);
    h = cairo_image_surface_get_height (image);
    cairo_scale(cr, 512.0/w, 512.0/h);


    cairo_set_source_surface (cr, image, 0, 0);
    cairo_paint(cr);
    // For drawing lines when glob has evolved
    int i, j;
    for (i = 0; i <= glob.count - 1; i++ ) {
        for (j = 0; j <= glob.count - 1; j++ ) {
            cairo_move_to(cr, glob.coordx[i], glob.coordy[i]);
            cairo_line_to(cr, glob.coordx[j], glob.coordy[j]);
        }
    }

    glob.count = 0;
    cairo_stroke(cr);
}

gboolean setPoint(GtkWidget *widget, double xp, double yp, int isLambert){
    cairo_t *cr;

    cr = gdk_cairo_create(gtk_widget_get_window(widget));

    cairo_arc(cr, xp, yp, 10.0, 0, 2*M_PI);

    cairo_close_path(cr);

    cairo_set_line_width(cr, 2.0); //Border weight

    if(isLambert){
        printf("Cest du lambert %lf / %lf\n", xp, yp);
        /*Ref sur la carte en x,y*/
        double xRef = 20;
        double yRef = 20;

        double xLRef = 340;
        double yLRef = 450;

        double tmpX = (xRef * xp)/xLRef;
        double tmpY = (yRef * yp)/yLRef;

        xp = tmpX;
        yp = tmpY;

        cairo_set_source_rgba(cr, 1, 0.8, 0.8, 1); //Fill colo

        
    }
    else{
        cairo_set_source_rgba(cr, 1, 0.2, 0.2, 1); //Fill colo
    }

    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.8); //Border color

    cairo_stroke(cr);

    cairo_destroy(cr);

    return TRUE;
}

gboolean setCircle(GtkWidget *widget, double xc, double yc, double taille){
    cairo_t *cr;

    cr = gdk_cairo_create(gtk_widget_get_window (widget));

    double radius = taille;
    double angle1 = 0  * (M_PI/180.0);  // angles are specified 
    double angle2 = 360.0 * (M_PI/180.0);  // in radians 

    cairo_set_line_width(cr, 10.0);
    cairo_arc(cr, xc, yc, radius, angle1, angle2);

    cairo_close_path(cr);

    cairo_set_line_width(cr, 5.0); //Border wight
    cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.2); //Fill colo
    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 1, 0.2, 0.2, 1); //Border color

    cairo_stroke(cr);

    cairo_destroy(cr);

    return TRUE;
} 

gboolean setPath(GtkWidget *widget, parcours* lp){
		parcours * tmp = lp->next;

		cairo_t *cr;

    	cr = gdk_cairo_create(gtk_widget_get_window(widget));

  		while(tmp->pt != NULL){


            setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 0);

            cairo_move_to(cr, tmp->pt->longitude, tmp->pt->latitude);
            if(tmp->next) cairo_line_to(cr, tmp->next->pt->longitude, tmp->next->pt->latitude); else break;
            cairo_stroke(cr);

            setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 1); //Hack point au dessus des lignes

            if(tmp->next) setPoint(widget, tmp->next->pt->longitude, tmp->next->pt->latitude, 0); else break;

            if(tmp->next) tmp = tmp->next; else break;
        }

}

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{

	cairo_t *cr;

    cr = gdk_cairo_create (gtk_widget_get_window (widget));

    printf("clicked\n");
    if (event->button == 1) {
        glob.coordx[glob.count] = event->x;
        glob.coordy[glob.count++] = event->y;
    }

    if (event->button == 3) {
        gtk_widget_queue_draw(widget);
        return TRUE;
    }

    //setTest(widget);

    do_drawing(cr);

    cairo_scale(cr, 1.55, 1.55); //Trouver le bon calcul à chaque fois

    (parcours*) user_data;

    setPath(widget, (parcours*) user_data);

    /*parcours * L;
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

    fclose(data);
    destroyList(L);*/
    

    /*double dashes[] = {50.0,  // ink
                   10.0,  // skip
                   10.0,  // ink
                   10.0   // skip
                  };
    int ndash  = sizeof (dashes)/sizeof(dashes[0]);
    double offset = -50.0;

    //cairo_set_dash (cr, dashes, ndash, offset);
    cairo_set_line_width (cr, 2.0);

    cairo_move_to (cr, 128.0, 25.6);
    cairo_line_to (cr, 230.4, 230.4);
    cairo_rel_line_to (cr, -102.4, 0.0);
    cairo_curve_to (cr, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);

    cairo_stroke (cr);*/

    //Cercle
    /*double xc = 128.0;
    double yc = 128.0;
    double radius = 100.0;
    double angle1 = 0  * (M_PI/180.0);  // angles are specified 
    double angle2 = 360.0 * (M_PI/180.0);  // in radians 

    cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.8);

    cairo_set_line_width (cr, 10.0);
    cairo_arc (cr, xc, yc, radius, angle1, angle2);

    cairo_stroke (cr);
    */

    //Parcours
    //setPoint(widget, 120, 120);
    /*cairo_set_line_width (cr, 4.0);
    cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.8);

    setPoint(widget, 0, 0, 0);

    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, 120, 120);
    cairo_stroke(cr);

    setPoint(widget, 120, 120, 0);

    cairo_move_to(cr, 120, 120); //Pt depart
    cairo_line_to(cr, 256, 0); //Pt arrivee (direction) translation horizontal + translation vertical

    setPoint(widget, 256, 0, 0);

    cairo_move_to(cr, 256, 0);
    cairo_line_to(cr, 234, 176);

    setPoint(widget, 234, 176, 0);

    setCircle(widget, 234, 176, 100);

    //setTest(widget);

    cairo_stroke(cr);

    cairo_text_extents_t extents;

	const char *utf8 = "INSA";
	double x,y;

	cairo_select_font_face (cr, "Sans",
	    CAIRO_FONT_SLANT_NORMAL,
	    CAIRO_FONT_WEIGHT_NORMAL);

	cairo_set_font_size (cr, 12.0);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.8);
	cairo_text_extents (cr, utf8, &extents);

	x=234;
	y=176;

	cairo_move_to (cr, x,y);
	cairo_show_text (cr, utf8);*/

	cairo_destroy(cr);


    return TRUE;
}

void enter_button(GtkWidget *widget, gpointer data) {
	printf("Mon test\n");
}

void adaptLocation(double xL, double latitude){
    double x, y, bx, by;
    double r  = 6371000;
    double cx = 0;
    double cy = 0;





    x = r*sin(latitude)*cos(xL);
    y = r*sin(latitude)*sin(latitude); 
    printf("X : %lf | Y : %lf \n", x, y);
}

