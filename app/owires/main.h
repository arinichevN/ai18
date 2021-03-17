#ifndef APP_OWIRES_H
#define APP_OWIRES_H

#include "../../lib/dstructure.h"
#include "../../lib/serial.h"
#include "../../lib/common.h"
#include "../../lib/acp/command/main.h"
#ifdef USE_AOIDS
#include "../../lib/aoid.h"
#endif
#include "../AppOwire/main.h"
#include "../AppOwire/llist.h"

#define FOREACH_APP_OWIRE FOREACH_LLIST_N(app_owire, owires, AppOwire)

extern int owires_begin(int default_btn);

extern Owire *owires_getOwireByPin(int pin);

extern int owires_addSlaveByOwire(Owire *iowire, iOwpollerSlave *slave);

extern int owires_deleteSlaveByOwire(Owire *iowire, iOwpollerSlave *slave);

extern void owires_control();

#ifdef USE_AOIDS
extern void owires_buildAoids(Aoid *next_oid, Aoid *parent_oid, size_t *id);
#endif

#endif 
