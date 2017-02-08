#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "data.h"
#include "parcours_list.h"

#define INTEREST_RATE 15

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
* Fonction utilisé uniquement dans la fonction readDb pour lire le fichier CSV
*
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
* A appeler avant de mettre les points à l'echelle
* @param list liste de points avec coord en Lambert
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
        //printf("Adresse : %s \n", adresse);
        //printf("%lf\n",lon );
        //printf("%lf\n",lat );

        if ( fabs(tmp->pt->latitude - lat)< 2 && fabs(tmp->pt->longitude - lon) < 2){
          printf("Je suis dans readDb\n" );
          tmp->pt->adresse = (char*)malloc(strlen(adresse));
          strcpy(tmp->pt->adresse,adresse); // faire malloc ?
          break;
        }
      }
      tmp = tmp->next;
    }
  }
  fclose(f);
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

/*
* Faire en sorte que on regarde un point est tant qu'il y a des points à portées on les supprimer
* Aprés on passe au points suivants.
*/
void cleanRedundantPoints() {
  parcours * listTemp = original_data->next;
  parcours * suiv;
  parcours *previous = NULL;
  while (listTemp->next != NULL) {
    suiv = listTemp->next;
    printf("%f \n",fabs(listTemp->pt->longitude - suiv->pt->longitude));
    printf("%f\n",fabs(listTemp->pt->latitude - suiv->pt->latitude));
    if( fabs(listTemp->pt->longitude - suiv->pt->longitude)< 30 && fabs(listTemp->pt->latitude - suiv->pt->latitude) < 30 ) {
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

void detectInterest(dataPoint * point) {
  parcours * listTemp = original_data->next;
  dataPoint* toBeDeleted[80];
  unsigned int count = 0;

  while (listTemp->next != NULL ) {
      printf("Coucou \n");
    if ( fabs(listTemp->pt->latitude - point->latitude ) < 30 && fabs(listTemp->pt->longitude - point->longitude ) < 30 )
    {
      if (count < 80){
        toBeDeleted[count] = listTemp->pt;
      }
      count++;
    }
    listTemp = listTemp->next;
  }
  if (count >= INTEREST_RATE)
  {
    // Appelle a la fonction remove Point de jeremy. en passant chacun des points.
    //point->adresse = "INTERET"; // Besoin d'allouer ou deja fait ?
    strcpy(point->adresse, "INTERET");

    printf("SALOPE\n %s" , point->adresse);
    int i;
    for (i = 0; i<80; i++) {
      removePoint(toBeDeleted[i],original_data);
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
      if(original_data->pt->adresse != NULL)
      {
        printf("%s\n", original_data->pt->adresse );
        img_point->adresse = (char*) malloc(60);
        strcpy(img_point->adresse, original_data->pt->adresse);
      }
      addPoint(img_point, list_img_point);
    }
    if(original_data->next) original_data = original_data->next; else break;
  }
    original_data = tmp;
  return list_img_point;
}
