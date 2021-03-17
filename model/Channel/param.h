#ifndef CHANNEL_PARAM_H
#define CHANNEL_PARAM_H

#include "../../lib/common.h"
#include "../Ds18b20/main.h"

typedef struct {
	int id;
	int pin;
	int resolution;
	uint8_t address[DS18B20_ADDRESS_LENGTH];
} ChannelParam;

extern err_t channelParam_check(const ChannelParam *self);

#endif 
