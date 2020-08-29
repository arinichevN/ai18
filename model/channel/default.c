#include "param.h"
#include "../../util/common.h"
/*
 * -user_config:
 * put parameters for each channel in this array
 * enable: YES or NO
 * {id, enable, sensor_index}
 */
const ChannelParam CHANNEL_DEFAULT_PARAMS[] = {
	{21, YES, 0},
	{22, YES, 1},
	{23, YES, 2}
};

#define CHANNEL_COUNT (sizeof CHANNEL_DEFAULT_PARAMS / sizeof CHANNEL_DEFAULT_PARAMS[0])
