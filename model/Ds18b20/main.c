#include "main.h"

static double ds18b20_getTemperatureCelsiusFromLocalScratchpad(Ds18b20 *self) {
	uint16_t v = (self->scratchpad[1] << 8) | self->scratchpad[0];
	double t;
	if (v >= 0x800) {//temperture is negative
		t = 0;
		//calculate the fractional part
		if (v & 0x0001) t += 0.0625;
		if (v & 0x0002) t += 0.1250;
		if (v & 0x0004) t += 0.2500;
		if (v & 0x0008) t += 0.5000;
	
		//calculate the whole number part
		v = (v >> 4) & 0x00FF;
		v = v - 0x0001; //subtract 1
		v = ~v; //ones compliment
		t = t - (double) (v & 0xFF);
	} else {//temperture is positive
		t = 0;
		//calculate the whole number part
		t = (v >> 4) & 0x00FF;
		//t = (v >> 4);
		//calculate the fractional part
		if (v & 0x0001) t = t + 0.06250;
		if (v & 0x0002) t = t + 0.12500;
		if (v & 0x0004) t = t + 0.25000;
		if (v & 0x0008) t = t + 0.50000;
	}
    return t;
}

static ds18b20_resolution_t ds18b20_configRegToResolution(uint8_t reg){
	switch(reg){
		case DS18B20_RESOLUTION_9BIT_REGISTER: return DS18B20_RESOLUTION_9BIT;
		case DS18B20_RESOLUTION_10BIT_REGISTER: return DS18B20_RESOLUTION_10BIT;
		case DS18B20_RESOLUTION_11BIT_REGISTER: return DS18B20_RESOLUTION_11BIT;
		case DS18B20_RESOLUTION_12BIT_REGISTER: return DS18B20_RESOLUTION_12BIT;
		default: break;
	}
	return DS18B20_RESOLUTION_UNKNOWN;
}

static uint8_t ds18b20_resolutionToConfigReg(ds18b20_resolution_t resolution){
	switch(resolution){
		case DS18B20_RESOLUTION_9BIT: return DS18B20_RESOLUTION_9BIT_REGISTER;
		case DS18B20_RESOLUTION_10BIT: return DS18B20_RESOLUTION_10BIT_REGISTER;
		case DS18B20_RESOLUTION_11BIT: return DS18B20_RESOLUTION_11BIT_REGISTER;
		case DS18B20_RESOLUTION_12BIT: return DS18B20_RESOLUTION_12BIT_REGISTER;
		default: break;
	}
	return DS18B20_RESOLUTION_9BIT_REGISTER;
}

unsigned long ds18b20_getTconvByResolution(ds18b20_resolution_t resolution){
	switch(resolution){
		case DS18B20_RESOLUTION_9BIT: return DS18B20_RESOLUTION_9BIT_TCONV_MS;
		case DS18B20_RESOLUTION_10BIT: return DS18B20_RESOLUTION_10BIT_TCONV_MS;
		case DS18B20_RESOLUTION_11BIT: return DS18B20_RESOLUTION_11BIT_TCONV_MS;
		case DS18B20_RESOLUTION_12BIT: return DS18B20_RESOLUTION_12BIT_TCONV_MS;
		default: break;
	}
	return DS18B20_RESOLUTION_12BIT_TCONV_MS;
}

int ds18b20_checkResolution(int resolution){
	switch(resolution){
		case DS18B20_RESOLUTION_9BIT:
		case DS18B20_RESOLUTION_10BIT:
		case DS18B20_RESOLUTION_11BIT:
		case DS18B20_RESOLUTION_12BIT:
			return 1;
	}
	return 0;
}

static int ds18b20_IDLE_FUNCTION(Ds18b20 *self, Owire *owire){
	return DS18B20_ERROR;
}

//========================CONTROL: SEND COMMAND======================================================
static int ds18b20_SEND_COMMAND1(Ds18b20 *self, Owire *owire);
static int ds18b20_SEND_COMMAND2(Ds18b20 *self, Owire *owire);
static int ds18b20_SEND_COMMAND3(Ds18b20 *self, Owire *owire);
static int ds18b20_SEND_COMMAND4(Ds18b20 *self, Owire *owire);

//reseting port
static int ds18b20_SEND_COMMAND1(Ds18b20 *self, Owire *owire){
	int r = owire->reset(owire);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			self->control = ds18b20_SEND_COMMAND2;
			return DS18B20_BUSY;
	}
	return DS18B20_ERROR;
}

//sending read rom command
static int ds18b20_SEND_COMMAND2(Ds18b20 *self, Owire *owire){
	uint8_t v = OW_CMD_MATCH_ROM;
	int r = owire->write(owire, &v, 1);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			self->control = ds18b20_SEND_COMMAND3;
			return DS18B20_BUSY;
	}
	self->control = ds18b20_IDLE_FUNCTION;
	return DS18B20_ERROR;
}

//sending device address
static int ds18b20_SEND_COMMAND3(Ds18b20 *self, Owire *owire){
	int r = owire->write(owire, self->address, DS18B20_ADDRESS_LENGTH);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			self->control = ds18b20_SEND_COMMAND4;
			return DS18B20_BUSY;
	}
	self->control = ds18b20_IDLE_FUNCTION;
	return DS18B20_ERROR;
}

//sending command
static int ds18b20_SEND_COMMAND4(Ds18b20 *self, Owire *owire){
	int r = owire->write(owire, &self->data, 1);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			self->control = ds18b20_IDLE_FUNCTION;
			return DS18B20_DONE;
	}
	self->control = ds18b20_IDLE_FUNCTION;
	return DS18B20_ERROR;
}

//========================CONTROL: READ SCRATCHPAD======================================================
static int ds18b20_READ_SCRATCHPAD1(Ds18b20 *self, Owire *owire);
static int ds18b20_READ_SCRATCHPAD2(Ds18b20 *self, Owire *owire);

//reading device response
static int ds18b20_READ_SCRATCHPAD1(Ds18b20 *self, Owire *owire){
	int r = owire->read(owire, self->scratchpad, DS18B20_SCRATCHPAD_LENGTH);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			self->control = ds18b20_READ_SCRATCHPAD2;
			return DS18B20_BUSY;
	}
	self->control = ds18b20_IDLE_FUNCTION;
	return DS18B20_ERROR;
}

//crc check
static int ds18b20_READ_SCRATCHPAD2(Ds18b20 *self, Owire *owire){
	self->control = ds18b20_IDLE_FUNCTION;
	uint8_t crc = 0;
	for(int i=0; i<DS18B20_SCRATCHPAD_LENGTH; i++){
		if(i == DS18B20_SCRATCHPAD_CRC_INDEX) continue;
		crc_update(&crc, self->scratchpad[i]);
	}
	uint8_t expected_crc = self->scratchpad[DS18B20_SCRATCHPAD_CRC_INDEX];
	if(crc != expected_crc){
		printd("ds18b20: bad scratchpad CRC: "); printd(crc); printd(", expected: "); printdln(expected_crc);
		return DS18B20_ERROR;
	}
	return DS18B20_DONE;
}

//========================READ ROM======================================================
static int ds18b20_READ_ROM1(Ds18b20 *self, Owire *owire);
static int ds18b20_READ_ROM2(Ds18b20 *self, Owire *owire);
static int ds18b20_READ_ROM3(Ds18b20 *self, Owire *owire);
static int ds18b20_READ_ROM4(Ds18b20 *self, Owire *owire);

//reseting port
static int ds18b20_READ_ROM1(Ds18b20 *self, Owire *owire){
	int r = owire->reset(owire);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			//printdln("18: reset done");
			self->readRom = ds18b20_READ_ROM2;
			return DS18B20_BUSY;
	}
	return DS18B20_ERROR;
}

//sending read rom command
static int ds18b20_READ_ROM2(Ds18b20 *self, Owire *owire){
	uint8_t v = OW_CMD_READ_ROM;
	int r = owire->write(owire, &v, 1);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			//printdln("18: write done");
			self->readRom = ds18b20_READ_ROM3;
			return DS18B20_BUSY;
	}
	self->readRom = ds18b20_READ_ROM1;
	return DS18B20_ERROR;
}

//reading device response
static int ds18b20_READ_ROM3(Ds18b20 *self, Owire *owire){
	int r = owire->read(owire, self->address, DS18B20_ADDRESS_LENGTH);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			//printdln("18: read done");
			self->readRom = ds18b20_READ_ROM4;
			return DS18B20_BUSY;
	}
	self->readRom = ds18b20_READ_ROM1;
	return DS18B20_ERROR;
}

//crc check
static int ds18b20_READ_ROM4(Ds18b20 *self, Owire *owire){
	self->readRom = ds18b20_READ_ROM1;
	uint8_t crc = 0;
	for(int i=0; i<DS18B20_ADDRESS_LENGTH; i++){
		if(i == DS18B20_ADDRESS_CRC_INDEX) continue;
		crc_update(&crc, self->address[i]);
	}
	uint8_t expected_crc = self->address[DS18B20_ADDRESS_CRC_INDEX];
	if(crc != expected_crc){
		printd("ds18b20: bad address CRC: "); printd(crc); printd(", expected: "); printdln(expected_crc);
		return DS18B20_ERROR;
	}
	//printdln("18: check done");
	return DS18B20_DONE;
}

//========================SET RESOLUTION======================================================
static int ds18b20_SET_RESOLUTION1(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v);
static int ds18b20_SET_RESOLUTION2(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v);
static int ds18b20_SET_RESOLUTION3(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v);
static int ds18b20_SET_RESOLUTION4(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v);
static int ds18b20_SET_RESOLUTION5(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v);
static int ds18b20_SET_RESOLUTION6(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v);
static int ds18b20_SET_RESOLUTION7(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v);

static int ds18b20_SET_RESOLUTION1(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v){
	self->data = DS18B20_CMD_RECALL;
	self->control = ds18b20_SEND_COMMAND1;
	self->setResolution = ds18b20_SET_RESOLUTION2;
	return DS18B20_BUSY;
}

//sending recall command
static int ds18b20_SET_RESOLUTION2(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->data = DS18B20_CMD_READ_SCRATCHPAD;
			self->control = ds18b20_SEND_COMMAND1;
			self->setResolution = ds18b20_SET_RESOLUTION3;
			return DS18B20_BUSY;
	}
	self->setResolution = ds18b20_SET_RESOLUTION1;
	return DS18B20_ERROR;
}

//sending read scratchpad command
static int ds18b20_SET_RESOLUTION3(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->control = ds18b20_READ_SCRATCHPAD1;
			self->setResolution = ds18b20_SET_RESOLUTION4;
			return DS18B20_BUSY;
	}
	self->setResolution = ds18b20_SET_RESOLUTION1;
	return DS18B20_ERROR;
}

//reading scratchpad, then alter scratchpad
static int ds18b20_SET_RESOLUTION4(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->scratchpad[DS18B20_SCRATCHPAD_CONFIG_REG] = ds18b20_resolutionToConfigReg(v);
			//printd("set resolution: "); printd(v); printd("->"); printdln(self->scratchpad[DS18B20_SCRATCHPAD_CONFIG_REG]);
			self->data = DS18B20_CMD_WRITE_SCRATCHPAD;
			self->control = ds18b20_SEND_COMMAND1;
			self->setResolution = ds18b20_SET_RESOLUTION5;
			return DS18B20_BUSY;
	}
	self->setResolution = ds18b20_SET_RESOLUTION1;
	return DS18B20_ERROR;
}

//sending write scratchpad command
static int ds18b20_SET_RESOLUTION5(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->setResolution = ds18b20_SET_RESOLUTION6;
			return DS18B20_BUSY;
	}
	self->setResolution = ds18b20_SET_RESOLUTION1;
	return DS18B20_ERROR;
}

//writing altered scratchpad
static int ds18b20_SET_RESOLUTION6(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v){
	int r = owire->write(owire, &self->scratchpad[DS18B20_WRITE_FIRST_INDEX], DS18B20_WRITE_LENGTH);
	switch(r){
		case OWIRE_BUSY:
			return DS18B20_BUSY;
		case OWIRE_DONE:
			self->data = DS18B20_CMD_COPY_SCRATCHPAD;
			self->control = ds18b20_SEND_COMMAND1;
			self->setResolution = ds18b20_SET_RESOLUTION7;
			return DS18B20_BUSY;
	}
	self->setResolution = ds18b20_SET_RESOLUTION1;
	return DS18B20_ERROR;
}

//sending copy scratchpad command
static int ds18b20_SET_RESOLUTION7(Ds18b20 *self, Owire *owire, ds18b20_resolution_t v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->setResolution = ds18b20_SET_RESOLUTION1;
			return DS18B20_DONE;
	}
	self->setResolution = ds18b20_SET_RESOLUTION1;
	return DS18B20_ERROR;
}

//========================GET RESOLUTION======================================================
static int ds18b20_GET_RESOLUTION1(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v);
static int ds18b20_GET_RESOLUTION2(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v);
static int ds18b20_GET_RESOLUTION3(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v);
static int ds18b20_GET_RESOLUTION4(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v);

static int ds18b20_GET_RESOLUTION1(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v){
	self->data = DS18B20_CMD_RECALL;
	self->control = ds18b20_SEND_COMMAND1;
	self->getResolution = ds18b20_GET_RESOLUTION2;
	return DS18B20_BUSY;
}

//sending recall command
static int ds18b20_GET_RESOLUTION2(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->data = DS18B20_CMD_READ_SCRATCHPAD;
			self->control = ds18b20_SEND_COMMAND1;
			self->getResolution = ds18b20_GET_RESOLUTION3;
			return DS18B20_BUSY;
	}
	self->getResolution = ds18b20_GET_RESOLUTION1;
	return DS18B20_ERROR;
}

//sending read scratchpad command
static int ds18b20_GET_RESOLUTION3(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->control = ds18b20_READ_SCRATCHPAD1;
			self->getResolution = ds18b20_GET_RESOLUTION4;
			return DS18B20_BUSY;
	}
	self->getResolution = ds18b20_GET_RESOLUTION1;
	return DS18B20_ERROR;
}

//reading scratchpad, then return result
static int ds18b20_GET_RESOLUTION4(Ds18b20 *self, Owire *owire, ds18b20_resolution_t *v){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			//printd("get resolution: "); printd(self->scratchpad[DS18B20_SCRATCHPAD_CONFIG_REG]);printd("->"); printdln(ds18b20_configRegToResolution(self->scratchpad[DS18B20_SCRATCHPAD_CONFIG_REG]));
			*v = ds18b20_configRegToResolution(self->scratchpad[DS18B20_SCRATCHPAD_CONFIG_REG]);
			self->getResolution = ds18b20_GET_RESOLUTION1;
			return DS18B20_DONE;
	}
	self->getResolution = ds18b20_GET_RESOLUTION1;
	return DS18B20_ERROR;
}

//========================READ======================================================
static int ds18b20_READ1(Ds18b20 *self, Owire *owire, double *tempr_cels);
static int ds18b20_READ2(Ds18b20 *self, Owire *owire, double *tempr_cels);
static int ds18b20_READ3(Ds18b20 *self, Owire *owire, double *tempr_cels);

static int ds18b20_READ1(Ds18b20 *self, Owire *owire, double *tempr_cels){
	self->data = DS18B20_CMD_READ_SCRATCHPAD;
	self->control = ds18b20_SEND_COMMAND1;
	self->read = ds18b20_READ2;
	return DS18B20_BUSY;
}


//sending read scratchpad command
static int ds18b20_READ2(Ds18b20 *self, Owire *owire, double *tempr_cels){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->control = ds18b20_READ_SCRATCHPAD1;
			self->read = ds18b20_READ3;
			return DS18B20_BUSY;
	}
	self->read = ds18b20_READ1;
	return DS18B20_ERROR;
}

//reading scratchpad, then return result
static int ds18b20_READ3(Ds18b20 *self, Owire *owire, double *tempr_cels){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
		    *tempr_cels = ds18b20_getTemperatureCelsiusFromLocalScratchpad(self);
			self->read = ds18b20_READ1;
			return DS18B20_DONE;
	}
	self->read = ds18b20_READ1;
	return DS18B20_ERROR;
}

//==========================REQUEST TEMPERATURE===============================================
static int ds18b20_REQUEST_TEMPERATURE1(Ds18b20 *self, Owire *owire);
static int ds18b20_REQUEST_TEMPERATURE2(Ds18b20 *self, Owire *owire);

static int ds18b20_REQUEST_TEMPERATURE1(Ds18b20 *self, Owire *owire){
	self->data = DS18B20_CMD_CONVERTT;
	self->control = ds18b20_SEND_COMMAND1;
	self->requestTemperature = ds18b20_REQUEST_TEMPERATURE2;
	return DS18B20_BUSY;
}

//sending read scratchpad command
static int ds18b20_REQUEST_TEMPERATURE2(Ds18b20 *self, Owire *owire){
	int r = self->control(self, owire);
	switch(r){
		case DS18B20_BUSY:
			return DS18B20_BUSY;
		case DS18B20_DONE:
			self->requestTemperature = ds18b20_REQUEST_TEMPERATURE1;
			return DS18B20_DONE;
	}
	self->requestTemperature = ds18b20_REQUEST_TEMPERATURE1;
	return DS18B20_ERROR;
}

void ds18b20_begin(Ds18b20 *self){
	self->control = ds18b20_IDLE_FUNCTION;
	self->read = ds18b20_READ1;
	self->getResolution = ds18b20_GET_RESOLUTION1;
	self->setResolution = ds18b20_SET_RESOLUTION1;
	self->readRom = ds18b20_READ_ROM1;
	self->requestTemperature = ds18b20_REQUEST_TEMPERATURE1;
}

static void ds18b20_printArr(uint8_t *arr, size_t length, HardwareSerial *serial){
	serial->print("{");
	size_t dlength = length - 1;
	for(size_t i = 0; i < length; i++){
		serial->print(arr[i]);
		if(i < dlength){
			serial->print(", ");
		}
	}
	serial->println("}");
}

void ds18b20_printRom(Ds18b20 *self, HardwareSerial *serial){
	 ds18b20_printArr(self->address, DS18B20_ADDRESS_LENGTH, serial);
}

void ds18b20_printScratchpad(Ds18b20 *self, HardwareSerial *serial){
	ds18b20_printArr(self->scratchpad, DS18B20_SCRATCHPAD_LENGTH, serial);
}

void ds18b20_setAddress(Ds18b20 *self, const uint8_t *address){
	for(size_t i = 0; i < DS18B20_ADDRESS_LENGTH; i++){
		self->address[i] = address[i];
	}
}
