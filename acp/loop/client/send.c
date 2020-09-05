
int acplc_sendII(ACPLC *item, char sign, int v1, int v2){
	switch(acplc_getState(item)){
		case ACP_READ_RESPONSE:
		case ACP_SEND_REQUEST:
			return ACP_BUSY;
		case ACP_DONE:
			item->control = acplc_IDLE;
			return ACP_DONE;
		case ACP_IDLE:{
			acpl_reset(item->acpl);
			int r = acp_buildPackII(item->acpl->buf, ACP_BUF_MAX_LENGTH, sign, v1, v2);
			if(!r) return ACP_ERROR;
			ACPLC_START(ACPLC_MODE_SEND)
			return ACP_BUSY;}
		default:
			//acplc_reset(item);
			printd("(acplc_sendII: error)");
			return ACP_ERROR;
	}
	return ACP_NOT_FOUND;
}

int acplc_sendIF(ACPLC *item, char sign, int v1, double v2){
	switch(acplc_getState(item)){
		case ACP_READ_RESPONSE:case ACP_SEND_REQUEST:
			return ACP_BUSY;
		case ACP_DONE:
			item->control = acplc_IDLE;
			return ACP_DONE;
		case ACP_IDLE:{
			acpl_reset(item->acpl);
			int r = acp_buildPackIF(item->acpl->buf, ACP_BUF_MAX_LENGTH, sign, v1, v2);
			if(!r) return ACP_ERROR;
			ACPLC_START(ACPLC_MODE_SEND)
			return ACP_BUSY;}
		default:
			item->control = acplc_IDLE;
			printd("(acplc_sendII: error)");
			return ACP_ERROR;
	}
	return ACP_NOT_FOUND;
}

int acplc_sendIII(ACPLC *item, char sign, int v1, int v2, int v3){
	switch(acplc_getState(item)){
		case ACP_READ_RESPONSE:case ACP_SEND_REQUEST:
			return ACP_BUSY;
		case ACP_DONE:
			item->control = acplc_IDLE;
			return ACP_DONE;
		case ACP_IDLE:{
			acpl_reset(item->acpl);
			int r = acp_buildPackIII(item->acpl->buf, ACP_BUF_MAX_LENGTH, sign, v1, v2, v3);
			if(!r) return ACP_ERROR;
			ACPLC_START(ACPLC_MODE_SEND)
			return ACP_BUSY;}
		default:
			item->control = acplc_IDLE;
			printd("(acplc_sendIII: error)");
			return ACP_ERROR;
	}
	return ACP_NOT_FOUND;
}

int acplc_sendIIII(ACPLC *item, char sign, int v1, int v2, int v3, int v4){
	switch(acplc_getState(item)){
		case ACP_READ_RESPONSE:case ACP_SEND_REQUEST:
			return ACP_BUSY;
		case ACP_DONE:
			item->control = acplc_IDLE;
			return ACP_DONE;
		case ACP_IDLE:{
			acpl_reset(item->acpl);
			int r = acp_buildPackIIII(item->acpl->buf, ACP_BUF_MAX_LENGTH, sign, v1, v2, v3, v4);
			if(!r) return ACP_ERROR;
			ACPLC_START(ACPLC_MODE_SEND)
			return ACP_BUSY;}
		default:
			item->control = acplc_IDLE;
			printd("(acplc_sendIIII: error)");
			return ACP_ERROR;
	}
	return ACP_NOT_FOUND;
}

int acplc_sendIIUl(ACPLC *item, char sign, int v1, int v2, unsigned long v3){
	switch(acplc_getState(item)){
		case ACP_READ_RESPONSE:case ACP_SEND_REQUEST:
			return ACP_BUSY;
		case ACP_DONE:
			item->control = acplc_IDLE;
			return ACP_DONE;
		case ACP_IDLE:{
			//printd("(acplc_sendIIUl: start)");
			acpl_reset(item->acpl);
			int r = acp_buildPackIIUl(item->acpl->buf, ACP_BUF_MAX_LENGTH, sign, v1, v2, v3);
			printdln(item->acpl->buf);
			if(!r) return ACP_ERROR;
			ACPLC_START(ACPLC_MODE_SEND)
			return ACP_BUSY;}
		default:
			item->control = acplc_IDLE;
			printd("(acplc_sendIIUl: error)");
			return ACP_ERROR;
	}
	return ACP_NOT_FOUND;
}

int acplc_sendIIF(ACPLC *item, char sign, int v1, int v2, double v3){
	switch(acplc_getState(item)){
		case ACP_READ_RESPONSE:case ACP_SEND_REQUEST:
			return ACP_BUSY;
		case ACP_DONE:
			item->control = acplc_IDLE;
			return ACP_DONE;
		case ACP_IDLE:{
			//printd("(acplc_sendIIF: start)");
			acpl_reset(item->acpl);
			int r = acp_buildPackIIF(item->acpl->buf, ACP_BUF_MAX_LENGTH, sign, v1, v2, v3);
			printdln(item->acpl->buf);
			if(!r) return ACP_ERROR;
			ACPLC_START(ACPLC_MODE_SEND)
			return ACP_BUSY;}
		default:
			item->control = acplc_IDLE;
			printd("(acplc_sendIIF: error)");
			return ACP_ERROR;
	}
	return ACP_NOT_FOUND;
}
