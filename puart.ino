#include "puart.h"

#define PUART_DELIMITER_END ';'
#define PUART_DELIMITER_BLOCK '_'
#define PUART_READ_BUF_LEN 32
#define PUART_SELECT_PACK_LEN 6
#define PUART_CMD_INT_STR_LENGTH 12
#define PUART_CMD_DOUBLE_STR_LENGTH 16

#define PUART_CMD_IS(v2) (strcmp(item->cmd,v2)==0)

#define PUART_MIN_RAW_PACK_LEN 5
#define PUART_MIN_PACK_LEN 4
#define PUART_TIMEOUT_MS 10000

#define PUART_PRINT_OK Serial.print("OK");Serial.write(PUART_DELIMITER_END);
#define PUART_WAIT_DATA if(Serial.available() <= 0){unsigned long now = millis();  if(now - t1 > PUART_TIMEOUT_MS){return 0; } continue; }
#define PUART_RESET_BUF memset(item->buf, 0, sizeof (*item->buf) * PUART_BUF_LENGTH); item->bi = 0;
#define PUART_SET_CMD strncpy(item->cmd, item->buf, sizeof (*item->cmd) * PUART_BUF_LENGTH);

#define CMD_GET_FTS "cgfts"

#define PUART_CMD_SELECT "select"
#define PUART_CMD_END "end"
#define PUART_CMD_SELECTED "selected"


void puart_init(PUART *item){
	Serial.setTimeout(300);
	ton_setInterval(&item->tmr, 10000);
	ton_reset(&item->tmr);
    item->bi = 0;
    item->selected = 0;
    item->state = READ_COMMAND;
}


int puart_readInt(int *v) {
  char str[PUART_CMD_INT_STR_LENGTH];
  memset(str, 0, sizeof str);
  int i = 0;
  unsigned long t1 = millis();
  while (1) {
    PUART_WAIT_DATA
    if (i >= (PUART_CMD_INT_STR_LENGTH - 1)) {
      return 0;
    }
    int c = Serial.read();
    if (isDigit(c)) {
      str[i] = (char)c;
      i++;
    } else if (c == PUART_DELIMITER_END) {
      *v = atoi(str);
      return 1;
    }
  }
  return 0;
}

int puart_readDouble(double *v) {
  Serial.println("read double");
  char str[PUART_CMD_DOUBLE_STR_LENGTH];
  memset(str, 0, sizeof str);
  int i = 0;
  unsigned long t1 = millis();
  while (1) {
    PUART_WAIT_DATA
    if (i >= (PUART_CMD_DOUBLE_STR_LENGTH - 1)) {
      return 0;
    }
    int c = Serial.read();
    if (isDigit(c)) {
      str[i] = (char)c;
      i++;
    } else if (c == '.') {
      str[i] = (char)c;
      i++;
    } else if (c == PUART_DELIMITER_END) {
      *v = atof(str);
      return 1;
    }
  }
  return 0;
}

int puart_readStr(char *v, int len) {
  char buf[len];
  memset(buf, 0, sizeof (*buf) * len);
  int i = 0;
  unsigned long t1 = millis();
  while (1) {
    PUART_WAIT_DATA
    if (i >= len) {
      return 0;
    }
    int c = Serial.read();
    if (c == PUART_DELIMITER_END) {
      memset(v, 0, sizeof (*v) * len);
      strcpy(v, buf);
      return 1;
    } else {
      buf[i] = (char)c;
      i++;
    }
  }

  return 0;
}

int puart_readPack(PUART *item){
	if(Serial.available() > 0){
		int c = Serial.read();
        if (c == '\n'){
            return 0;
        }
		if (c == PUART_DELIMITER_END){
            item->buf[item->bi] = '\0';
			return 1;
		}else{
            if(item->bi < PUART_BUF_LENGTH){
                item->buf[item->bi] = (char)c;
                item->bi++;
            }else{
                item->bi = 0;
            }
		}
	}
	return 0;
}

int puart_parseInt(PUART *item, int *v) {
  for (int i = 0; i < item->bi; i++) {
    if (!isDigit(item->buf[i])) {
      return 0;
    }
  }
  *v = atoi(item->buf);
  return 1;
}

#define FOREACH_CHANNEL for(int i=0;i<len;i++){Channel *channel = &channels[i];
#define FOREACH_CHANNEL1 for(int i=0,Channel *channel = NULL;i<len, channel = &channels[i];i++){
#define SWDB Serial.write(PUART_DELIMITER_BLOCK)
#define SWDE Serial.write(PUART_DELIMITER_END)
#define SP(v) Serial.print(v)
#define SF Serial.flush()

void puart_server(PUART *item, Channel *channels, int len) {
  if (!puart_readPack(item)) {
    return;
  }
  //Serial.print("BUF: "); Serial.println(item->buf);
  switch (item->state) {
    case READ_COMMAND:
      PUART_SET_CMD
      ton_reset(&item->tmr);
      if (PUART_CMD_IS(PUART_CMD_SELECT) ) {//Serial.println("PUART_CMD_SELECT");
        item->state = READ_INT;
      } else if (PUART_CMD_IS(PUART_CMD_END) ) {
        item->selected = 0;
        FOREACH_CHANNEL
        channel->selected = 0;
      }
  } else if (item->selected) {
    if (PUART_CMD_IS(PUART_CMD_SELECTED)) {
    FOREACH_CHANNEL
    if (channel->selected) {
      SP(channel->id); SWDE; SF;
    }
  }

} else if (PUART_CMD_IS(CMD_GET_FTS)) { //Serial.println("CMD_GET_FTS");
  FOREACH_CHANNEL
  if (channel->selected) {
    SP(channel->id); SWDB; SP(channel->sensor.output.value); SWDB; SP(channel->sensor.output.tm); SWDB; SP(channel->sensor.output.state); SWDE; SF;
    //SP(channel->id);SWDB;SP(33.18);SWDB;SP(1);SWDB;SP(millis());SWDE;SF;
  }
}
} else {
  //Serial.println("unknown command");
}
}
PUART_RESET_BUF
break;
case READ_INT://Serial.println("READ_INT");Serial.print("CMD: ");Serial.println(item->cmd);
int v = -1;
if (ton(&item->tmr)) {
  PUART_RESET_BUF
  item->state = READ_COMMAND;//Serial.println("READ_INT timeout");
  return;
}
if (!puart_parseInt(item, &v)) { //Serial.println("READ_INT failed");
  PUART_RESET_BUF
  return;
}
//Serial.print("INT: "); Serial.println(v);
if (PUART_CMD_IS(PUART_CMD_SELECT)) {
  FOREACH_CHANNEL
  if (channel->id == v) {
    channel->selected = 1;
    item->selected = 1;
    //Serial.print("channel selected ");	Serial.println(v);
    break;
  }
}
} else {
  //Serial.println("READ_INT no command found");
}
item->state = READ_COMMAND;
PUART_RESET_BUF
break;
default:
//Serial.println("? state");
return;
}
return;
}

// select;1;cgfts;end;
