#ifndef CHANNEL_H
#define CHANNEL_H

#include "sensor.h"

#include "puart.h"

struct Channel{
	Sensor sensor;
    int selected;
    int id;
};

extern void channel_init(Channel *item, int sensor_pin);

extern int channel_control(Channel *item);

#endif 
