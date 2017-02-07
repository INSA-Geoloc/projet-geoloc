#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "data.h"
#include "parcours_list.h"

extern parcours * original_data;
/**
 * @brief Lecture du fichier Geolog
 * @param p descripteur de fichier -> ouverture d'un fichier
 * @return parcours*
 */

parcours* readData(FILE * p){
   dataPoint* read_mat;
   int i,j, date;
   double lon, lat;
   parcours * lp;

   lp = initParcours();

   while(fscanf(p, "date:%d,lat:%lf,long:%lf;\n", &date, &lat, &lon) == 3){
      read_mat = newPoint(date, lat, lon);
      addPoint(read_mat, lp);

   }
   return lp;
}

/**
 * @brief Initialise une liste vide
 * @return parcours* liste vide
 */
parcours * initParcours(){
   parcours * temp = (parcours *) malloc(sizeof(parcours));
   temp->pt = NULL;
   temp->next = NULL;
   temp->size = 0;
   return temp;
}

/**
 * @brief Ajout d'un point en queue de liste
 * @param ptd structure d'un point
 * @param p liste
 * @param liste
 */
void addPoint(dataPoint * ptd, parcours * p){

  // On augmente la taille
  p->size++;

  // Puis on ajoute
   if (p->next != NULL)
  {
      addPoint(ptd,p->next);
   }
   else
  {
      p->next = (parcours *) malloc(sizeof(parcours));
      p->next->pt = ptd;
      p->next->next = NULL;
   }
}

int removePoint(dataPoint * ptd, parcours * p){
  dataPoint *test;
  parcours *ptr_rech = (parcours *) malloc(sizeof(parcours)); // Pointeur de recherche

  parcours *ptr_trait = (parcours *) malloc(sizeof(parcours)); // Pointeur de traitement
  
  ptr_trait = p->next;

  if (ptr_trait->pt->time == ptd->time){
      printf("Debut trouve \n");
      p->next = p->next->next;
      p->size--;
      //free(ptr_trait);
      return 1;
  }
  ptr_rech = p->next->next;
  while(ptr_rech->pt != NULL){
      if(ptr_rech->pt->time == ptd->time){
          printf("%i trouve (%i) \n", ptr_rech->pt->time, ptd->time);
          ptr_trait->next = ptr_rech->next;
          printf("testT\n");
          p->size--;
          printf("TestG\n");
          printf("TestV\n");
          //free(ptr_rech);
          return 1;
      }else{
          printf("Pas trouve (%i != %i) passe au suivant\n", ptr_rech->pt->time, ptd->time);

          if(ptr_rech->next == NULL){
            printf("Non trouve \n");
            printf("testRemove9\n");
            return 0;
          }
          ptr_trait = ptr_rech;
          ptr_rech = ptr_rech->next;
      }

  }
  printf("testRemove77\n");
  free(ptr_rech);
  free(ptr_trait);
  return 0;
}

/**
 * @brief Affichage de toute la liste
 * @param p liste
 */
void displayList(parcours * pl){
  parcours * tmp = pl->next;

  while(tmp->pt != NULL){
      displayData(tmp->pt);
      if(tmp->next) tmp = tmp->next; else break;
  }

}


/**
 * @brief Destruction liste
 * @param l liste
 */
void destroyList(parcours * pl){

   if(pl->next != NULL){
      destroyList(pl->next);
   }

   if(pl->pt != NULL) {
      destroyPoint(pl->pt);
   }
   else {
      pl->size = 0;
   }
   free(pl);

}


void GPStoLambertList()
{
  parcours * tmp = original_data;
  original_data = original_data->next;

  while(original_data->pt != NULL){
      pointToPoint(original_data->pt);
      if(original_data->next) original_data = original_data->next; else break;
  }
  original_data = tmp;
}

parcours * LambertToImg()
{
  parcours * tmp = original_data;
  original_data = original_data->next;
  parcours * list_img_point;

  list_img_point = initParcours();

  //coordonnées lambert en 0,0
  double one_x = 651816.37;
  double one_y = 6666705.91;

  //Coordonnées lambert en bas à droite de la carte
  double two_x = 657157.92;
  double two_y = 6663353.00;

  double limit_x = two_x - one_x;
  double limit_y = two_y - one_y;

  dataPoint* img_point;

  while(original_data->pt != NULL){
    //longitude --> X
    //latitude --> Y

    if(original_data->pt->longitude - one_x <= limit_x  || original_data->pt->latitude <= one_y > limit_y)
    {
      double img_x = (original_data->pt->longitude - one_x) * 1200 / limit_x;
      double img_y = (original_data->pt->latitude - one_y) * 743 / limit_y;

      img_point = newPoint(original_data->pt->time, img_y, img_x);
      addPoint(img_point, list_img_point);
    }
    if(original_data->next) original_data = original_data->next; else break;
  }
    original_data = tmp;
  return list_img_point;
}
