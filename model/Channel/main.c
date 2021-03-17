#include "main.h"

static int channel_poll(void *vself, Owire *owire){
	Channel *self = (Channel *) vself;
	int r = ds18b20_read(&self->sensor, owire, &self->output.value);
	switch(r){
		case DS18B20_BUSY:
			return STATE_BUSY;
		case DS18B20_DONE:
			self->output.success = YES;
			if(self->error_id == ERROR_READ){
				self->error_id = ERROR_NO;
			}
			printd(self->id); printd(" "); printd(self->output.value); printdln(" ");
			return STATE_DONE;
	}
	self->output.success = NO;
	self->error_id = ERROR_READ;
	return STATE_FAILURE;
}

static void channel_setTimestamp(void *vself, struct timespec tm){
	Channel *self = (Channel *) vself;
	self->output.tm = tm;
}

static void channel_pollFailed(void *vself){
	Channel *self = (Channel *) vself;
	self->output.success = NO;
}

static void channel_IDLE(Channel *self);
static void channel_SET_RESOLUTION1(Channel *self);
static void channel_SET_RESOLUTION2(Channel *self);
static void channel_GET_RESOLUTION1(Channel *self);
static void channel_GET_RESOLUTION2(Channel *self);
static void channel_READ_ADDRESS1(Channel *self);
static void channel_READ_ADDRESS2(Channel *self);
static void channel_OFF(Channel *self);
static void channel_INIT(Channel *self);
static void channel_FAILURE(Channel *self);

void channel_readAddress(Channel *self){
	if(self->owire != NULL){
		self->control = channel_READ_ADDRESS1;
	}
}

state_t channel_getState(Channel *self){
	if(self->control == channel_OFF) {
		return STATE_OFF;
	} else if(self->control == channel_IDLE || self->control == channel_SET_RESOLUTION1 || self->control == channel_SET_RESOLUTION2 || self->control == channel_GET_RESOLUTION1 || self->control == channel_GET_RESOLUTION2 || self->control == channel_READ_ADDRESS1 || self->control == channel_READ_ADDRESS2) {
		return STATE_RUN;
	} else if(self->control == channel_INIT) {
		return STATE_INIT;
	} else if(self->control == channel_FAILURE) {
		return STATE_FAILURE;
	}
	return STATE_UNKNOWN;
}

err_t channel_getError(Channel *self){
	return self->error_id;
}

static void channel_buildIOwpollerSlave(Channel *self, iOwpollerSlave *ifs){
	ifs->self = self;
	ifs->next = NULL;
	ifs->poll = channel_poll;
	ifs->setTimestamp = channel_setTimestamp;
	ifs->pollFailed = channel_pollFailed;
	ifs->poller = NULL;
	ifs->pollerOnWaitIntervalChanged = NULL;
	ifs->wait_interval_ms = DS18B20_RESOLUTION_12BIT_TCONV_MS;
}

static void channel_buildInterfaces(Channel *self){
	channel_buildIOwpollerSlave(self, &self->im_iwpp_slave);
}

static err_t channel_setParam(Channel *self, size_t ind){
	self->ind = ind;
	ChannelParam param;
	if(!pmem_getChannelParam(&param, ind)){
		printdln("   failed to get channel from NVRAM");
		return ERROR_NVRAM_READ;
	}
	err_t r = channelParam_check(&param);
	if(r != ERROR_NO){
		return r;
	}
	self->id = param.id;
	self->pin = param.pin;
	self->resolution_nvram = (ds18b20_resolution_t) param.resolution;
	self->device_kind = DEVICE_KIND_DS18B20;
	printd("channel param: id "); printd(self->id); printd(", pin "); printd(self->pin); printd(", resolution "); printdln(self->resolution_nvram);
	memcpy(self->sensor.address, param.address, sizeof self->sensor.address);
	self->owire = owires_getOwireByPin(self->pin);
	if(self->owire == NULL){
		printd("1wire not found for channel ind "); printdln(self->ind);
		return ERROR_PARAM;
	}
	return ERROR_NO;
}

void channel_begin(Channel *self, size_t ind){
	printd("beginning channel ");printd(ind); printdln(":");
	channel_buildInterfaces(self);
	self->error_id = channel_setParam(self, ind);
	self->control = channel_INIT;
	printd("\tid: ");printdln(self->id);
	printd("\n");
}


void channel_free(Channel *self){
	;
}

int channel_start(Channel *self){
	if(self->control == channel_OFF){
		printd("starting channel ");printd(self->ind);printdln(":");
		self->output.value = 0.0;
		self->output.tm = getCurrentTs();
		self->output.success = NO;
		mutexcl_reset(&self->ow_rmutex_cl);
		ds18b20_begin(&self->sensor);
		self->control = channel_SET_RESOLUTION1;
		return 1;
	}
	return 0;
}

int channel_stop(Channel *self){
	if(self->control == channel_FAILURE ){
		return 0;
	}
	printd("stopping channel ");printdln(self->ind);
	if(self->control == channel_GET_RESOLUTION2 || self->control == channel_SET_RESOLUTION2 || self->control == channel_READ_ADDRESS2){//owire is beeing used by this channel
		owire_restart(self->owire);
		owire_unlock(self->owire, &self->ow_rmutex_cl);
	}
	owires_deleteSlaveByOwire(self->owire, &self->im_iwpp_slave);
	self->output.success = NO;
	self->error_id = ERROR_NO;
	self->control = channel_OFF;
	return 1;
}

int channel_disconnect(Channel *self){
	return channel_stop(self);
}

int channel_reset(Channel *self){
	printd("restarting channel ");printd(self->ind); printdln(":");
	channel_stop(self);
	channel_free(self);
	channel_begin(self, self->ind);
	return 1;
}

static void channel_IDLE(Channel *self){
	;
}

static void channel_OFF(Channel *self){
	;
}

static void channel_FAILURE(Channel *self){
	;
}

static void channel_SET_RESOLUTION1(Channel *self){
	if(owire_lock(self->owire, &self->ow_rmutex_cl)){
		self->control = channel_SET_RESOLUTION2;
	}
}
static void channel_SET_RESOLUTION2(Channel *self){
	int r = ds18b20_setResolution(&self->sensor, self->owire, self->resolution_nvram);
	switch(r){
		case DS18B20_BUSY:
			return;
		case DS18B20_DONE:
			//printd("set resolution: "); printdln(self->resolution_nvram);
			owire_unlock(self->owire, &self->ow_rmutex_cl);
			self->control = channel_GET_RESOLUTION1;
			return;
	}
	printd("set resolution: "); printdln("failed");
	owire_unlock(self->owire, &self->ow_rmutex_cl);
	self->error_id = ERROR_READ;
	self->control = channel_FAILURE;
}

static void channel_GET_RESOLUTION1(Channel *self){
	if(owire_lock(self->owire, &self->ow_rmutex_cl)){
		self->control = channel_GET_RESOLUTION2;
	}
}

static void channel_GET_RESOLUTION2(Channel *self){
	int r = ds18b20_getResolution(&self->sensor, self->owire, &self->resolution_real);
	switch(r){
		case DS18B20_BUSY:
			return;
		case DS18B20_DONE:
			//printd("get resolution: "); printdln(self->resolution_real);
			owire_unlock(self->owire, &self->ow_rmutex_cl);
			self->im_iwpp_slave.wait_interval_ms = ds18b20_getTconvByResolution(self->resolution_real);
			if(self->resolution_nvram == self->resolution_real){
				printd("slave next: "); printdln((int)self->im_iwpp_slave.next);
				int success = owires_addSlaveByOwire(self->owire, &self->im_iwpp_slave);
				if(!success){
					printd("failed to add channel to poller for channel ind "); printdln(self->ind);
					self->error_id = ERROR_PARAM;
					self->control = channel_FAILURE;
					return;
				}
				if(self->im_iwpp_slave.pollerOnWaitIntervalChanged != NULL){
					self->im_iwpp_slave.pollerOnWaitIntervalChanged(self->im_iwpp_slave.poller, &self->im_iwpp_slave);
				}
				self->control = channel_IDLE;
			}else{
				printd("failure: resolution: "); printd(self->resolution_real); printd(" expected:"); printdln(self->resolution_nvram);
				self->control = channel_FAILURE;
			}
			return;
	}
	printd("get resolution: "); printdln("failed");
	owire_unlock(self->owire, &self->ow_rmutex_cl);
	self->error_id = ERROR_READ;
	self->control = channel_FAILURE;
}

static int channel_saveAddress(Channel *self){
	ChannelParam param;
	if(!pmem_getChannelParam(&param, self->ind)){
		return 0;
	}
	memcpy(param.address, self->sensor.address, sizeof param.address);
	if(channelParam_check(&param) == ERROR_NO){
		return pmem_saveChannelParam(&param, self->ind);
	}
	return 0;
}

static void channel_READ_ADDRESS1(Channel *self){
	if(owire_lock(self->owire, &self->ow_rmutex_cl)){
		self->control = channel_READ_ADDRESS2;
	}
}

static void channel_READ_ADDRESS2(Channel *self){
	int r = ds18b20_readRom(&self->sensor, self->owire);
	switch(r){
		case DS18B20_BUSY:
			return;
		case DS18B20_DONE:
			printdln("channel read address: done");
			owire_unlock(self->owire, &self->ow_rmutex_cl);
			channel_saveAddress(self);
			self->control = channel_IDLE;
			return;
	}
	printd("channel read address: "); printdln("failed");
	owire_unlock(self->owire, &self->ow_rmutex_cl);
	self->error_id = ERROR_READ;
	self->control = channel_IDLE;
}

static void channel_INIT(Channel *self){
	if(self->error_id != ERROR_NO){
		self->control = channel_FAILURE;
		return;
	}
	self->output.value = 0.0;
	self->output.tm = getCurrentTs();
	self->output.success = NO;
	mutexcl_reset(&self->ow_rmutex_cl);
	ds18b20_begin(&self->sensor);
	printd("channel init: done "); printdln(self->ind);
	self->control = channel_SET_RESOLUTION1;
}

#ifdef USE_AOIDS
#include "aoid.c"

void channel_buildAoids(Channel *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	channelAoid_build(&self->aoid, next_oid, parent_oid, self, id);
}

#endif

#ifdef USE_NOIDS
#include "noid.c"
#endif

