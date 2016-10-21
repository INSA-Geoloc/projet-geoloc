#include <stdio.h>
#include <cairo/cairo.h>
#include <gtk/gtk.h>

#include "traitement-donnees.h"

static void do_drawing(cairo_t *);

struct {
    int count;
    double coordx[100];
    double coordy[100];
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
                              gpointer user_data)
{
    do_drawing(cr);
    
    return FALSE;
}


static void do_drawing(cairo_t *cr)
{
    
    // HACK JFL: image draw test
    int              w, h;
    cairo_surface_t *image;
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 0.5);
    
    
    
    image = cairo_image_surface_create_from_png("france_wikipedia.png");
    w = cairo_image_surface_get_width (image);
    h = cairo_image_surface_get_height (image);
    cairo_scale (cr, 256.0/w, 256.0/h);
    
    
    cairo_set_source_surface (cr, image, 0, 0);
    cairo_paint (cr);
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

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
                        gpointer user_data)
{
    printf("clicked\n");
    if (event->button == 1) {
        glob.coordx[glob.count] = event->x;
        glob.coordy[glob.count++] = event->y;
    }
    
    if (event->button == 3) {
        gtk_widget_queue_draw(widget);
    }
    
    return TRUE;
}



int main(int argc, char *argv[])
{
    
    //GPStoLambert();
    LambertToGPS(620130,6681057);
    
    
    GtkWidget *window;
    GtkWidget *darea;
    
    glob.count = 0;
    
    gtk_init(&argc, &argv);
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);
    
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
    
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);
    
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_title(GTK_WINDOW(window), "Lines");
    
    gtk_widget_show_all(window);
    
    gtk_main();
    
    return 0;
}
