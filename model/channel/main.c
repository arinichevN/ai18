#include "main.h"

Channel channel_buf[CHANNEL_COUNT];
extern const ChannelParam CHANNEL_DEFAULT_PARAMS[];

void channel_INIT(Channel *item, DallasTemperature *sensors);
void channel_RUN(Channel *item, DallasTemperature *sensors);
void channel_OFF(Channel *item, DallasTemperature *sensors);
void channel_FAILURE(Channel *item, DallasTemperature *sensors);
	
const char *channel_getStateStr(Channel *item){
	if(item->control == channel_RUN)			return "RUN";
	else if(item->control == channel_OFF)		return "OFF";
	else if(item->control == channel_FAILURE)	return "FAILURE";
	else if(item->control == channel_INIT)		return "INIT";
	return "?";
}

const char *channel_getErrorStr(Channel *item){
	return getErrorStr(item->error_id);
}

static int channelParam_check(const ChannelParam *item){
	if(!common_checkBlockStatus(item->enable)){
		printd("channelParam_check(): bad enable where id = "); printdln(item->id);
		return ERROR_PARAM;
	}
	return ERROR_NO;
}

static int channel_setParam(Channel *item, const ChannelParam *param){
	int r = channelParam_check(param);
	if(r != ERROR_NO){
		return r;
	}
	item->id = param->id;
	item->enable = param->enable;
	item->sensor_ind = param->sensor_ind;
	item->device_kind = DEVICE_KIND_DS18B20;
	printd("channel param:"); printd(" id: "); printd(item->id); printd(" sensor_ind: "); printd(item->sensor_ind); printdln(" ");
	return ERROR_NO;
}

static int channel_setDefaults(Channel *item, size_t ind){
	const ChannelParam *param = &CHANNEL_DEFAULT_PARAMS[ind];
	int r = channel_setParam(item, param);
	if(r == ERROR_NO){
		pmem_savePChannel(param, ind);
	}
	return r;
}

static int channel_setFromNVRAM(Channel *item, size_t ind){
	ChannelParam param;
	if(!pmem_getPChannel(&param, ind)){
		printdln("   failed to get channel from NVRAM");
		return ERROR_PMEM_READ;
	}
	return channel_setParam(item, &param);
}

static int channel_setParamAlt(Channel *item, size_t ind, int default_btn){
	if(default_btn == BUTTON_DOWN){
		int r = channel_setDefaults(item, ind);
		if(r != ERROR_NO){
			return r;
		}
		printd("\tdefault param\n");
	}else{
		int r = channel_setFromNVRAM(item, ind);
		if(r != ERROR_NO){
			return r;
		}
		printd("\tNVRAM param\n");
	}
	item->ind = ind;
	return ERROR_NO;
}

void channel_begin(Channel *item, size_t ind, int default_btn){
	printd("beginning channel ");printd(ind); printdln(":");
	item->error_id = channel_setParamAlt(item, ind, default_btn);
	item->control = channel_INIT;
	printd("\tid: ");printdln(item->id);
	printd("\n");
}

void channels_buildFromArray(ChannelLList *channels, Channel arr[]){
	if(CHANNEL_COUNT <= 0) return;
	channels->length = CHANNEL_COUNT;
	channels->top = &arr[0];
	channels->last = &arr[CHANNEL_COUNT - 1];
	for(size_t i = 0;i<CHANNEL_COUNT;i++){
		arr[i].next = NULL;
	}
	for(size_t i = 0; i<CHANNEL_COUNT-1; i++){
		arr[i].next = &arr[i+1];
	}
}

void channels_begin(ChannelLList *channels, int default_btn){
	extern Channel channel_buf[CHANNEL_COUNT];
	channels_buildFromArray(channels, channel_buf);
	size_t i = 0;
	FOREACH_CHANNEL(channels){
		channel_begin(channel, i, default_btn); i++;
	}
}

void channel_free(Channel *item){
	;
}

int channel_start(Channel *item){
	if(item->control == channel_OFF || item->control == channel_FAILURE){
		printd("starting channel ");printd(item->ind);printdln(":");
		item->enable = YES;
		item->control = channel_INIT;
		CHANNEL_SAVE_FIELD(enable)
		return 1;
	}
	return 0;
}

int channel_stop(Channel *item){
	printd("stopping channel ");printdln(item->ind);
	item->enable = NO;
	item->out.state = 0;
	item->error_id = ERROR_NO;
	item->control = channel_OFF;
	CHANNEL_SAVE_FIELD(enable)
	return 1;
}

int channel_disconnect(Channel *item){
	printd("disconnecting channel ");printdln(item->ind);
	item->out.state = 0;
	item->error_id = ERROR_NO;
	item->control = channel_OFF;
	return 1;
}

int channel_reset(Channel *item){
	printd("resetting channel ");printd(item->ind); printdln(":");
	channel_free(item);
	channel_begin(item, item->ind, digitalRead(DEFAULT_CONTROL_PIN));
	return 1;
}

int channels_activeExists(ChannelLList *channels){
	FOREACH_CHANNEL(channels){
		if(channel->control != channel_OFF){
			return 1;
		}
	}
	return 0;
}

int channels_getIdFirst(ChannelLList *channels, int *out){
	int success = 0;
	int f = 0;
	int v;
	FOREACH_CHANNEL(channels){
		if(!f) { v=channel->id; f=1; success=1;}
		if(channel->id < v) v = channel->id;
	}
	*out = v;
	return success;
}

void channel_INIT(Channel *item, DallasTemperature *sensors){
	if(item->error_id != ERROR_NO){
        item->control = channel_FAILURE;
        return;
    }
	item->out.state = 0;
	item->out.tm = getCurrentTs();
	item->error_id = ERROR_NO;
	item->control = channel_OFF;
	if(item->enable == YES){
		item->control = channel_RUN;
	}
}

void channel_RUN(Channel *item, DallasTemperature *sensors){
	item->out.value = sensors->getTempCByIndex(item->sensor_ind);
	item->out.tm = getCurrentTs();
	if(item->out.value < -100.0){
		item->out.state = 0;
	}else{
		item->out.state = 1;
	}
	if(item->out.state == 0){
		item->error_id = ERROR_READ;
	}else{
		item->error_id = ERROR_NO;
	}
}

void channel_OFF(Channel *item, DallasTemperature *sensors){
	;
}

void channel_FAILURE(Channel *item, DallasTemperature *sensors){
	;
}

int CHANNEL_FUN_GET(enable)(Channel *item){return item->enable;}
int CHANNEL_FUN_GET(device_kind)(Channel *item){return item->device_kind;}
