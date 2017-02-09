#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "data.h"
#include "parcours_list.h"

#define INTEREST_RATE 20

extern parcours * original_data;
extern parcours * deleted_data;

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
* @brief Fonction utilisé pour essayer de parser le fichier csv.
*/
char* getfield(char * line, int num)
{
  char * tok;
  for( tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
  {
    if(!--num)
      return tok;
  }
  return NULL;
}


/*
* @brief Algorithme de lecture de la base IGN, 
* Cherche si des points de la lise correspondent à ceux de la base IGN pou remplir leur champ adresse.
*/
void readDb()
{
  parcours * tmp = original_data->next;
  FILE * f = fopen("IGN.csv","r");
  char buffer[1024], *tmpbuf;
  double lat, lon;
  char * adresse = (char*) malloc(60);

  if ( f == NULL)
  {
    fprintf(stderr, "Erreur ouverture de fichier IGN \n ");
  }
  else
  {
    while( tmp->next != NULL) {
      while(fgets(buffer, 1024, f))
      {
        //rempli la latitude
        tmpbuf = strdup(buffer);
        sscanf(getfield(tmpbuf, 1), "%lf", &lon);

        //rempli la longitude
        tmpbuf = strdup(buffer);
        sscanf(getfield(tmpbuf, 2), "%lf", &lat);

        //rempli l'adresse
        tmpbuf = strdup(buffer);
        adresse = getfield(tmpbuf, 3);
        //while(fscanf(f, "%lf;%lf;%[^;]\n", &lat, &lon, adresse) == 3) {
        printf("Adresse : %s \n", adresse);
        printf("%lf %lf\n",lon , tmp->pt->longitude);
        printf("%lf  %lf\n ",lat ,tmp->pt->latitude);

        if ( fabs(tmp->pt->latitude - lat)< 2 && fabs(tmp->pt->longitude - lon) < 2){
          //tmp->pt->adresse = (char*)malloc(strlen(adresse));
          strcpy(tmp->pt->adresse,adresse); 
          break;
        }
      }
      tmp = tmp->next;
    }
  }
  fclose(f);
}



/*
* @brief Calcul le nombre d'habitation dans la base IGN à proximité des coords données.
* @param pointLat Latitude pour la quelle chercher dans la base IGN
* @param pointLong longitude pour la quelle chercher dans la base IGN
* @return densité correspondante dans la base IGN.
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
    fclose(f);
  }
  return density;
}

/*
* @brief Fonction de nettoyage de la trame.
* parcours la liste à la recherche de point à supprimer
*/
void cleanRedundantPoints() {
  parcours * listTemp = original_data->next;
  parcours * suiv; 
  parcours *previous = NULL;
  while (listTemp->next != NULL) {
    suiv = listTemp->next;
    if( fabs(listTemp->pt->longitude - suiv->pt->longitude)< 30 && fabs(listTemp->pt->latitude - suiv->pt->latitude) < 30 
    && strcmp(listTemp->pt->adresse,"INTERET")!=0 ) {
      if(previous != NULL) { // First point
        addPoint(listTemp->pt, deleted_data);
        previous->next = suiv;
      }
    } else {
      previous = listTemp;
    }
    listTemp = suiv;
  }
}

/*
* @brief Fonction de correction des points d'interets.
* Fusionne les points d'interets s'ils se trouvent trop proche.
*/
void correctInterest() {
  parcours * tmp = original_data->next;
  parcours * tmp2 ;
  while (tmp->next != NULL){
    if(strcmp(tmp->pt->adresse,"INTERET") == 0) {
      tmp2 = tmp->next;
      while (tmp2->next != NULL) {
        if(strcmp(tmp->pt->adresse,"INTERET") == 0 && fabs(tmp->pt->latitude - tmp2->pt->latitude ) < 30 && fabs(tmp->pt->longitude - tmp2->pt->longitude ) < 30){
          removePoint(tmp2->pt,original_data);
        }
        tmp2 = tmp2->next;
      }
    }
    tmp = tmp->next;
  }
}


/*
* @brief Algorithme de detection des points d'interets
* @param point le point pour lequel chercher s'il doit être considéré comme un point d'interet
*/
void detectInterest(dataPoint * point) {
  parcours * listTemp = original_data->next;
  dataPoint* toBeDeleted[80];
  unsigned int count = 0;

  while (listTemp->next != NULL ) {

    if ( fabs(listTemp->pt->latitude - point->latitude ) < 30 && fabs(listTemp->pt->longitude - point->longitude ) < 30 )
    {
      if(listTemp->pt->latitude != point->latitude && listTemp->pt->longitude != point->longitude && strcmp(listTemp->pt->adresse, "INTERET") != 0){
        if (count < 80){
          toBeDeleted[count] = listTemp->pt;
        }
        count++;
      }
    }
    listTemp = listTemp->next;
  } 
  if (count >= INTEREST_RATE)
  {
    // Appelle a la fonction remove Point de jeremy. en passant chacun des points.
    //point->adresse = "INTERET"; // Besoin d'allouer ou deja fait ?
    strcpy(point->adresse, "INTERET");
    int i;
    for (i = 0; i<80; i++) {
      //removePoint(toBeDeleted[i],original_data);
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
* @brief fonction de suppression d'un point dans une liste.
* Recherche le point en parametre et le supprime de la liste.
* @param ptd pointeur sur le point à supprimer
* @param p pointeur sur la liste dans laquelle le point doit être supprimer
*/
int removePoint(dataPoint * ptd, parcours * p){
  dataPoint *test;
  parcours *ptr_rech = (parcours *) malloc(sizeof(parcours)); // Pointeur de recherche

  parcours *ptr_trait = (parcours *) malloc(sizeof(parcours)); // Pointeur de traitement

  ptr_trait = p->next;

  if (ptr_trait->pt->time == ptd->time){
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
          p->size--;
          //free(ptr_rech);
          return 1;
      }else{
          //printf("Pas trouve (%i != %i) passe au suivant\n", ptr_rech->pt->time, ptd->time);

          if(ptr_rech->next == NULL){
            printf("Non trouve \n");
            return 0;
          }
          ptr_trait = ptr_rech;
          ptr_rech = ptr_rech->next;
      }

  }
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

/**
* @brief Converti la liste original des données, de GPS à Lambert93
*
*/
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


/**
* @brief Crée une liste de points, avec des coordonnées mise à l'échelle de la carte.
* @return une nouvelle liste à l'échelle de la carte.
*/
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
      //strcpy(img_point->adresse,original_data->pt->adresse);
      img_point->adresse = original_data->pt->adresse;
      addPoint(img_point, list_img_point);
    }
    if(original_data->next) original_data = original_data->next; else break;
  }
    original_data = tmp;
  return list_img_point;
}

parcours * LambertToDelImg()
{
  parcours * tmp = deleted_data;
  deleted_data = deleted_data->next;
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

  while(deleted_data->pt != NULL){
    //longitude --> X
    //latitude --> Y

    if(deleted_data->pt->longitude - one_x <= limit_x  || deleted_data->pt->latitude <= one_y > limit_y)
    {
      double img_x = (deleted_data->pt->longitude - one_x) * 1200 / limit_x;
      double img_y = (deleted_data->pt->latitude - one_y) * 743 / limit_y;

      img_point = newPoint(deleted_data->pt->time, img_y, img_x);
      //strcpy(img_point->adresse, deleted_data->pt->adresse);
      addPoint(img_point, list_img_point);
    }
    if(deleted_data->next) deleted_data = deleted_data->next; else break;
  }
    deleted_data = tmp;
  return list_img_point;
}
