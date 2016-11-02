//
//  traitement-donnees.h
//  Geoloc
//
//  Created by Quentin Laplanche on 21/10/2016.
//  Copyright © 2016 projet-geoloc. All rights reserved.
//

#ifndef traitement_donnees_h
#define traitement_donnees_h

#include <stdio.h>
#include <math.h>




double to_degrees(double radians);
double to_radians(double degrees);
void GPStoLambert();
void LambertToGPS(double XLAMB, double YLAMB);

// Les constantes par David
#define N 0.72560777650
#define C 11754255.426
#define XS 700000.0
#define YS 12655612.050
#define LAMBDA0 2.2014025
#define E 0.08248325676
#define EPSILON 0.00000000001

#endif /* traitement_donnees_h */
