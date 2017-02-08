#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "data.h"
#include "parcours_list.h"
#include "traitement-donnees.h"


/**
 * @brief Création d'un nouveau point
 * @param t timestamp
 * @param longi longitude
 * @param lat latitude
 * @return structure point
 */
dataPoint* newPoint(int t,double lat, double longi){
  dataPoint * temp = (dataPoint *) malloc(sizeof(dataPoint));
  temp->time = t;
  temp->longitude = longi;
  temp->latitude = lat;
  temp->adresse = NULL;
  //temp->adresse = (char*)malloc(100*sizeof(char));
  return temp;
}

/**
 * @brief Affichage d'un point
 * @param d structure point
 */
void displayData(dataPoint * d){
   int i,j;

   if(d == NULL){
      printf("C'est nul!");
   }
   printf("----Un point de parcours\n");
   printf("Time : %d\n", d->time);
   printf("Longitude : %lf\n", d->longitude);
   printf("Latitude : %lf\n", d->latitude);
   printf("----\n");


}

void pointToPoint(dataPoint *point) {

  float latitude = point->latitude;
  float longitude = point->longitude;

  float N = 0.7256077650;
  float C = 11754255.426;
  float Xs = 700000.0;
  float Ys = 12655612.050;
  float E = 0.08248325676;
  float LAMBDAC = 3.0;

  float phi = to_radians(latitude);
  float LAMBDA = to_radians(longitude);

  float latitude_iso = log(tan((M_PI/4) + (latitude * M_PI/(180*2))) * pow(((1 - E * sin( latitude * M_PI/180)) / (1 + E * sin(latitude * M_PI/180))), E / 2));
  float x = floor(Xs + C * exp(-N * latitude_iso) * sin(N * (M_PI/180) * (longitude - LAMBDAC)));
  float y = floor(Ys - C * exp(-N * latitude_iso) * cos(N * (M_PI/180) * (longitude   - LAMBDAC)));

  x += 3;
  y += 349;

  point->latitude = y;
  point->longitude = x;
}

void timeTxt(time_t timestamp){
   time_t t = timestamp;
   const char *format = "%A %d %B %Y %H:%M:%S";

   struct tm lt;
   char res[32];

   localtime_r(&t, &lt);

   if (strftime(res, sizeof(res), format, &lt) == 0) {
        fprintf(stderr,  "strftime(3): cannot format supplied "
                                        "date/time into buffer of size %u "
                                        "using: '%s'\n",
                                        sizeof(res), format);
        exit(EXIT_FAILURE);
   }
   printf("TEST time\n");
   printf("%s\n", res);

}


/**
 * @brief Affichage de la distance entre deux points a partir de leurs coords GPS
 * @param d1 structure point
 * @param d2 structure point
 */
double distanceBtwnPoints(dataPoint * d1, dataPoint * d2){
   int R;
   double lat_a, lon_a, lat_b, lon_b, d;
   R = 6371000; //Rayon de la terre en mètre

   printf("Lat 1 : %lf long 1 : %lf\n", d1->latitude, d1->longitude);
   printf("Lat 2 : %lf long 2 : %lf\n", d2->latitude, d2->longitude);

   lat_a = to_radians(d1->latitude);
   lon_a = to_radians(d1->longitude);

   lat_b = to_radians(d2->latitude);
   lon_b = to_radians(d2->longitude);

   printf("Lat 1 : %lf long 1 : %lf\n", lat_a, lon_a);
   printf("Lat 2 : %lf long 2 : %lf\n", lat_b, lon_b);

   timeTxt(d1->time);



   d = R * (M_PI/2 - asin( sin(lat_b) * sin(lat_a) + cos(lon_b - lon_a) * cos(lat_b) * cos(lat_a) ) );
   return d;

}

/**
 * @brief Calcul de position X et Y a partir d'une position de reference
 * @param d1 structure point
 * @param d2 structure point
 */
void setPosition(dataPoint * dp, dataPoint * dref){
    printf("Lat 13 : %lf long 13 : %lf\n", dp->latitude, dp->longitude);
    printf("Lat 2 : %lf long 2 : %lf\n", dref->latitude, dref->longitude);


}


/**
 * @brief Destruction d'un point
 * @param d structure point
 */
void destroyPoint(dataPoint * d){
   free(d);
}
