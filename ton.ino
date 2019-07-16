#include "ton.h"

void ton_setInterval(Ton *item, unsigned long interval_ms){
	item->interval = interval_ms;
}

void ton_reset(Ton *item){
	item->end = millis() + item->interval;
}

unsigned long ton_timePassed(Ton *item){
	unsigned long now = millis();
	return item->end - now;
}

int ton(Ton *item){
	if(millis() > item->end){
		return 1;
	}
	return 0;
}

