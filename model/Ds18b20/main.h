#ifndef DS18B20_H
#define DS18B20_H

#include "../../lib/debug.h"
#include "../../lib/common.h"
#include "../../lib/1wire/main.h"
#include "../../lib/1wire/common.h"

#define DS18B20_SCRATCHPAD_LENGTH				9
#define DS18B20_ADDRESS_LENGTH					8
#define DS18B20_SCRATCHPAD_CRC_INDEX			8
#define DS18B20_ADDRESS_CRC_INDEX				7
#define DS18B20_EEPROM_BYTE_NUM					3
#define DS18B20_SCRATCHPAD_CONFIG_REG			4
#define DS18B20_WRITE_FIRST_INDEX				2
#define DS18B20_WRITE_LENGTH					3

#define DS18B20_CMD_CONVERTT					0x44
#define DS18B20_CMD_WRITE_SCRATCHPAD			0x4E
#define DS18B20_CMD_READ_SCRATCHPAD				0xBE
#define DS18B20_CMD_COPY_SCRATCHPAD				0x48
#define DS18B20_CMD_RECALL						0xB8

#define DS18B20_RESOLUTION_9BIT_REGISTER		0x1f
#define DS18B20_RESOLUTION_10BIT_REGISTER		0x3f
#define DS18B20_RESOLUTION_11BIT_REGISTER		0x5f
#define DS18B20_RESOLUTION_12BIT_REGISTER		0x7f

#define DS18B20_RESOLUTION_9BIT_TCONV_MS		94
#define DS18B20_RESOLUTION_10BIT_TCONV_MS		188
#define DS18B20_RESOLUTION_11BIT_TCONV_MS		375
#define DS18B20_RESOLUTION_12BIT_TCONV_MS		750

typedef enum{
	DS18B20_RESOLUTION_UNKNOWN=0,
	DS18B20_RESOLUTION_9BIT=9,
	DS18B20_RESOLUTION_10BIT=10,
	DS18B20_RESOLUTION_11BIT=11,
	DS18B20_RESOLUTION_12BIT=12
} ds18b20_resolution_t;

enum{
	DS18B20_ERROR,
	DS18B20_BUSY,
	DS18B20_DONE
};

typedef struct ds18b20_st Ds18b20; 
struct ds18b20_st{
	uint8_t address[DS18B20_ADDRESS_LENGTH];
	uint8_t scratchpad[DS18B20_SCRATCHPAD_LENGTH];
	uint8_t data;
	int (*control)(Ds18b20 *, Owire *);
	int (*read)(Ds18b20 *, Owire *, double *);
	int (*getResolution)(Ds18b20 *, Owire *, ds18b20_resolution_t *);
	int (*setResolution)(Ds18b20 *, Owire *, ds18b20_resolution_t);
	int (*readRom)(Ds18b20 *, Owire *);
	int (*requestTemperature)(Ds18b20 *, Owire *);
};

#define ds18b20_read(self, port, tempr) (self)->read((self), port, tempr) 
#define ds18b20_getResolution(self, port, resolution) (self)->getResolution((self), port, resolution) 
#define ds18b20_setResolution(self, port, resolution) (self)->setResolution((self), port, resolution) 
#define ds18b20_readRom(self, port) (self)->readRom((self), port)
#define ds18b20_requestTemperature(self, port) (self)->requestTemperature((self), port)

extern void ds18b20_begin(Ds18b20 *self);
extern void ds18b20_printRom(Ds18b20 *self, HardwareSerial *serial);
extern void ds18b20_printScratchpad(Ds18b20 *self, HardwareSerial *serial);
extern void ds18b20_setAddress(Ds18b20 *self, const uint8_t *address);
extern unsigned long ds18b20_getTconvByResolution(ds18b20_resolution_t resolution);
extern int ds18b20_checkResolution(int resolution);

#endif

