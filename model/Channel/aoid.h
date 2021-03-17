#ifndef CHANNEL_AOID_H
#define CHANNEL_AOID_H

#include "../../lib/common.h"

typedef struct {
	Aoid main;
	Aoid id;
	Aoid pin;
	Aoid resolution;
	Aoid address;
	Aoid address_read;
} ChannelAoid;

extern void channelAoid_build(ChannelAoid *self, Aoid *next, Aoid *parent, size_t *id, void *vchannel);

#endif 
