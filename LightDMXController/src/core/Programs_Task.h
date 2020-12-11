/*
 * programms_task.h
 *
 * Created: 27.10.2020 23:15:38
 *  Author: Franz Forster
 */ 


#ifndef PROGRAMMS_TASK_H_
#define PROGRAMMS_TASK_H_

// -------------------------------------------------------------------------------------------------
// Constants
// -------------------------------------------------------------------------------------------------

typedef struct {
	EventGroupHandle_t*	pRenderOutputCycleEventgroup;
	QueueHandle_t*		pRemoteCommandQueue;
} tProgramsTaskParams;


extern void	Programs_Task (void* param);


// -------------------------------------------------------------------------------------------------


#endif /* PROGRAMMS_TASK_H_ */