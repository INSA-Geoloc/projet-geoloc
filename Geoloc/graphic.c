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

extern parcours *original_data;
extern parcours * img_point_data;
extern GtkWidget *darea;
extern menuFilters filters;
extern parcours *animated_data;
dataPoint *animated_point;
extern parcours *deleted_data;
extern dataPoint *deleting_point;

cairo_surface_t *image = NULL;

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    do_drawing(cr);

    return FALSE;
}


/**
 * @brief Fonction d'affichage de point pour la visualitation dynamique
 */

gboolean animatePath(){
    cairo_t *cr;
    cr = gdk_cairo_create(gtk_widget_get_window(darea));

    if(filters.stopAnimation == 1){
        return FALSE; //On stoppe l'animation
    }

    if(!animated_data){
        printf("Erreur animated_data\n");
    }else{
        if(animated_data->pt){ //Point dans le parcours

            if(animated_point != NULL){
                cairo_move_to(cr, animated_point->longitude, animated_point->latitude);
                cairo_line_to(cr, animated_data->pt->longitude, animated_data->pt->latitude);
                cairo_stroke(cr);

                setPoint(darea, animated_point->longitude, animated_point->latitude, 0); //Hack point au dessus des lignes
            }
            setPoint(darea, animated_data->pt->longitude, animated_data->pt->latitude, 0);
            animated_point = animated_data->pt; //Sauvegarde du point pour les tracés
        }else { //1er point du parcours
            setPoint(darea, animated_data->next->pt->longitude, animated_data->next->pt->latitude, 0);
            animated_point = animated_data->next->pt;
        }
    }

    if(animated_data->next){
        animated_data = animated_data->next;
    }else{
        printf("Fin animation\n");
        return FALSE;
    }
}

void do_drawing(cairo_t *cr)
{
    // HACK JFL: image draw test
    int w, h;
    //cairo_surface_t *image = NULL;

    cairo_set_source_rgb(cr, 0, 0, 0);
    //cairo_set_line_width(cr, 0.5);

    if(image == NULL)
    {
      image = cairo_image_surface_create_from_png ("map_17.png");
    }
    w = cairo_image_surface_get_width (image);
    h = cairo_image_surface_get_height (image);
    cairo_scale(cr, 1200.0/w, 743.0/h);


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
    if(original_data != NULL && original_data->next != NULL && filters.displayPoints == 1)
    {
    	setPath(darea, img_point_data, filters.displayRoutes); //Affichage de notre parcours
    }
    if(original_data != NULL && deleted_data->next != NULL && filters.displayPoints == 1 && filters.displayDeletedPoints)
    {
        setPath(darea, deleted_data, 3); //Affichage des points supprimes
    }
    cairo_stroke(cr);
}

/**
 * @brief Creation d'un point sur la carte
 * @param widget conteneur pour le dessin
 * @param xp position en x ou placer le point
 * @param yp position en y ou placer le point
 * @param pointType type de point a afficher
 */

gboolean setPoint(GtkWidget *widget, double xp, double yp, int pointType){
    cairo_t *cr;

    cr = gdk_cairo_create(gtk_widget_get_window(widget));

    switch (pointType){
        case 0: //Point classique
            cairo_arc(cr, xp, yp, 5.0, 0, 2*M_PI);
            cairo_set_source_rgba(cr, 0, 0.5, 1, 0.8); //Fill color blue
            break;
        case 1: //Point d'une autre couleur
            cairo_arc(cr, xp, yp, 5.0, 0, 2*M_PI);
            cairo_set_source_rgba(cr, 1, 0.8, 0.8, 0.8); //Fill colo
            break;

        case 2: //Point d'interet
            cairo_arc(cr, xp, yp, 20.0, 0, 2*M_PI);
            cairo_set_source_rgba(cr, 0, 0.5, 0.5, 0.8); //Fill colo
            break;
        case 3: //Point supprime
            cairo_arc(cr, xp, yp, 5.0, 0, 2*M_PI);
            cairo_set_source_rgba(cr, 1, 0.2, 0, 0.8); //Fill colo
            break;
        default:
            cairo_arc(cr, xp, yp, 5.0, 0, 2*M_PI);
            cairo_set_source_rgba(cr, 0, 0.5, 1, 0.8); //Fill colo
    }
    cairo_close_path(cr);
    cairo_set_line_width(cr, 1.0); //Border weight

    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 0, 0, 0, 0.8); //Border color

    cairo_stroke(cr);

    cairo_destroy(cr);

    return TRUE;
}


/**
 * @brief Creation d'un cercle sur la carte
 * @param widget conteneur pour le dessin
 * @param xc position en x ou placer le cercle
 * @param yc position en y ou placer le cercle
 * @param taille le rayon du cercle a afficher
 */

gboolean setCircle(GtkWidget *widget, double xc, double yc, double taille){
    cairo_t *cr;

    cr = gdk_cairo_create(gtk_widget_get_window (widget));

    double radius = taille;
    double angle1 = 0  * (M_PI/180.0);  // angles are specified
    double angle2 = 360.0 * (M_PI/180.0);  // in radians


    cairo_set_line_width(cr, 10.0);
    cairo_arc(cr, xc, yc, radius, angle1, angle2);

    cairo_close_path(cr);

    cairo_set_line_width(cr, 2.0); //Border wight
    cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.1); //Fill colo
    cairo_fill_preserve(cr);
    cairo_set_source_rgba(cr, 1, 0.2, 0.2, 1); //Border color

    cairo_stroke(cr);

    cairo_destroy(cr);

    return TRUE;
}


/**
 * @brief Creation d'un texte sur la carte
 * @param widget conteneur pour le dessin
 * @param xl position en x ou placer le texte
 * @param yl position en y ou placer le texte
 * @param text le texte a ecrire
 */

gboolean setLabel(GtkWidget *widget, double xl, double yl, char* text){

    cairo_t *cr;

    cr = gdk_cairo_create(gtk_widget_get_window(widget));

    cairo_text_extents_t extents;
    char* utf8;
    if(text)
        utf8 = text;
    else
        utf8 = "Rue";

    cairo_select_font_face (cr, "Sans",
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size (cr, 12.0);
    cairo_set_source_rgba (cr, 0, 0, 0, 0.8);
    cairo_text_extents (cr, utf8, &extents);

    cairo_move_to (cr, (xl+10), (yl-10));
    cairo_show_text (cr, utf8);
}


/**
 * @brief Affichage d'un parcours complet de points
 * @param widget conteneur pour le dessin
 * @param lp parcours contenant tous les points a afficher
 * @param showRoutes filtre servant pour afficher different nos points
 */

gboolean setPath(GtkWidget *widget, parcours* lp, int showRoutes){
		parcours * tmp = lp->next;

        if(tmp == NULL){
            printf("Erreur parcours nul\n");
            return FALSE;
        }

		cairo_t *cr;

    	cr = gdk_cairo_create(gtk_widget_get_window(widget));
  		while(tmp->pt != NULL){

            if(showRoutes == 1){
                setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 0);
                cairo_move_to(cr, tmp->pt->longitude, tmp->pt->latitude);
                if(tmp->next) cairo_line_to(cr, tmp->next->pt->longitude, tmp->next->pt->latitude); else break;
                cairo_stroke(cr);
            


                if( strcmp(tmp->pt->adresse,"INTERET")== 0 && filters.displayIPoints){
                    setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 2); //Hack point au dessus des lignes
                    setLabel(widget, (tmp->pt->longitude+10), (tmp->pt->latitude-10), "Point d'interet");
                    if(tmp->next ){
                        if (strcmp(tmp->pt->adresse,"INTERET") == 0 ){
                        setPoint(widget, tmp->next->pt->longitude, tmp->next->pt->latitude, 2); //Hack point au dessus des lignes
                        setLabel(widget, (tmp->next->pt->longitude+10), (tmp->next->pt->latitude-10), "Route d'interet");
                        } else {
                        setPoint(widget, tmp->next->pt->longitude, tmp->next->pt->latitude, 0); //Hack point au dessus des lignes
                        }
                    }
                }
                else{
                    setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 0);
                }

                if(tmp->next) setPoint(widget, tmp->next->pt->longitude, tmp->next->pt->latitude, 0); else break;
            }else{
                if(strcmp(tmp->pt->adresse,"INTERET") == 0 && filters.displayIPoints){
                    setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 2);
                    setLabel(widget, (tmp->pt->longitude+10), (tmp->pt->latitude-10), "Point d'interet");
                }
                else{
                    if(showRoutes == 3){ //pt suppr
                        printf("%lf / %lf\n", tmp->pt->longitude, tmp->pt->latitude);
                        setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 3);
                    }else{
                        setPoint(widget, tmp->pt->longitude, tmp->pt->latitude, 0);    
                    }
                }

                if(filters.editionMode == 1 && strcmp(tmp->pt->adresse,"INTERET") == 0/* && deleting_point == NULL*/){
                    if(deleting_point == NULL){
                        setCircle(widget, tmp->pt->longitude, tmp->pt->latitude, 20);
                        deleting_point = newPoint(tmp->pt->time, tmp->pt->latitude, tmp->pt->longitude);
                    }else if(tmp->pt->time == deleting_point->time){
                        setCircle(widget, tmp->pt->longitude, tmp->pt->latitude, 20);
                    }
                }

            }
            if(tmp->next) tmp = tmp->next; else break;
        }

}

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	cairo_t *cr;

    cr = gdk_cairo_create (gtk_widget_get_window (widget));

    if (event->button == 1) {
        glob.coordx[glob.count] = event->x;
        glob.coordy[glob.count++] = event->y;
        printf("Left Click : %i\n", glob.count);
    }

    if (event->button == 3) {
        printf("Right Click\n");
        gtk_widget_queue_draw(widget);
        //return TRUE;
    }

	cairo_destroy(cr);


    return TRUE;
}
