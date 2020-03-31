#ifndef APP_INCLUDE_H
#define APP_INCLUDE_H

#include "util/debug.h"
#include "util/common.c"
#include "util/crc.c"
#include "util/check.c"
#include "util/ton.c"
#include "util/serial.c"
#include "util/pmem.c"

#include "model/sensor.c"
#include "model/channel.c"

#include "acp/main.c"
#include "acp/loop/main.c"
#include "acp/loop/server.c"

#include "app/serial_config.c"
#include "app/config.c"
#include "app/serial.c"
#include "app/main.c"
#include "app/server_config.c"

#include "util/print_var.c"


#endif 