#include "param.h"

err_t appOwireParam_check (AppOwireParam *self){
	switch(self->enable){
		case YES:
		case NO:
			break;
		default:
			return ERROR_PARAM;
	}
	return ERROR_NO;
}
