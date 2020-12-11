/*
 * dmx_output_task.h
 *
 * Created: 27.10.2020 20:09:15
 *  Author: Franz Forster
 */ 


#ifndef DMX_OUTPUT_TASK_H_
#define DMX_OUTPUT_TASK_H_

#include "core/types.h"

// -------------------------------------------------------------------------------------------------
// Constants
// -------------------------------------------------------------------------------------------------

#define MAXIMUM_NUM_LAMPS	32
// -------------------------------------------------------------------------------------------------

extern void			DMX_Init(void);
extern void			DMX_Output_Task (void* param);
extern tRgbwColor*	GetBackBuffer (void);
extern tRgbwColor*	GetFrontBuffer (void);


// -------------------------------------------------------------------------------------------------
#endif /* DMX_OUTPUT_TASK_H_ */