#ifndef SENSOR_H
#define SENSOR_H

#include <DallasTemperature.h>


typedef struct {
	int ind;
	FTS output;
} Sensor;

extern void sensor_control(Sensor *item, DallasTemperature *sensors);

#endif 
