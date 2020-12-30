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

#define PRG_PROP_STATIC_COLOR		(1<<0)
#define PRG_PROP_DYNAMIC			(1<<1)
#define PRG_PROP_SIMULTAN			(1<<2)
#define PRG_PROP_OFF				(1<<3)


typedef struct {
	tRgbwColor*	colorList;
	uint8_t		speed;				// from 5 to 
	uint8_t		brightness;
	uint8_t		numColors;
	uint8_t		propertys;
	uint8_t		offsets;
} tProgram;

typedef struct {
	EventGroupHandle_t*	pRenderOutputCycleEventgroup;
	QueueHandle_t*		pRemoteCommandQueue;
} tProgramsTaskParams;


extern void	Programs_Task (void* param);


// -------------------------------------------------------------------------------------------------


#endif /* PROGRAMMS_TASK_H_ */