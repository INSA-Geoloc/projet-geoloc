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
#include <unistd.h>

parcours * original_data = NULL;
parcours * img_point_data;
GtkWidget *darea;
GtkWidget *window;
parcours * animated_data = NULL;
parcours *deleted_data = NULL;

/*
* Structure pour les filtres tous les filters sont implémentés dans graphic.h
* Toutes les valeurs sont initialisés à 0 par défaut
* Fonctionnement booléen : 0 les données ne sont pas à afficher | 1 les données sont à afficher
*/
menuFilters filters;

//Sous menu
GtkWidget *pointsDisplayMi;
GtkWidget *routesDisplayMi;
GtkWidget *pointInteretDisplayMi;
GtkWidget *cercleAnonymatDisplayMi;


void pointsDisplayMiEvent (GtkWidget *widget, gpointer *data)
{
	int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des points et que des données ont été chargées
  if(isactive == 1 && original_data != NULL)
    filters.displayPoints = 1;
  //Si on active l'affichage des loints mais que aucune données n'ont été chargées
  else if(isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget* dialog = gtk_message_dialog_new (window ,
                                   flags,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "Impossible d'afficher les points, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_check_menu_item_set_active(pointsDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des points
  else {
    filters.displayPoints = 0;
		gtk_check_menu_item_set_active(routesDisplayMi, 0);
    gtk_check_menu_item_set_active(pointInteretDisplayMi, 0);
    gtk_check_menu_item_set_active(cercleAnonymatDisplayMi, 0);
  }

	gtk_widget_queue_draw(darea);
}

void routesDisplayMiEvent (GtkWidget *widget, gpointer *data)
{
  int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des routes et que des données ont été chargées
  if(isactive == 1 && original_data != NULL){
		//Si on affiche les routes on affiche aussi les points et donc coche la checkbox "affichage des points"
		filters.displayPoints = 1;
		gtk_check_menu_item_set_active(pointsDisplayMi, 1);
    filters.displayRoutes = 1;

  }
  //Si on active l'affichage des routes mais que aucune données n'ont été chargées
  else if(isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget* dialog = gtk_message_dialog_new (window ,
                                   flags,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "Impossible d'afficher les routes, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_check_menu_item_set_active(routesDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des routes
  else {
    filters.displayRoutes = 0;
  }

  gtk_widget_queue_draw(darea);
}

void pointInteretDisplayMiEvent (GtkWidget *widget, gpointer *data)
{
  int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des routes et que des données ont été chargées
  if(isactive == 1 && original_data != NULL){
    //Si on affiche les routes on affiche aussi les points et donc coche la checkbox "affichage des points"
    filters.displayIPoints = 1;
    gtk_check_menu_item_set_active(pointInteretDisplayMi, 1);
    filters.displayIPoints = 1;

  }
  //Si on active l'affichage des routes mais que aucune données n'ont été chargées
  else if(isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget* dialog = gtk_message_dialog_new (window ,
                                   flags,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "Impossible d'afficher les points d'interet, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_check_menu_item_set_active(pointInteretDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des routes
  else {
    filters.displayIPoints = 0;
  }

  gtk_widget_queue_draw(darea);
}

void cercleAnonymatDisplayMiEvent (GtkWidget *widget, gpointer *data)
{
  int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des routes et que des données ont été chargées
  if(isactive == 1 && original_data != NULL){
    //Si on affiche les routes on affiche aussi les points et donc coche la checkbox "affichage des points"
    filters.displayCircles = 1;
    gtk_check_menu_item_set_active(cercleAnonymatDisplayMi, 1);
    filters.displayCircles = 1;

  }
  //Si on active l'affichage des routes mais que aucune données n'ont été chargées
  else if(isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget* dialog = gtk_message_dialog_new (window ,
                                   flags,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "Impossible d'afficher les cercles, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_check_menu_item_set_active(cercleAnonymatDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des routes
  else {
    filters.displayCircles = 0;
  }

  gtk_widget_queue_draw(darea);
}

void playMiEvent (GtkWidget *widget, gpointer *data)
{
  gtk_widget_queue_draw(widget);
  filters.displayPoints = 0;
  gtk_check_menu_item_set_active(pointsDisplayMi, 0);

    if(!animated_data->next){
      printf("Tu as deja fait une animation\n");
      animated_data = img_point_data;
    }
    g_timeout_add(100, G_CALLBACK(animatePath), NULL);

  gtk_widget_queue_draw(darea);
}

void on_data_loaded()
{
  GtkResponseType result;
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget* dialog = gtk_message_dialog_new (window ,
                                 flags,
                                 GTK_MESSAGE_QUESTION,
                                 GTK_BUTTONS_YES_NO,
                                 "Les données ont été chargées, voulez-vous afficher les points ?");

  result = gtk_dialog_run(GTK_DIALOG (dialog));

  gtk_widget_destroy(dialog);
  if(result == GTK_RESPONSE_YES)
  {
    filters.displayPoints = 1;
    gtk_check_menu_item_set_active(pointsDisplayMi, 1);
  }
  else if(result == GTK_RESPONSE_NO)
  {
    filters.displayPoints = 0;
  }
}

void load_Data(char * filename)
{

  FILE* file;
  file = fopen(filename, "r");
  if(file == NULL)
  {
     perror("Fail open data");
     exit(EXIT_FAILURE);
  }
  original_data = readData(file);
  deleted_data = initParcours();
	GPStoLambertList(); //Conversion des données GPS en Lambert 93
	img_point_data = LambertToImg();
  animated_data = img_point_data;
  fclose(file);
}

void choose_File(GtkWidget *item, gpointer data)
{
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  char *filename;

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

    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
    filename = gtk_file_chooser_get_filename (chooser);

    load_Data(filename);
    g_free (filename);
  }
	gtk_widget_destroy (dialog);
	if (res == GTK_RESPONSE_ACCEPT)
	{
		on_data_loaded();
	}
}


int main(int argc, char *argv[]) {

  //GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *grid;

  GtkWidget *swindow;
  GtkWidget *viewport;

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
  //GtkWidget *pointsDisplayMi;
  //GtkWidget *routesDisplayMi;
  //GtkWidget *pointInteretDisplayMi;
  //GtkWidget *cercleAnonymatDisplayMi;

  //Annimations
  GtkWidget *playMi;

  glob.count = 0;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
  gtk_window_set_title(GTK_WINDOW(window), "Geoloc");


  vbox = gtk_box_new(FALSE, 0);
  //gtk_container_add(GTK_CONTAINER(window), vbox);

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
  pointsDisplayMi = gtk_check_menu_item_new_with_label("Affichage des points");
  routesDisplayMi = gtk_check_menu_item_new_with_label("Affichage des routes");
  pointInteretDisplayMi = gtk_check_menu_item_new_with_label("Affichage des points d'interêts");
  cercleAnonymatDisplayMi = gtk_check_menu_item_new_with_label("Affichage des cercle d'anonymat");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(displayMi), displayMenu);

  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), pointsDisplayMi);
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

  /////////////////////////////////////////////////
  //Zone cartograĥique
  /////////////////////////////////////////////////


  //GtkWidget *darea;
  darea = gtk_drawing_area_new();
  gtk_widget_set_size_request (darea, 1200, 743);


  grid = gtk_grid_new();
  //Ajout
  /*swindow = gtk_scrolled_window_new (NULL,NULL); //De base valeur à modifier
  viewport = gtk_viewport_new (NULL,NULL); //De base valeur par défaut ce sont les valeurs à modifier
  //GtkAdjustment *gtk_scrollable_get_hadjustment (GtkScrollable *scrollable);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_grid_attach(GTK_GRID(grid), swindow, 0, 1, 400, 200);

  gtk_container_add (GTK_CONTAINER(viewport), darea);
  //gtk_container_add (GTK_CONTAINER(swindow), viewport);
  gtk_container_add (GTK_CONTAINER(swindow), viewport);
  //gtk_grid_attach (GTK_GRID(grid), swindow, 0, 1, 1, 2);*/
  //Fin ajout
  gtk_container_add(GTK_CONTAINER(window), grid);


  gtk_grid_attach (GTK_GRID (grid), vbox, 0, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (grid), darea, 0, 1, 1, 1);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), original_data);

 //////////////////////////////////////////////////

  //Linkage des fonctions
  g_signal_connect(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(quitMi), "activate",
        G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(openMi), "activate",
        G_CALLBACK(choose_File), NULL);

  //Cocher au demrrage affichage des routes
  //gtk_check_menu_item_set_active(pointsDisplayMi, 1);
  g_signal_connect (G_OBJECT (pointsDisplayMi), "toggled",
      	G_CALLBACK (pointsDisplayMiEvent), pointsDisplayMi);

  g_signal_connect (G_OBJECT (routesDisplayMi), "toggled",
        G_CALLBACK (routesDisplayMiEvent), routesDisplayMi);

  g_signal_connect (G_OBJECT (pointInteretDisplayMi), "toggled",
        G_CALLBACK (pointInteretDisplayMiEvent), pointInteretDisplayMi);

  g_signal_connect (G_OBJECT (cercleAnonymatDisplayMi), "toggled",
        G_CALLBACK (cercleAnonymatDisplayMiEvent), cercleAnonymatDisplayMi);

  g_signal_connect (G_OBJECT (playMi), "activate",
        G_CALLBACK (playMiEvent), playMi);




  //Affichage de la fenêtre

//gtk_window_maximize (GTK_WINDOW (p_window));

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
