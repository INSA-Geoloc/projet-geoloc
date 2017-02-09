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

float to_radians(float degrees);
void GPStoLambert(float lattitude, float longitude);

#endif /* traitement_donnees_h */
