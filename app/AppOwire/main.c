#include "main.h"

static void appOwire_RUN(AppOwire *self){
	OwPoller *poller = &self->poller;
	poller->control(poller);
}

static void appOwire_OFF(AppOwire *self){
	;
}

static void appOwire_FAILURE(AppOwire *self){
	;
}

static err_t appOwire_setParam(AppOwire *self, size_t ind){
	self->ind = ind;
	AppOwireParam param;
	if(!pmem_getOwireParam(&param, ind)){
		printdln("   failed to get 1wire param from NVRAM");
		return ERROR_NVRAM_READ;
	}
	err_t r = appOwireParam_check(&param);
	if(r != ERROR_NO){
		return r;
	}
	self->enable = (yn_t) param.enable;
	owire_setParam(&self->owire, param.pin);
	owpoller_setParam(&self->poller, &self->owire, param.poll_interval_ms, DS18B20_CMD_CONVERTT);
	return ERROR_NO;
}

int appOwire_begin(AppOwire *self, size_t ind){
	self->error_id = appOwire_setParam(self, ind);
	if(self->error_id != ERROR_NO){
		self->control = appOwire_FAILURE;
		return 0;
	}
	owire_begin(&self->owire);
	owpoller_begin(&self->poller);
	self->control = appOwire_OFF;
	if(self->enable == YES){
		self->control = appOwire_RUN;
	}
	printd("app owire begin: success "); printdln(ind);
	return 1;
}

#ifdef USE_AOIDS

void appOwire_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	//AppOwire *self = (AppOwire *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(None)
	}
	acpls_reset(server);
}

void appOwire_serveAoidRequestGGS(void *vself, Aoid *oid, void *vserver, int command, void (*sendRamParam)(Aoid *, Acpls *, AppOwire *), void (*sendNvramParam)(Aoid *, Acpls *, const AppOwireParam *, yn_t), int (*setNvramParam)(Acpls *, AppOwireParam *)){
	AppOwire *self = (AppOwire *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			sendRamParam(oid, server, self);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:{
				yn_t success = NO;
				AppOwireParam param;
				memset(&param, 0, sizeof param);
				if(pmem_getOwireParam(&param, self->ind)){
					success = YES;
				}
				sendNvramParam(oid, server, &param, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				AppOwireParam param;
				if(!pmem_getOwireParam(&param, self->ind)){
					acpls_reset(server);
					return;
				}
				if(!setNvramParam(server, &param)){
					acpls_reset(server);
					return;
				}
				if(appOwireParam_check(&param) == ERROR_NO){
					pmem_saveOwireParam(&param, self->ind);
				}
			}
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GGS)
	}
	acpls_reset(server);
}

static void appOwire_sendRamParamPin(Aoid *oid, Acpls *server, AppOwire *self){int v = owire_getPin(&self->owire); aoidServer_sendII(oid, server, v, YES);}
static void appOwire_sendNvramParamPin(Aoid *oid, Acpls *server, const AppOwireParam *param, yn_t success){aoidServer_sendII(oid, server, param->pin, success);}
static int appOwire_setNvramParamPin(Acpls *server, AppOwireParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->pin = v; return 1;}
static void appOwire_serveAoidRequestPin(void *vself, Aoid *oid, void *vserver, int command){appOwire_serveAoidRequestGGS(vself, oid, vserver, command, appOwire_sendRamParamPin, appOwire_sendNvramParamPin, appOwire_setNvramParamPin);}

static void appOwire_sendRamParamInterval(Aoid *oid, Acpls *server, AppOwire *self){unsigned long v = owpoller_getPollInterval(&self->poller); aoidServer_sendUlI(oid, server, v, YES);}
static void appOwire_sendNvramParamInterval(Aoid *oid, Acpls *server, const AppOwireParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->poll_interval_ms, success);}
static int appOwire_setNvramParamInterval(Acpls *server, AppOwireParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->poll_interval_ms = v; return 1;}
static void appOwire_serveAoidRequestInterval(void *vself, Aoid *oid, void *vserver, int command){appOwire_serveAoidRequestGGS(vself, oid, vserver, command, appOwire_sendRamParamInterval, appOwire_sendNvramParamInterval, appOwire_setNvramParamInterval);}

static void appOwire_sendRamParamEnable(Aoid *oid, Acpls *server, AppOwire *self){aoidServer_sendII(oid, server, self->enable, YES);}
static void appOwire_sendNvramParamEnable(Aoid *oid, Acpls *server, const AppOwireParam *param, yn_t success){aoidServer_sendII(oid, server, param->enable, success);}
static int appOwire_setNvramParamEnable(Acpls *server, AppOwireParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->enable = (yn_t) v; return 1;}
static void appOwire_serveAoidRequestEnable(void *vself, Aoid *oid, void *vserver, int command){appOwire_serveAoidRequestGGS(vself, oid, vserver, command, appOwire_sendRamParamEnable, appOwire_sendNvramParamEnable, appOwire_setNvramParamEnable);}

void appOwire_buildAoids(AppOwire *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	//AOID_SET_PARAM(OID, 						NEXT,							PARENT,				KIND,						DESCR,									FUNCTION, 								DATA,	ID)
	AOID_SET_PARAM(&self->aoid,					&self->pin_aoid,				parent_oid,			AOID_KIND_COMPLEX,			AOID_DESCRIPTION_1WIRE,					appOwire_serveAoidRequestSelf,			self,	*id)
	AOID_SET_PARAM(&self->pin_aoid,				&self->poll_interval_aoid,		&self->aoid,		AOID_KIND_UINT8_PARAM,		AOID_DESCRIPTION_PIN,					appOwire_serveAoidRequestPin,			self,	*id)
	AOID_SET_PARAM(&self->poll_interval_aoid,	&self->enable_aoid,				&self->aoid,		AOID_KIND_TIMEMS_PARAM,		AOID_DESCRIPTION_POLL_INTERVAL_MS,		appOwire_serveAoidRequestInterval,		self,	*id)
	AOID_SET_PARAM(&self->enable_aoid,			next_oid,						&self->aoid,		AOID_KIND_YN_PARAM,			AOID_DESCRIPTION_ENABLE,				appOwire_serveAoidRequestEnable,		self,	*id)
}

#endif

