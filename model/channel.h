#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H

#include "../util/dstructure.h"
#include "../util/common.h"
#include "../util/ton.h"
#include "sensor.h"

/*
 * -user_config:
 * set number of channels you want to use:
 */
#define CHANNEL_COUNT 3

struct channel_st {
	int id;
	int sensor_ind;
	FTS out;
	int error_id;
	int state;
	struct channel_st *next;
};

typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel){
	
extern const char *channel_getStateStr(Channel *item);
extern void channel_setParam(Channel *item, int id, int sensor_ind);
extern void channel_begin(Channel *item);
extern int channel_start(Channel *item);
extern int channel_stop(Channel *item);
extern int channel_control(Channel *item,  DallasTemperature *sensors);
extern int channels_activeExists(ChannelLList *channels);
extern void channels_stop(ChannelLList *channels);
extern int channels_getIdFirst(ChannelLList *channels, int *out);

#endif 
