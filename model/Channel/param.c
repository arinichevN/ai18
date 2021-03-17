#include "param.h"

err_t channelParam_check(const ChannelParam *self){
	if(!ds18b20_checkResolution(self->resolution)){
		printdln("bad resolution");
		return ERROR_PARAM;
	}
	return ERROR_NO;
}
