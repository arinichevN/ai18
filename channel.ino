#include "channel.h"

void channel_init(Channel *item, int id, int sensor_ind){
	item->selected = 0;
	item->id = id;
	item->sensor.ind = sensor_ind;
}

int channel_control(Channel *item){
	item->sensor.output.value = sensors.getTempCByIndex(item->sensor.ind);
	item->sensor.output.tm = millis();
	if(item->sensor.output.value < -100.0){
		item->sensor.output.state = 0;
	}else{
		item->sensor.output.state = 1;
	}
	//Serial.print(item->sensor.output.value);Serial.print(" ");Serial.print(item->reg.output);Serial.print(" ");Serial.print(item->reg.goal);Serial.println(" ");
}
