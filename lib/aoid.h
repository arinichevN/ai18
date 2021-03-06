#ifndef APP_OBJECT_ID_H
#define APP_OBJECT_ID_H

#include "acp/main.h"

#define AOID_ID_UNKNOWN 0

//each kind has a set of ACP commands, which can be applied to it
typedef enum {
	AOID_KIND_UNKNOWN=0,
	AOID_KIND_COMPLEX,
	AOID_KIND_LIST,
	AOID_KIND_YN_PARAM,
	AOID_KIND_SERIAL_ID_PARAM,
	AOID_KIND_SERIAL_RATE_PARAM,
	AOID_KIND_SERIAL_DPS_PARAM,
	AOID_KIND_SERIAL_MODE_PARAM,
	AOID_KIND_DEVICE_KIND_PARAM,
	AOID_KIND_INT8_PARAM,
	AOID_KIND_INT16_PARAM,
	AOID_KIND_INT32_PARAM,
	AOID_KIND_UINT8_PARAM,
	AOID_KIND_UINT16_PARAM,
	AOID_KIND_UINT32_PARAM,
	AOID_KIND_FLOAT_PARAM,
	AOID_KIND_STR_PARAM,
	AOID_KIND_DAY_TIME_PARAM,
	AOID_KIND_DATE_PARAM,
	AOID_KIND_TIME_PARAM,
	AOID_KIND_TIMEUS_PARAM,
	AOID_KIND_TIMEMS_PARAM,
	AOID_KIND_TIMES_PARAM,
	AOID_KIND_TODS_PARAM,
	AOID_KIND_DATED_PARAM,
	AOID_KIND_PIN_STATE_PARAM,
	AOID_KIND_PIN_MODE_PARAM,
	AOID_KIND_DS18B20_RESOLUTION_PARAM,
	AOID_KIND_COMMAND,
	AOID_KIND_DS18B20_ADDRESS_PARAM,
	AOID_KIND_NOID_COMMAND_PARAM,
	AOID_KIND_AIND_MODE_PARAM,
	AOID_KIND_AOUT_CHANNEL_DEVICE_KIND_PARAM
} AoidKind;

#define AOID_KIND_ID_PARAM AOID_KIND_UINT16_PARAM

typedef enum {
	AOID_DESCRIPTION_UNKNOWN = 0,
	AOID_DESCRIPTION_APP,
	AOID_DESCRIPTION_START,
	AOID_DESCRIPTION_STOP,
	AOID_DESCRIPTION_RESET,
	AOID_DESCRIPTION_ID,
	AOID_DESCRIPTION_DEVICE_KIND,
	AOID_DESCRIPTION_SERIALS,
	AOID_DESCRIPTION_SERIAL,
	AOID_DESCRIPTION_CHANNELS,
	AOID_DESCRIPTION_CHANNEL,
	AOID_DESCRIPTION_REMOTE_ID,
	AOID_DESCRIPTION_RATE,
	AOID_DESCRIPTION_DPS,
	AOID_DESCRIPTION_TIME,
	AOID_DESCRIPTION_MODE,
	AOID_DESCRIPTION_NOID_FIRST,
	AOID_DESCRIPTION_NOID_NEXT,
	AOID_DESCRIPTION_ENABLE,
	AOID_DESCRIPTION_TIMEOUT,
	AOID_DESCRIPTION_OUTPUT,
	AOID_DESCRIPTION_SECURE,
	AOID_DESCRIPTION_SPWM,
	AOID_DESCRIPTION_HPWM,
	AOID_DESCRIPTION_RESOLUTION,
	AOID_DESCRIPTION_PERIOD_MS,
	AOID_DESCRIPTION_DUTY_CYCLE_MIN_MS,
	AOID_DESCRIPTION_DUTY_CYCLE_MAX_MS,
	AOID_DESCRIPTION_PULSE_WIDTH_MIN_US,
	AOID_DESCRIPTION_PULSE_WIDTH_MAX_US,
	AOID_DESCRIPTION_INPUT_MIN,
	AOID_DESCRIPTION_INPUT_MAX,
	AOID_DESCRIPTION_PIN,
	AOID_DESCRIPTION_POLL_INTERVAL_MS,
	AOID_DESCRIPTION_PIN_CS,
	AOID_DESCRIPTION_PIN_SCLK,
	AOID_DESCRIPTION_PIN_MISO,
	AOID_DESCRIPTION_PIN_MOSI,
	AOID_DESCRIPTION_DAY_TIMER,
	AOID_DESCRIPTION_ON_TIME,
	AOID_DESCRIPTION_OFF_TIME,
	AOID_DESCRIPTION_2STATE_CYCLIC_TIMER,
	AOID_DESCRIPTION_INTERVAL1_S,
	AOID_DESCRIPTION_INTERVAL2_S,
	AOID_DESCRIPTION_INITIAL_PIN_STATE,
	AOID_DESCRIPTION_DATE,
	AOID_DESCRIPTION_RTC,
	AOID_DESCRIPTION_1WIRE,
	AOID_DESCRIPTION_ADDRESS,
	AOID_DESCRIPTION_ADDRESS_READ,
	AOID_DESCRIPTION_1WIRES,
	AOID_DESCRIPTION_DEVICE,
	AOID_DESCRIPTION_DEVICES,
	AOID_DESCRIPTION_PIN1,
	AOID_DESCRIPTION_PIN2,
	AOID_DESCRIPTION_PIN3,
	AOID_DESCRIPTION_DISPLAY,
	AOID_DESCRIPTION_PRECISION,
	AOID_DESCRIPTION_NOID_COMMAND,
	AOID_DESCRIPTION_SERIAL_ID,
	AOID_DESCRIPTION_TIMEUS,
	AOID_DESCRIPTION_TIMEMS,
	AOID_DESCRIPTION_TIMES
} AoidDescription;

#define AOID_ACP_REQUEST_IND_ID			ACP_REQUEST_IND_PARAM1
#define AOID_ACP_REQUEST_IND_PARAM1		ACP_REQUEST_IND_PARAM2
#define AOID_ACP_REQUEST_IND_PARAM2		ACP_REQUEST_IND_PARAM3
#define AOID_ACP_REQUEST_IND_PARAM3		ACP_REQUEST_IND_PARAM4

typedef struct aoid_st Aoid;
struct aoid_st{
	int id;
	AoidKind kind;
	AoidDescription description;
	Aoid *next;
	Aoid *parent;
	void (*serveRequest)(void *, Aoid *, void *, int);
	void *data;
};

#define AOID_INITIALIZER {.id = AOID_ID_UNKNOWN, .kind = AOID_KIND_UNKNOWN, .description = AOID_DESCRIPTION_UNKNOWN, .next = NULL, .parent = NULL, .serveRequest = NULL, .data = NULL}
#define AOID_RESET(OID) (OID)->id = AOID_ID_UNKNOWN; (OID)->next = NULL; (OID)->parent = NULL; (OID)->kind = AOID_KIND_UNKNOWN; (OID)->description = AOID_DESCRIPTION_UNKNOWN; (OID)->serveRequest = NULL; (OID)->data = NULL; 
#define AOID_SET_PARAM(OID, NEXT, PARENT, KIND, DESCR, FUNC, DATA, ID) (OID)->next = NEXT; (OID)->parent = PARENT; (OID)->kind = KIND; (OID)->description = DESCR; (OID)->serveRequest = FUNC; (OID)->data = DATA; (ID)++; (OID)->id = (ID);
#define FOREACH_AOID(TOP, ITEM) for(Aoid * ITEM=(TOP); ITEM!=NULL; ITEM=ITEM->next)

#endif 
