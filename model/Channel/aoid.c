#include "aoid.h"

static void channel_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_START:
			channel_start(self);
			acpls_reset(server);
			return;
		case CMD_AOID_STOP:
			channel_stop(self);
			acpls_reset(server);
			return;
		case CMD_AOID_RESET:
			channel_reset(self);
			acpls_reset(server);
			return;
		case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			aoidServer_sendSupportedSSR(oid, server);
			return;
	}
	acpls_reset(server);
}

static void channel_serveAoidRequestAddressRead(void *vself, Aoid *oid, void *vserver, int command){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_SET_NVRAM_VALUE:
			channel_readAddress(self);
			acpls_reset(server);
			return;
		case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			aoidServer_sendSupportedSet(oid, server);
			return;
	}
	acpls_reset(server);
}
//static void channel_serveRequestNone(void *vself, Aoid *oid, void *vserver, int command) {
	//Acpls *server = (Acpls *) vserver;
	//switch(command){
		//case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			//aoidServer_sendSupportedNone(oid, server);
			//return;
	//}
	//acpls_reset(server);
//}

static void channel_serveAoidRequestGGS(void *vself, Aoid *oid, void *vserver, int command, void (*sendRamParam)(Aoid *, Acpls *, Channel *), void (*sendNvramParam)(Aoid *, Acpls *, const ChannelParam *, yn_t), int (*setNvramParam)(Acpls *, ChannelParam *)){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			sendRamParam(oid, server, self);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:{
				yn_t success = NO;
				ChannelParam param;
				memset(&param, 0, sizeof param);
				if(pmem_getChannelParam(&param, self->ind)){
					success = YES;
				}
				sendNvramParam(oid, server, &param, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				ChannelParam param;
				if(!pmem_getChannelParam(&param, self->ind)){
					acpls_reset(server);
					return;
				}
				if(!setNvramParam(server, &param)){
					acpls_reset(server);
					return;
				}
				if(channelParam_check(&param) == ERROR_NO){
					pmem_saveChannelParam(&param, self->ind);
				}
			}
			acpls_reset(server);
			return;
		case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			aoidServer_sendSupportedGGS(oid, server);
			return;
	}
	acpls_reset(server);
}

static void channel_sendRamParamId(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->id, YES);}
static void channel_sendNvramParamId(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->id, success);}
static int channel_setNvramParamId(Acpls *server, ChannelParam *param){int v;	if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){	return 0;	}	param->id = v;	return 1;}
static void channel_serveAoidRequestId(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamId, channel_sendNvramParamId, channel_setNvramParamId);}

static void channel_sendRamParamPin(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->pin, YES);}
static void channel_sendNvramParamPin(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->pin, success);}
static int channel_setNvramParamPin(Acpls *server, ChannelParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->pin = v; return 1;}
static void channel_serveAoidRequestPin(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamPin, channel_sendNvramParamPin, channel_setNvramParamPin);}

static void channel_sendRamParamResolution(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, (int) channel->resolution_real, YES);}
static void channel_sendNvramParamResolution(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->resolution, success);}
static int channel_setNvramParamResolution(Acpls *server, ChannelParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->resolution = v; return 1;}
static void channel_serveAoidRequestResolution(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamResolution, channel_sendNvramParamResolution, channel_setNvramParamResolution);}

static void channel_sendRamParamAddress(Aoid *oid, Acpls *server, Channel *channel){int v[DS18B20_ADDRESS_LENGTH]; for(int i=0; i<DS18B20_ADDRESS_LENGTH; i++){v[i] = (int) channel->sensor.address[i];} aoidServer_sendAiI(oid, server, v, DS18B20_ADDRESS_LENGTH, YES);}
static void channel_sendNvramParamAddress(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){int v[DS18B20_ADDRESS_LENGTH]; for(int i=0; i<DS18B20_ADDRESS_LENGTH; i++){v[i] = (int) param->address[i];} aoidServer_sendAiI(oid, server, v, DS18B20_ADDRESS_LENGTH, success);}
static int channel_setNvramParamAddress(Acpls *server, ChannelParam *param){int v[DS18B20_ADDRESS_LENGTH]; for(int i=0; i<DS18B20_ADDRESS_LENGTH; i++){if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1 + i, &v[i])){return 0;}} for(int i=0; i<DS18B20_ADDRESS_LENGTH; i++){if(v[i] < 0 || v[i] > 255) {return 0;}} for(int i=0; i<DS18B20_ADDRESS_LENGTH; i++){param->address[i] = (uint8_t) v[i];}; return 1;}
static void channel_serveAoidRequestAddress(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamAddress, channel_sendNvramParamAddress, channel_setNvramParamAddress);}


void channelAoid_build(ChannelAoid *self, Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,									DESCR,									FUNCTION, 								DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->id,					parent,			AOID_KIND_COMPLEX,						AOID_DESCRIPTION_CHANNEL,				channel_serveAoidRequestSelf,			vchannel,	*id)
	AOID_SET_PARAM(&self->id,					&self->pin,					&self->main,	AOID_KIND_ID_PARAM,						AOID_DESCRIPTION_ID,					channel_serveAoidRequestId,				vchannel,	*id)
	AOID_SET_PARAM(&self->pin,					&self->resolution,			&self->main,	AOID_KIND_UINT8_PARAM,					AOID_DESCRIPTION_PIN,					channel_serveAoidRequestPin,			vchannel,	*id)
	AOID_SET_PARAM(&self->resolution,			&self->address,				&self->main,	AOID_KIND_DS18B20_RESOLUTION_PARAM,		AOID_DESCRIPTION_RESOLUTION,			channel_serveAoidRequestResolution,		vchannel,	*id)
	AOID_SET_PARAM(&self->address,				&self->address_read,		&self->main,	AOID_KIND_DS18B20_ADDRESS_PARAM,		AOID_DESCRIPTION_ADDRESS,				channel_serveAoidRequestAddress,		vchannel,	*id)
	AOID_SET_PARAM(&self->address_read,			next,						&self->main,	AOID_KIND_COMMAND,						AOID_DESCRIPTION_ADDRESS_READ,			channel_serveAoidRequestAddressRead,	vchannel,	*id)
}
