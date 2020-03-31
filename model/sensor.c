#include "sensor.h"

void sensor_control(Sensor *item, DallasTemperature *sensors){
	item->output.value = sensors->getTempCByIndex(item->ind);
	item->output.tm = getCurrentTs();
	if(item->output.value < -100.0){
		item->output.state = 0;
	}else{
		item->output.state = 1;
	}
}
