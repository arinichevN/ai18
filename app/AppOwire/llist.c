#include "llist.h"

#ifdef USE_AOIDS

static void appOwireLList_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	//AppOwireLList *self = (AppOwireLList *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(None)
	}
	acpls_reset(server);
}

void appOwireLList_buildAoids(AppOwireLList *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	Aoid *next_oid_l = next_oid;
	if(self->top != NULL){
		next_oid_l = &self->top->aoid;
	}
	//AOID_SET_PARAM(OID, 		NEXT,			PARENT,		KIND,				DESCR,						FUNCTION, 								DATA,	ID)
	AOID_SET_PARAM(&self->aoid,	next_oid_l,		parent_oid,	AOID_KIND_LIST,		AOID_DESCRIPTION_1WIRES,	appOwireLList_serveAoidRequestSelf,		self,	*id)	
	FOREACH_LLIST(item, self, AppOwire){
		next_oid_l = next_oid;
		if(item->next != NULL){
			next_oid_l = &item->next->aoid;
		}
		appOwire_buildAoids(item, next_oid_l, &self->aoid, id);
	}
}


#endif
