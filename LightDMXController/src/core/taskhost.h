/*
 * taskhost.h
 *
 * Created: 27.10.2020 19:52:55
 *  Author: Franz Forster
 */ 


#ifndef TASKHOST_H_
#define TASKHOST_H_

// -------------------------------------------------------------------------------------------------
// Constants
// -------------------------------------------------------------------------------------------------
// Bits for RenderOutputCycle:
#define ROC_RUNNING									(1<<0)
#define ROC_RENDERING_FINISHED_READY_FOR_OUTPUT		(1<<1)
#define ROC_BUFFER_SWAPED_READY_FOR_RENDERING		(1<<2)

#define BUTTON1_TIME_TO_RESET						6000 // apprx. 6s to reset

// -------------------------------------------------------------------------------------------------
// Task Handles
// -------------------------------------------------------------------------------------------------

TaskHandle_t DMXOutputTaskHandle;	// used in time_controller (wake from isr)
TaskHandle_t RecvRemoteTaskHandle;	// used in recv_remote_task (wake from remote ISR)






// -------------------------------------------------------------------------------------------------
extern void	Taskhost_Start			(void);




// Einsprungspunkt für Overflow-catch (durch FreeRTOS vorgegeben)
extern void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName );



#endif /* TASKHOST_H_ */