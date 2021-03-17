#ifndef APP_OWIRE_LLIST_H
#define APP_OWIRE_LLIST_H

#ifdef USE_AOIDS
#include "../../lib/aoid.h"
#endif
#include "main.h"

typedef struct {
	AppOwire *top; 
	AppOwire *last; 
	size_t length;
#ifdef USE_AOIDS
	Aoid aoid;
#endif
} AppOwireLList;

#ifdef USE_AOIDS
extern void appOwireList_buildAoids(AppOwireLList *self, Aoid *next_oid,  Aoid *parent_oid, size_t *id);
#endif

#endif
