#include "main.h"

#define OWIRE_DEFAULT_PARAMS app_owire_default_params
/*
 * -user_config:
 * put default parameters for each 1-Wire bus in this array
 * OwireParam structure members:
 * pin,
 * poll_interval,
 * enable
 */
const AppOwireParam OWIRE_DEFAULT_PARAMS[] = {
	{3, 1000UL, YES},
	{5, 1000UL, YES},
	{7, 1000UL, YES}
};

#define OWIRE_COUNT (sizeof OWIRE_DEFAULT_PARAMS / sizeof OWIRE_DEFAULT_PARAMS[0])

AppOwire owire_buf[OWIRE_COUNT];

AppOwireLList owires = LLIST_INITIALIZER;

int owires_begin(int default_btn){
	LLIST_BUILD_FROM_ARRAY_N(owires, owire_buf, OWIRE_COUNT)
	size_t ind = 0;
	if(default_btn == BUTTON_DOWN){
		FOREACH_APP_OWIRE {
			if(!pmem_saveOwireParam(&OWIRE_DEFAULT_PARAMS[ind], ind)){
				return 0;
			}
			printd("default param has been saved for owire ind \n"); printdln(ind);
			ind++;
		}
	}
	ind = 0;
	FOREACH_APP_OWIRE {
		if(!appOwire_begin(app_owire, ind)){
			return 0;
		} 
		ind++;
	}
	return 1;
}

Owire *owires_getOwireByPin(int pin){
	FOREACH_APP_OWIRE {
		int p = owire_getPin(&app_owire->owire);
		if(p == pin){
			return &app_owire->owire;
		}
	}
	return NULL;
}

int owires_addSlaveByOwire(Owire *iowire, iOwpollerSlave *slave){
	FOREACH_APP_OWIRE {
		if(&app_owire->owire == iowire){
			return owpoller_addSlave(&app_owire->poller, slave);
		}
	}
	printdln("owires_addSlaveByOwire: owire not found");
	return 0;
}

int owires_deleteSlaveByOwire(Owire *iowire, iOwpollerSlave *slave){
	FOREACH_APP_OWIRE {
		if(&app_owire->owire == iowire){
			return owpoller_deleteSlave(&app_owire->poller, slave);
		}
	}
	return 0;
}

void owires_control(){
	FOREACH_APP_OWIRE {
		CONTROL(app_owire);
	}
}

#ifdef USE_AOIDS
void owires_buildAoids(Aoid *next_oid, Aoid *parent_oid, size_t *id){
	return appOwireLList_buildAoids(&owires, next_oid, parent_oid, id);
}
#endif

