#ifndef PUART_H
#define PUART_H

#include "channel.h"

//crc is hardware job
//multiple slaves can listen - that is why we can send one command for several slaves
//but only one slave can ask - that is why we read from slave one after another

#define PUART_BUF_LENGTH 16

struct PUART {
  char buf[PUART_BUF_LENGTH];
  char cmd[PUART_BUF_LENGTH];
  int bi;
  Ton tmr;
  int state;
  int selected;
};

//extern void puart_server(PUART *item, Channel *channels, int len);
	
#endif 
