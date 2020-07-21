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
	id_t id;
	kind_t device_kind;
	state_t enable;
	size_t ind;
	
	int sensor_ind;
	FTS out;
	errorid_t error_id;
	state_t state;
	struct channel_st *next;
};

typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel){
#define CHANNEL_SAVE_FIELD(F) PmemChannel pchannel;	if(pmem_getPChannel(&pchannel, item->ind)){pchannel.F = item->F; pmem_savePChannel(&pchannel, item->ind);}
#define CHANNEL_FUN_GET(param) channel_get_ ## param

#endif 
