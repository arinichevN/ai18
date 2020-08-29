#include "main.h"

extern ChannelLList channels;
extern AppSerial serials[];

void app_OFF(App *item);
void app_FAILURE(App *item);
void app_RESET(App *item);
void app_DISABLE(App *item);
void app_DSTEP1(App *item);
void app_DSTEP2(App *item);
void app_RUN(App *item);
void app_INIT(App *item);

void app_begin(App *item);

void app_OFF(App *item){
	;
}

void app_FAILURE(App *item){
	;
}

void app_RESET(App *item){
	channels_stop(&channels);
	item->next_control = app_INIT;
	item->control = app_DSTEP1;
}

void app_DISABLE(App *item){
	channels_stop(&channels);
	item->next_control = app_OFF;
	item->control = app_DSTEP1;
}

void app_DSTEP1(App *item){
	FOREACH_CHANNEL(&channels){
		channel_control(channel, &item->sensors);
	}
	appSerials_control(serials);
	if(!channels_activeExists(&channels)){
		item->control = app_DSTEP2;
	}
}

void app_DSTEP2(App *item){
	FOREACH_SERIAL(i){
		AppSerial *serial = &serials[i];
		appSerial_free(serial);
	}
	item->control = item->next_control;
}

void app_RUN(App *item){
	if (tonr(&item->tmr)) {
		//digitalWrite(INDICATOR_PIN, HIGH);
		DallasTemperature *sensors = &item->sensors;
		sensors->requestTemperatures();
		unsigned long t_end = sensors->millisToWaitForConversion(SENSOR_RESOLUTION) + millis();
		while(millis() < t_end){
			appSerials_control(serials);
		}
		item->error_id = ERROR_NO;
		FOREACH_CHANNEL(&channels){
			channel_control(channel, &item->sensors);
			if(channel->error_id != ERROR_NO){
				item->error_id = ERROR_SUBBLOCK;
				printdln("channel error");
			}
			appSerials_control(serials);
			appei_control(&item->error_indicator, item->error_id);
		}
		//digitalWrite(INDICATOR_PIN, LOW);
	}
	appSerials_control(serials);
	appei_control(&item->error_indicator, item->error_id);
}

void app_INIT(App *item){
	app_begin(item);
}

//time for attempt to upload sketch in case of error
void app_uploadDelay(){
	pinMode(INDICATOR_PIN, OUTPUT);
	digitalWrite(INDICATOR_PIN, HIGH);
	delay(5000);
	digitalWrite(INDICATOR_PIN, LOW);
}

const char *app_getErrorStr(App *item){
	return getErrorStr(item->error_id);
} 

const char *app_getStateStr(App *item){
	if(item->control == app_RUN)			return "RUN";
	else if(item->control == app_FAILURE)	return "FAILURE";
	else if(item->control == app_OFF)		return "OFF";
	else if(item->control == app_RESET)		return "RESET";
	else if(item->control == app_DISABLE)	return "DISABLE";
	else if(item->control == app_DSTEP1)	return "DSTEP1";
	else if(item->control == app_DSTEP2)	return "DSTEP2";
	return "?";
}

int appc_checkId(int v){
	return 1;
}

int appc_checkSerialRate(int v){
	unsigned long r = serial_getRate(v);
	if(r > 0){
		return 1;
	}
	return 0;
}

int appc_checkSerialConfig(int v){
	int r = serial_checkConfig(v);
    if(r){
		return 1;
	}
	return 0;
}


void app_begin(App *item){
	item->error_id = ERROR_NO;
	item->next_control = app_FAILURE;
	app_uploadDelay();
	appei_begin(&item->error_indicator, INDICATOR_PIN);
	pinMode(DEFAULT_CONTROL_PIN, INPUT_PULLUP);
	int default_btn = digitalRead(DEFAULT_CONTROL_PIN);
	Serial.begin(9600, SERIAL_8N1);while(!Serial){;}DEBUG_SERIAL_DEVICE = &Serial;
	appSerials_init(serials);
	AppConfig config;
	item->error_id = appConfig_begin(&config, default_btn);
	if(item->error_id != ERROR_NO){goto err;}
	item->id = config.id;
	FOREACH_SERIAL(i){
		item->error_id = appSerial_beginKind(&serials[i], &config.serial[i], &DEBUG_SERIAL_DEVICE);
		if(item->error_id != ERROR_NO) goto err;
	}
	item->one_wire.begin(SENSOR_PIN);
	item->sensors = DallasTemperature(&item->one_wire);
	item->sensors.begin();
	item->sensors.setResolution(SENSOR_RESOLUTION);
	item->sensors.setWaitForConversion(0);
	ton_setInterval(&item->tmr, READ_PERIOD_MS);
	ton_reset(&item->tmr);
	channels_begin(&channels, default_btn);
	
	printdln("app: RUN");
	item->control = app_RUN;
	return;
	
	err:
	printdln("app: FAILURE");
	appei_control(&item->error_indicator, item->error_id);
	item->control = app_FAILURE;
}

void app_stop(App *item){
	item->control = app_DISABLE;
}

void app_reset(App *item){
	item->control = app_RESET;
}

void app_init(App *item){
	item->control = app_INIT;
}






