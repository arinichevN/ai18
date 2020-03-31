#include "channel.h"

Channel channel_buf[CHANNEL_COUNT];

const char *channel_getStateStr(Channel *item){
	switch(item->state){
		case RUN: return "RUN";
		case OFF: return "OFF";
		case INIT: return "INIT";
	}
	return "?";
}

const char *channel_getErrorStr(Channel *item){
	return getErrorStr(item->error_id);
}

int channel_getEnable (Channel *item){return 1;}

void channel_setParam(Channel *item, int id, int sensor_ind){
	item->id = id;
	item->sensor_ind = sensor_ind;
}
void channel_init(Channel *item){
	item->state = INIT;
}

void channel_begin(Channel *item){
	item->state = INIT;
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

#define SET_CHANNEL_PARAM(ID, SENSOR_IND) if(chn != NULL){channel_setParam(chn, ID, SENSOR_IND);chn = chn->next;} else {printdln("call SET_CHANNEL_PIN for each channel"); return 0;}
int channels_begin(ChannelLList *channels, int default_btn){
	extern Channel channel_buf[CHANNEL_COUNT];
	channels_buildFromArray(channels, channel_buf);
	Channel *chn = channels->top;
	/*
	 * -user_config:
	 * call
	 * SET_CHANNEL_PIN(id, sensor_ind)
	 * for each channel:
	 */
	SET_CHANNEL_PARAM(11, 2);
	SET_CHANNEL_PARAM(12, 1);
	SET_CHANNEL_PARAM(13, 0);
	
	if(chn != NULL){
		printd("number of calles of SET_CHANNEL_PARAM() should be equal to CHANNEL_COUNT");
		return 0;
	}
	size_t i = 0;
	FOREACH_CHANNEL(channels)
		channel_begin(channel); i++;
		if(channel->error_id != ERROR_NO) return 0;
	}
	return 1;
}

int channel_start(Channel *item){
	printd("(starting channel ");printdln(item->id);printd(") ");
	channel_begin(item);
	return 1;
}

int channel_stop(Channel *item){
	printd("(stopping channel ");printdln(item->id);printd(") ");
	item->state = OFF;
	return 1;
}

int channel_reload(Channel *item){
	item->state = INIT;
	return 1;
}

int channels_activeExists(ChannelLList *channels){
	FOREACH_CHANNEL(channels)
		if(channel->state != OFF){
			return 1;
		}
	}
	return 0;
}

void channels_stop(ChannelLList *channels){
	FOREACH_CHANNEL(channels)
		channel_stop(channel);
	}
}

int channels_getIdFirst(ChannelLList *channels, int *out){
	int success = 0;
	int f = 0;
	int v;
	FOREACH_CHANNEL(channels)
		if(!f) { v=channel->id; f=1; success=1;}
		if(channel->id < v) v = channel->id;
	}
	*out = v;
	return success;
}

int channel_control(Channel *item, DallasTemperature *sensors){
	switch(item->state){
		case RUN:
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
			break;
		case OFF:
			break;
		case INIT:
			item->out.state = 0;
			item->out.tm = getCurrentTs();
			item->error_id = ERROR_NO;
			item->state = RUN;
			break;
		default:
			break;
	}
	return item->state;
}

