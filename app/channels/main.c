#include "main.h"

/*
 * -user_config:
 * put default parameters for each channel in this array
 * ChannelParam structure members:
 * id,
 * pin,
 * resolution (9-12),
 * address (8-byte ROM code, unique for each sensor)
 */
const ChannelParam CHANNEL_DEFAULT_PARAMS[] = {
	{21, 3, 9, {40, 238, 195, 169, 33, 22, 2, 183}},
	//{22, 3, 9, {40, 238, 98, 171, 33, 22, 2, 78}},
	//{23, 3, 9, {40, 238, 202, 193, 33, 22, 2, 200}}
};

#define CHANNEL_COUNT (sizeof CHANNEL_DEFAULT_PARAMS / sizeof CHANNEL_DEFAULT_PARAMS[0])

Channel channel_buf[CHANNEL_COUNT];

ChannelLList channels = LLIST_INITIALIZER;

int channels_activeExists(){
	return channelLList_activeExists(&channels);
}

int channels_getIdFirst(int *out){
	return channelLList_getIdFirst(&channels, out);
}

void channels_begin(int default_btn){
	LLIST_BUILD_FROM_ARRAY_N(channels, channel_buf, CHANNEL_COUNT)
	size_t ind = 0;
	if(default_btn == BUTTON_DOWN){
		FOREACH_CHANNEL {
			pmem_saveChannelParam(&CHANNEL_DEFAULT_PARAMS[ind], ind);
			printd("default param has been saved for channel ind \n"); printdln(ind);
			ind++;
		}
	}
	ind = 0;
	FOREACH_CHANNEL {
		channel_begin(channel, ind); 
		ind++;
	}
}

#ifdef USE_AOIDS
void channels_buildAoids(Aoid *next_oid, Aoid *parent_oid, size_t *id){
	return channelLList_buildAoids(&channels, next_oid, parent_oid, id);
}
#endif

#ifdef USE_NOIDS
Noid *channels_buildNoids(Noid *prev){
	return channelLList_buildNoids(&channels, prev);
}
#endif
