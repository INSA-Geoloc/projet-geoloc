#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "data.h"
#include "parcours_list.h"

#define INTEREST_RATE 15

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

/*
* A appeler avant de mettre les points à l'echelle
* @param list liste de points avec coord en Lambert 
*/
parcours* readDb( parcours * list) 
{
  parcours * tmp = list;
  FILE * f = fopen("IGN.csv","r");
  double lat, lon;
  char * adresse = (char*) malloc(60);

  if ( f == NULL) 
  {
    fprintf(stderr, "Erreur ouverture de fichier IGN \n ");
  } 
  else 
  {
    while( tmp->next != NULL) {
      while(fscanf(f, "%s,%lf,%lf\n", adresse, &lat, &lon) == 3) {
        printf("Adresse : %s \n", adresse);
        if ( fabs(list->pt->latitude - lat)< 2 && fabs(list->pt->longitude - lon) < 2){
          list->pt->adresse = (char*)malloc(strlen(adresse));
          strcpy(list->pt->adresse,adresse); // faire malloc ? 
          break;
        }
      }
      tmp = tmp->next;
    }
  }
  return list; 
}


/*
* Calcul le nombre d'habitation dans la base IGN à proximité des coords données.
* A voir si besoin de passsé la valeur du nb de metre en param.
*/
int computeDensity(double pointLat, double pointLong) {
  FILE * f = fopen("IGN.csv","r");
  double lat, lon;
  char * trash = (char*)malloc(100);
  int density = 0;
  if ( f == NULL) 
  {
    fprintf(stderr, "Erreur ouverture de fichier IGN \n ");
     // Code erreur a definir
  }
  else {
    while(fscanf(f, "%s;%lf;%lf\n", trash, &lat, &lon) == 3) {
      if ( fabs(pointLat - lat) < 15 && fabs(pointLong - lon) < 15 ) { 
        density +=1;
      }
    }
  }
  return density;
}

void cleanRedundantPoints(parcours * list) {
  dataPoint * tmp;
  parcours * listTemp = list;

  while (listTemp->next != NULL) {
    if( fabs(listTemp->pt->longitude - listTemp->next->pt->longitude)< 2 && fabs(listTemp->pt->latitude - listTemp->next->pt->latitude) < 2 ) {
      // We clean the next point.
      if (listTemp->next->next != NULL) { // There is a point after the one we want to delete
        //destroyPoint(list->next->pt);
        listTemp->next = listTemp->next->next;
      } else {
        // Next point is last point.
        listTemp->next = NULL;
      }
    }
    listTemp = listTemp->next;
  }
}

void detectInterest(parcours * list, dataPoint * point) {
  parcours * listTemp = list;
  dataPoint* toBeDeleted[50];
  unsigned int count = 0;

  do {
    if ( fabs(listTemp->pt->latitude - point->latitude ) < 15 && fabs(listTemp->pt->longitude - point->longitude ) < 15 )
    {
      if (count < 50){
        toBeDeleted[count] = listTemp->pt;
      }
      count++;
    }
    listTemp = listTemp->next;
  } while (listTemp->next != NULL);

  if (count >= INTEREST_RATE) 
  {
    // Appelle a la fonction remove Point de jeremy. en passant chacun des points.
    point->adresse = "INTERET"; // Besoin d'allouer ou deja fait ? 
    for (int i = 0; i<50; i++) {
      // removePoint(list, toBeDeleted[i]);
    }
  }
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
