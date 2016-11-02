//
//  traitement-donnees.c
//  Geoloc
//
//  Created by Quentin Laplanche && David Neyron on 21/10/2016.
//  Copyright © 2016 projet-geoloc. All rights reserved.
//
#include "traitement-donnees.h"

/**
 * Fonction brouillon
 */
void GPStoLambert(){
    //a modifier
    double latitude   = 49.036705880377674;
    double longitude = 2.046720988527103;


    //variables:
    float a        = 6378137; //demi grand axe de l'ellipsoide (m)
    float e        = 0.08181919106; //première excentricité de l'ellipsoide
    float lc       = to_radians(3.f);
    float l0    = to_radians(3.f);
    float phi0    = to_radians(46.5f); //latitude d'origine en radian
    float phi1    = to_radians(44.f); //1er parallele automécoïque
    float phi2    = (49.f); //2eme parallele automécoïque

    float x0    = 700000; //coordonnées à l'origine
    float y0    = 6600000; //coordonnées à l'origine

    float phi    = to_radians(latitude);
    float l        = to_radians(longitude);

    //calcul des grandes normales
    float gN1    = a/sqrt(    1 - e * e * sin(phi1) * sin( phi1 ) );
    float gN2    = a/sqrt(    1 - e * e * sin(phi2) * sin( phi2 ) );

    const double pi = 3.14159265358979323846;
    //calculs des latitudes isométriques
    float gl1    = log( tan( pi / 4 + phi1 / 2) * pow( (1 - e * sin( phi1 ) ) / ( 1 + e * sin( phi1 ) ), e / 2) );
    float gl2    = log( tan( pi / 4 + phi2 / 2) * pow( (1 - e * sin( phi2 ) ) / ( 1 + e * sin( phi2 ) ), e / 2) );
    float gl0    = log( tan( pi / 4 + phi0 / 2) * pow( (1 - e * sin( phi0 ) ) / ( 1 + e * sin( phi0 ) ), e / 2) );
    float gl    = log( tan( pi / 4 + phi  / 2) * pow( (1 - e * sin( phi  ) ) / ( 1 + e * sin( phi  ) ), e / 2) );

    //calcul de l'exposant de la projection
    float n        = ( log( ( gN2 * cos( phi2 ) ) / ( gN1 * cos( phi1 )))) / ( gl1 - gl2);//ok

    //calcul de la constante de projection
    float c        = (( gN1 * cos( phi1 )) / n) * exp( n * gl1);//ok

    //calcul des coordonnées
    float ys    = y0 + c * exp( -1 * n * gl0);

    float x93    = x0 + c * exp( -1 * n * gl) * sin( n * ( l - lc));
    float y93    = ys - c * exp( -1 * n * gl) * cos( n * ( l - lc));

    printf("%f %f ", x93, y93);
}

/**
 * Transforme des coordonnees utlisant la norme Lambert 93 en Degres numeriques.
 * A finir
 * @param XLAMB
 * @param YLAMB
 */
void LambertToGPS(double XLAMB, double YLAMB){
    double R, gamma, longitude, latIso, phiAvant, latitude, phiI;

    R = sqrt( pow((XLAMB - XS), 2) + pow((YLAMB - YS), 2)  );
    gamma = atan( (XLAMB - XS) / (YS - YLAMB) );

    longitude =  LAMBDA0 + (gamma / N) ;
    latIso = 1/N * log10(fabs(R/C));

    printf("%2f \n", longitude);

    phiAvant = 2 * atan(exp(latIso)) - (M_PI/2);

    printf("%2f\n",phiAvant);

    do
    {
      phiI = 2* atan( ( pow((1 + E * sin(phiAvant))/(1 - E * sin(phiAvant)) , E/2 )) * exp(latIso) ) - (M_PI/2) ;
      printf(" phiI %2f\n",phiI);
      if (fabs(phiI-phiAvant)<EPSILON)
      {
          latitude = phiI;
          printf(" latitude :%2f \n", latitude);
          return;
      }
      else
      {
          phiAvant=phiI;
      }
    } while(fabs(phiI - phiAvant) < EPSILON);
}

/**
 * Transforme des radians en Degres
 * @param  radians
 * @return
 */
double to_degrees(double radians){
    return radians * (180.0 / M_PI);
}

/**
 * Transforme des degres en radians
 * @param  degrees
 * @return
 */
double to_radians(double degrees){
    return (degrees * M_PI) / 180.0;
}
