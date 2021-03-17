#ifndef APP_OWIRE_H
#define APP_OWIRE_H

#include "../../lib/1wire/poller/main.h"
#include "../../lib/1wire/main.h"
#include "../../model/Ds18b20/main.h"

#ifdef USE_AOIDS
#include "../../lib/aoid.h"
#endif
#include "param.h"


typedef struct app_owire_st AppOwire;
struct app_owire_st {
	Owire owire;
	OwPoller poller;
	yn_t enable;
#ifdef USE_AOIDS
	Aoid aoid;
	Aoid pin_aoid;
	Aoid poll_interval_aoid;
	Aoid enable_aoid;
#endif
	void (*control)(AppOwire *);
	err_t error_id;
	size_t ind;
	AppOwire *next;
};


extern int appOwire_begin(AppOwire *self, size_t ind);

#ifdef USE_AOIDS
extern void appOwire_buildAoids(AppOwire *self, Aoid *next_oid, Aoid *parent_oid, size_t *id);
#endif

#endif
