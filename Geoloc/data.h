#include <time.h>

#ifndef _DATA_H
#define _DATA_H

typedef struct
{
    /*! Adresse d'une point d'interet à implementer*/
    char *adresse;
    /*! Timestamp d'un point */
    int time;
    /*! Longitude d'un point */
    double longitude;
    /*! Latitude d'un point */
    double latitude;
} dataPoint;

#define N 0.7256077650
#define C 11754255.426
#define Xs 700000.0
#define Ys 12655612.050
#define E 0.08248325676
#define LAMBDAC 3.0

extern dataPoint *newPoint(int t, double lat, double longi);

extern void displayData(dataPoint *d);

extern double distanceBtwnPoints(dataPoint *d1, dataPoint *d2);

extern void pointToPoint(dataPoint *point);

extern void timeTxt(time_t timestamp);

extern void setPosition(dataPoint *dp, dataPoint *dref);

extern void destroyPoint(dataPoint *d);

#endif
