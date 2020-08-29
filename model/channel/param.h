#ifndef MODEL_CHANNEL_PARAM_H
#define MODEL_CHANNEL_PARAM_H


typedef struct {
	int id;
	int enable;
	int sensor_ind;
} ChannelParam;

#define CHANNEL_DEFAULT_PARAMS chdefps
#endif 
