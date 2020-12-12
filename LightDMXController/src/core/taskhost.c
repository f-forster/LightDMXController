/*
 * taskhost.c
 *
 * Created: 27.10.2020 19:52:35
 *  Author: Franz Forster
 */ 
// -------------------------------------------------------------------------------------------------

#include <asf.h>
#include "core/taskhost.h"
#include "common/dbg_tools.h"
#include "dmx/dmx_output_task.h"
#include "core/Programs_Task.h"
#include "core/recv_remote_task.h"


// -------------------------------------------------------------------------------------------------
// Constants (local for access control)
// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------
// Variables (local for access control)
// -------------------------------------------------------------------------------------------------

TaskHandle_t StatusLedTaskHandle;
TaskHandle_t ProgramsTaskHandle;
TaskHandle_t RecvRemoteTaskHandle;

// -------------------------------------------------------------------------------------------------
// Eventgroups, Queues
// -------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------
// Prototypes (local for access control)
// -------------------------------------------------------------------------------------------------
static void					StatusLed_Task (void* param);


// -------------------------------------------------------------------------------------------------




void	Taskhost_Start(void)
{
	EventGroupHandle_t*	pROCEventGrp = pvPortMalloc(sizeof(EventGroupHandle_t));
	*pROCEventGrp = xEventGroupCreate();
	
	QueueHandle_t* pRemoteCmdQ = pvPortMalloc(sizeof(QueueHandle_t));
	*pRemoteCmdQ = xQueueCreate(5, sizeof(uint8_t));
	
	tProgramsTaskParams* programsTaskParams = pvPortMalloc(sizeof(tProgramsTaskParams));
	programsTaskParams->pRemoteCommandQueue =  pRemoteCmdQ;
	programsTaskParams->pRenderOutputCycleEventgroup = pROCEventGrp;
	
	
	
	xTaskCreate(StatusLed_Task,
		(const char *)"statled",
		configMINIMAL_STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY,
		&StatusLedTaskHandle);
	
	xTaskCreate(Recv_Remote_Task,
		(const char *)"remote",
		configMINIMAL_STACK_SIZE,
		pRemoteCmdQ,
		tskIDLE_PRIORITY+1,
		&RecvRemoteTaskHandle);
	
	xTaskCreate(Programs_Task,
		(const char *)"prog",
		1024,
		programsTaskParams,
		tskIDLE_PRIORITY+1,
		&ProgramsTaskHandle); 

	
	xTaskCreate(DMX_Output_Task,
		(const char *)"dmxout",
		512,
		pROCEventGrp,
		tskIDLE_PRIORITY+2,		// Higher Priority to ensure that Task runs after Timer ISR has fired
		&DMXOutputTaskHandle);
	
	
	
	
	// ---------------------------------
	// Temporary
	xEventGroupSetBits(*pROCEventGrp, ROC_RUNNING | ROC_BUFFER_SWAPED_READY_FOR_RENDERING);
}




// -------------------------------------------------------------------------------------------------


void StatusLed_Task (void* param)
{
	static uint16_t redLedDutyCycle = 0xF0F0;
	static uint16_t greenLedDutyCycle = 0;
	static uint8_t shifter = 0;
	TickType_t lastWake;
	lastWake = xTaskGetTickCount();
	
	while (1) {
		// every Second
		if (1) {
			greenLedDutyCycle = 0xFF00;
		} else {
				greenLedDutyCycle = (1<<1) | (1<<3);
		}

		
		// LED Control
		for (shifter = 0; shifter < 16; ++shifter) {
			if (redLedDutyCycle & (1<<shifter)) {
				ioport_set_pin_level(LED_RED_PIN, LED_ACTIVE);
				} else {
				ioport_set_pin_level(LED_RED_PIN, LED_INACTIVE);
			}
			if (greenLedDutyCycle & (1<<shifter)) {
				ioport_set_pin_level(LED_GREEN_PIN, LED_ACTIVE);
				} else {
				ioport_set_pin_level(LED_GREEN_PIN, LED_INACTIVE);
			}
			vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(150));
		}
	}
}


void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
	// TODO: should be disabled in final version!
	
	DBG_ERR (" # Overflow: %s\r\n", pcTaskName);
	
}