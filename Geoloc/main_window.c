#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "data.h"
#include "parcours_list.h"
#include "traitement-donnees.h"
#include "graphic.h"



parcours * load_Data(char * filename)
{
  FILE* file;
  parcours * list;

  file = fopen(filename, "r");
  if(file == NULL)
  {
     perror("Fail open data");
     exit(EXIT_FAILURE);
  }

  list = readData(file);
  return list;
}

static void choose_File( GtkWidget *item, gpointer data)
{
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  dialog = gtk_file_chooser_dialog_new ("Open File",
                                      NULL,
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    parcours * data;
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
    filename = gtk_file_chooser_get_filename (chooser);
    

    data = load_Data(filename);
    displayList(data);
    
    g_free (filename);
  }

  gtk_widget_destroy (dialog);
}




int main(int argc, char *argv[]) {

  GtkWidget *window;
  GtkWidget *vbox;

  GtkWidget *menubar1;
  GtkWidget *menubar2;
  GtkWidget *menubar3;


  //Menu niveau 1
  //Fichier
  GtkWidget *fileMenu;
  GtkWidget *fileMi;
  
  //Affichage
  GtkWidget *displayMenu;
  GtkWidget *displayMi;

  //Animations
  GtkWidget *animationMenu;
  GtkWidget *animationMi;

  //Menu niveau 2 
  //Fichier
  GtkWidget *quitMi;
  GtkWidget *openMi;

  //Affichage
  GtkWidget *routesDisplayMi;
  GtkWidget *pointInteretDisplayMi;
  GtkWidget *cercleAnonymatDisplayMi;

  //Annimations
  GtkWidget *playMi;
  
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
  gtk_window_set_title(GTK_WINDOW(window), "Geoloc");

  vbox = gtk_box_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  menubar1 = gtk_menu_bar_new();
  menubar2 = gtk_menu_bar_new();
  menubar3 = gtk_menu_bar_new();

  //Fichier
  fileMenu = gtk_menu_new();
  
  fileMi = gtk_menu_item_new_with_label("Fichier");
  openMi = gtk_menu_item_new_with_label("Ouvrir");
  quitMi = gtk_menu_item_new_with_label("Quitter");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);

  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
  
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar1), fileMi);

  gtk_box_pack_start(GTK_BOX(vbox), menubar1, FALSE, FALSE, 0);

  //Affichage
  displayMenu = gtk_menu_new();
  displayMi = gtk_menu_item_new_with_label("Affichage");
  routesDisplayMi = gtk_check_menu_item_new_with_label("Affichage des routes");
  pointInteretDisplayMi = gtk_check_menu_item_new_with_label("Affichage des points d'interêts");
  cercleAnonymatDisplayMi = gtk_check_menu_item_new_with_label("Affichage des cercle d'anonymat");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(displayMi), displayMenu);

  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), routesDisplayMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), pointInteretDisplayMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), cercleAnonymatDisplayMi);

  gtk_menu_shell_append(GTK_MENU_SHELL(menubar2), displayMi);
  
  gtk_box_pack_start(GTK_BOX(vbox), menubar2, FALSE, FALSE, 0);

  //Animations
  animationMenu = gtk_menu_new();
  animationMi = gtk_menu_item_new_with_label("Animations");
  playMi = gtk_menu_item_new_with_label("Play");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(animationMi), animationMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(animationMenu), playMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar3), animationMi);
  gtk_box_pack_start(GTK_BOX(vbox), menubar3, FALSE, FALSE, 0);


  //Linkage des fonctions 
  g_signal_connect(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(quitMi), "activate",
        G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(openMi), "activate",
        G_CALLBACK(choose_File), NULL);


  //Affichage de la fenêtre
  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
