/*
 * recv_remote_task.h
 *
 * Created: 28.10.2020 09:54:58
 *  Author: Franz Forster
 */ 


#ifndef RECV_REMOTE_TASK_H_
#define RECV_REMOTE_TASK_H_

#include "core/types.h"

typedef enum
{
	CMD_OFF,
	CMD_ON,
	CMD_STATIC_COLOR,
	CMD_DYNAMIC_PROG,
	CMD_SPEED,
	CMD_TOGGLE_WHITE
} eRemoteCmdCode;

typedef struct  
{
	eRemoteCmdCode  cmdCode;
	union {
		tRgbwColor		color;
		uint8_t			dynamicProgNr;
		uint8_t			speed;
	};
} tRemoteCmd;


extern void Recv_Remote_Task(void* param);


#endif /* RECV_REMOTE_TASK_H_ */