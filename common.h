#ifndef COMMON_H
#define COMMON_H

enum StateE {
	INIT,
	OFF,
	FAILURE,
  DISABLE,
	
	DONE,
	RUN,
	REG_PID,
	REG_ONF,
	OUT_MAX,
	OUT_MIN,
	WAIT,
	WAIT_OUT,
	WAIT_HIGH,
	WAIT_LOW,
	WAIT_GAP,
	BLOCK,

  HOLD,
  REACH,
  NSTEP,
	
	
	WAIT_IN,
	WAIT_DELAY,
	
	READ_COMMAND,
	READ_INT,
	READ_DOUBLE
};

typedef enum {
	BLOCK_DISABLE=0,
	BLOCK_ENABLE=1
} CmdState;

struct FTS {
	double value;
	unsigned long tm;
	int state;
};

#endif 
