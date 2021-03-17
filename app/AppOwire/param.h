#ifndef APP_OWIRE_PARAM_H
#define APP_OWIRE_PARAM_H

typedef struct {
	int pin;
	unsigned long poll_interval_ms;
	int enable;
} AppOwireParam;

extern err_t appOwireParam_check (AppOwireParam *self);

#endif
