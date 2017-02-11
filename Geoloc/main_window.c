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
#include "data.h"
#include "parcours_list.h"
#include "traitement-donnees.h"
#include "graphic.h"
#include <unistd.h>

parcours *original_data = NULL; //Liste des points importés du fichier
parcours *img_point_data; //Liste des poinst affichables sur la carte
GtkWidget *darea; //Zone de dessin
GtkWidget *window; //Fenêtre principale
parcours *animated_data = NULL;
parcours *deleted_data = NULL;

dataPoint *deleting_point; //point contenant un point d'interet a supprimer

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
GtkWidget *deletedPointsDisplayMi;

GtkWidget *anonymatChoiceMi;
GtkWidget *anonymatMi;

/*
 * @brief Appelé lors du click sur "affichage des points"
 * Active ou descative l'indice d'affichage des points
 */
void pointsDisplayMiEvent(GtkWidget *widget, gpointer *data)
{
  int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des points et que des données ont été chargées
  if (isactive == 1 && original_data != NULL)
    filters.displayPoints = 1;
  //Si on active l'affichage des loints mais que aucune données n'ont été chargées
  else if (isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(window,
                                               flags,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Impossible d'afficher les points, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_check_menu_item_set_active(pointsDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des points
  else
  {
    filters.displayPoints = 0;
    gtk_check_menu_item_set_active(routesDisplayMi, 0);
    gtk_check_menu_item_set_active(pointInteretDisplayMi, 0);
    gtk_check_menu_item_set_active(deletedPointsDisplayMi, 0);
  }
  gtk_widget_queue_draw(darea);
}

/*
 * @brief Appelé lors du click sur "affichage des routes"
 * Active ou descative l'indice d'affichage des routes
 */
void routesDisplayMiEvent(GtkWidget *widget, gpointer *data)
{
  int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des routes et que des données ont été chargées
  if (isactive == 1 && original_data != NULL)
  {
    //Si on affiche les routes on affiche aussi les points et donc coche la checkbox "affichage des points"
    filters.displayPoints = 1;
    gtk_check_menu_item_set_active(pointsDisplayMi, 1);
    filters.displayRoutes = 1;
  }
  //Si on active l'affichage des routes mais que aucune données n'ont été chargées
  else if (isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(window,
                                               flags,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Impossible d'afficher les routes, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_check_menu_item_set_active(routesDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des routes
  else
  {
    filters.displayRoutes = 0;
  }
  gtk_widget_queue_draw(darea);
}

/*
 * @brief Appelé lors du click sur "affichage des points d'intérêts"
 * Active ou descative l'indice d'affichage des points d'intérêts
 */
void pointInteretDisplayMiEvent(GtkWidget *widget, gpointer *data)
{
  int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des routes et que des données ont été chargées
  if (isactive == 1 && original_data != NULL)
  {
    //Si on affiche les routes on affiche aussi les points et donc coche la checkbox "affichage des points"
    filters.displayIPoints = 1;
    gtk_check_menu_item_set_active(pointInteretDisplayMi, 1);
    filters.displayIPoints = 1;
  }
  //Si on active l'affichage des routes mais que aucune données n'ont été chargées
  else if (isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(window,
                                               flags,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Impossible d'afficher les points d'interet, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_check_menu_item_set_active(pointInteretDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des routes
  else
  {
    filters.displayIPoints = 0;
  }
  gtk_widget_queue_draw(darea);
}

/*
 * @brief Appelé lors du click sur "supprimer un point d'intérêt"
 * Effectue les traitements nécessaires
 */
void deletedPointsDisplayMiEvent(GtkWidget *widget, gpointer *data)
{
  int isactive = gtk_check_menu_item_get_active(data);
  //Si on active l'affichage des routes et que des données ont été chargées
  if (isactive == 1 && original_data != NULL)
  {
    //Si on affiche les routes on affiche aussi les points et donc coche la checkbox "affichage des points"
    filters.displayDeletedPoints = 1;
    gtk_check_menu_item_set_active(deletedPointsDisplayMi, 1);
    filters.displayDeletedPoints = 1;
  }
  //Si on active l'affichage des routes mais que aucune données n'ont été chargées
  else if (isactive == 1 && original_data == NULL)
  {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(window,
                                               flags,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Impossible d'afficher les cercles, aucune donnée n'a été chargée");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_check_menu_item_set_active(deletedPointsDisplayMi, 0);
    gtk_widget_destroy(dialog);
  }
  //Si on désactive l'affichage des routes
  else
  {
    filters.displayDeletedPoints = 0;
  }
  gtk_widget_queue_draw(darea);
}

/**
 * @brief Fonction d'activation du mode edition avec proposition de points d'interet
 */

void anonymatChoiceMiEvent(GtkWidget *widget, gpointer *data)
{
  if (filters.editionMode != 1)
  {
    GtkResponseType result;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(window,
                                               flags,
                                               GTK_MESSAGE_QUESTION,
                                               GTK_BUTTONS_YES_NO,
                                               "Le mode edition a été activé, un cercle d'anonymisation a été mis sur le point a supprimer pour le supprimer choisir 'Supprimer un point d'interêt' dans le menu.");

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);

    filters.editionMode = 1;
    filters.displayRoutes = 0;
    gtk_check_menu_item_set_active(routesDisplayMi, 0);
    gtk_check_menu_item_set_active(deletedPointsDisplayMi, 0);
  }
  else
  {
    printf("Tu es déjà en mode edition\n");
  }
}

/**
 * @brief Fonction de suppression de points d'interet selectionnes
 */

void anonymatMiEvent(GtkWidget *widget, gpointer *data)
{
  GtkResponseType result;
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget *dialog = gtk_message_dialog_new(window,
                                             flags,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "Le point a supprimer a été encerclé, voulez-vous le supprimer ?");

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);
  if (result == GTK_RESPONSE_YES)
  {
    //Recuperer le point encerclé et le supprimer de img_data pour le mettre dans deleted_data
    printf("Salut j'ai bien suppr ton pt\n");
    if (filters.editionMode == 1 && deleting_point != NULL)
    {
      removePoint(deleting_point, img_point_data);
      addPoint(deleting_point, deleted_data);
      filters.editionMode = 0;
      deleting_point = NULL;
    }
    else
    {
      printf("Action impossible : sorti du mode edition\n");
      filters.editionMode = 0;
      deleting_point = NULL;
    }
  }
}

/*
 * @brief Appelé lors du click sur "Play"
 * Effectue les traitements nécessaires
 */
void playMiEvent(GtkWidget *widget, gpointer *data)
{
  gtk_widget_queue_draw(widget);
  filters.displayPoints = 0;
  gtk_check_menu_item_set_active(pointsDisplayMi, 0);
  if (!animated_data->next || filters.stopAnimation == 1)
  {
    printf("Tu as deja fait une animation\n");
    animated_data = img_point_data;
    filters.stopAnimation = 0;
  }
  g_timeout_add(100, G_CALLBACK(animatePath), NULL); //Fonction appelante a intervalle reguliers

  gtk_widget_queue_draw(darea);
}

/*
 * @brief Appelé lors du click sur "Stop"
 * Effectue les traitements nécessaires
 */
void stopMiEvent(GtkWidget *widget, gpointer *data)
{
  if (filters.stopAnimation == 1)
  {
    printf("deja envoye");
  }
  else
  {
    filters.stopAnimation = 1;
  }

  gtk_widget_queue_draw(darea);
}

/*
 * @brief Ouvre une fenêtre de dialogue demandant à l'utilisateur si il désire afficher les points ou non
 * Effectue les traitements en conséquences
 */
void on_data_loaded()
{
  GtkResponseType result;
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget *dialog = gtk_message_dialog_new(window,
                                             flags,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "Les données ont été chargées, voulez-vous afficher les points ?");

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);
  if (result == GTK_RESPONSE_YES)
  {
    filters.displayPoints = 1;
    gtk_check_menu_item_set_active(pointsDisplayMi, 1);
  }
  else if (result == GTK_RESPONSE_NO)
  {
    filters.displayPoints = 0;
  }
}

/*
 * @brief Ouvre et charge les données du fichier @filename
 * Effectue les principaux traitement sur les données
 */
void load_Data(char *filename)
{
  FILE *file;
  file = fopen(filename, "r");
  if (file == NULL)
  {
    perror("Fail open data");
    exit(EXIT_FAILURE);
  }
  original_data = readData(file);
  deleted_data = initParcours();
  GPStoLambertList(); //Conversion des données GPS en Lambert 93

  cleanRedundantPoints();
  parcours *tmp = original_data->next;
  while (tmp->next != NULL)
  {

    detectInterest(tmp->pt);
    tmp = tmp->next;
  }
  correctInterest();

  img_point_data = LambertToImg(); //Conversion des points en Lambert en points affichable
  deleted_data = LambertToDelImg();
  animated_data = img_point_data;
  fclose(file);
}

/*
 * @brief Appelé lors du click sur "Ouvrir"
 * Ouvre une fenêtre de dialogue pour séléctionner le fichier a Ouvrir
 * Lance le chargement de données
 */
void choose_File(GtkWidget *item, gpointer data)
{
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  char *filename;

  dialog = gtk_file_chooser_dialog_new("Open File",
                                       NULL,
                                       action,
                                       ("_Cancel"),
                                       GTK_RESPONSE_CANCEL,
                                       ("_Open"),
                                       GTK_RESPONSE_ACCEPT,
                                       NULL);

  res = gtk_dialog_run(GTK_DIALOG(dialog));

  if (res == GTK_RESPONSE_ACCEPT)
  {

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    filename = gtk_file_chooser_get_filename(chooser);

    load_Data(filename);
    g_free(filename);
  }
  gtk_widget_destroy(dialog);
  if (res == GTK_RESPONSE_ACCEPT)
  {
    on_data_loaded();
  }
}

/*
 * @brief Foncion principale lancant la fenêtre principale
 * Effectue les traitement nécessaire pour mettre en relation les fonctions et la vue
 */
int main(int argc, char *argv[])
{
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

  //Annimations
  GtkWidget *playMi;
  GtkWidget *stopMi;

  glob.count = 0;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
  gtk_window_set_title(GTK_WINDOW(window), "Geoloc");

  vbox = gtk_box_new(FALSE, 0);

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
  deletedPointsDisplayMi = gtk_check_menu_item_new_with_label("Affichage des points supprimés");

  anonymatChoiceMi = gtk_menu_item_new_with_label("Proposition pour la suppression d'un point d'interêt");
  anonymatMi = gtk_menu_item_new_with_label("Supprimer un point d'interêt");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(displayMi), displayMenu);

  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), pointsDisplayMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), routesDisplayMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), pointInteretDisplayMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), deletedPointsDisplayMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), anonymatChoiceMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(displayMenu), anonymatMi);

  gtk_menu_shell_append(GTK_MENU_SHELL(menubar2), displayMi);

  gtk_box_pack_start(GTK_BOX(vbox), menubar2, FALSE, FALSE, 0);

  //Animations
  animationMenu = gtk_menu_new();
  animationMi = gtk_menu_item_new_with_label("Animations");
  playMi = gtk_menu_item_new_with_label("Play");
  stopMi = gtk_menu_item_new_with_label("Stop");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(animationMi), animationMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(animationMenu), playMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(animationMenu), stopMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar3), animationMi);
  gtk_box_pack_start(GTK_BOX(vbox), menubar3, FALSE, FALSE, 0);

  /////////////////////////////////////////////////
  //Zone cartograĥique
  /////////////////////////////////////////////////

  darea = gtk_drawing_area_new();
  gtk_widget_set_size_request(darea, 1200, 743);

  grid = gtk_grid_new();

  gtk_container_add(GTK_CONTAINER(window), grid);

  gtk_grid_attach(GTK_GRID(grid), vbox, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), darea, 0, 1, 1, 1);

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

  g_signal_connect(G_OBJECT(pointsDisplayMi), "toggled",
                   G_CALLBACK(pointsDisplayMiEvent), pointsDisplayMi);

  g_signal_connect(G_OBJECT(routesDisplayMi), "toggled",
                   G_CALLBACK(routesDisplayMiEvent), routesDisplayMi);

  g_signal_connect(G_OBJECT(pointInteretDisplayMi), "toggled",
                   G_CALLBACK(pointInteretDisplayMiEvent), pointInteretDisplayMi);

  g_signal_connect(G_OBJECT(deletedPointsDisplayMi), "toggled",
                   G_CALLBACK(deletedPointsDisplayMiEvent), deletedPointsDisplayMi);

  g_signal_connect(G_OBJECT(anonymatChoiceMi), "activate",
                   G_CALLBACK(anonymatChoiceMiEvent), anonymatChoiceMi);

  g_signal_connect(G_OBJECT(anonymatMi), "activate",
                   G_CALLBACK(anonymatMiEvent), anonymatMi);

  g_signal_connect(G_OBJECT(playMi), "activate",
                   G_CALLBACK(playMiEvent), playMi);

  g_signal_connect(G_OBJECT(stopMi), "activate",
                   G_CALLBACK(stopMiEvent), stopMi);

  //Affichage de la fenêtre
  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
