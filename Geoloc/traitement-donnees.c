//
//  traitement-donnees.c
//  Geoloc
//
//  Created by Quentin Laplanche && David Neyron on 21/10/2016.
//  Copyright © 2016 projet-geoloc. All rights reserved.
//
#include "traitement-donnees.h"

/**
* @brief NOT USED -> data.c#pointToPOint 
* Algorithme historique de conversion des données en Lambert
*/
void GPStoLambert(float latitude, float longitude)
{

  // Constante
  float N = 0.7256077650;
  float C = 11754255.426;
  float Xs = 700000.0;
  float Ys = 12655612.050;
  float E = 0.08248325676;
  float LAMBDAC = 3.0;

  float phi = to_radians(latitude);
  float LAMBDA = to_radians(longitude);

  float latitude_iso = log(tan((M_PI / 4) + (latitude * M_PI / (180 * 2))) * pow(((1 - E * sin(latitude * M_PI / 180)) / (1 + E * sin(latitude * M_PI / 180))), E / 2));
  float x = floor(Xs + C * exp(-N * latitude_iso) * sin(N * (M_PI / 180) * (longitude - LAMBDAC)));
  float y = floor(Ys - C * exp(-N * latitude_iso) * cos(N * (M_PI / 180) * (longitude - LAMBDAC)));

  // Correction des coordonnée
  x += 3;
  y += 349;
}

/**
* @brief Conversion en radians de données en degrés
*/
float to_radians(float degrees)
{
  return (degrees * M_PI) / 180.0;
}
