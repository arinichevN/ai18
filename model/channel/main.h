#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H

#include "../../util/dstructure.h"
#include "../../util/common.h"
#include "../../util/ton.h"
#include "../sensor.h"

struct channel_st {
	int id;
	int device_kind;
	int enable;
	size_t ind;
	
	int sensor_ind;
	FTS out;
	int error_id;
	void (*control)(struct channel_st *, DallasTemperature *); 
	struct channel_st *next;
};

typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel)
#define CHANNEL_SAVE_FIELD(F) PmemChannel pchannel;	if(pmem_getPChannel(&pchannel, item->ind)){pchannel.F = item->F; pmem_savePChannel(&pchannel, item->ind);}
#define CHANNEL_FUN_GET(param) channel_get_ ## param

#define channel_control(item, sensors) item->control(item, sensors)
#endif 
