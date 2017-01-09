#include <time.h>

#ifndef   _DATA_H
#define   _DATA_H

typedef struct{
	/*! Adresse d'une point d'interet à implementer*/
	char * adresse;
	/*! Timestamp d'un point */
	int time;
	/*! Longitude d'un point */
    	double longitude;
    	/*! Latitude d'un point */
    	double latitude;
}dataPoint;

extern dataPoint* newPoint(int t,double lat, double longi);

extern void displayData(dataPoint * d);

extern double distanceBtwnPoints(dataPoint * d1, dataPoint * d2);

extern void timeTxt(time_t timestamp);

extern void setPosition(dataPoint * dp, dataPoint * dref);

extern void destroyPoint(dataPoint * d);


#endif
