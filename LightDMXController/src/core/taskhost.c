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

typedef struct {
	uint16_t red;
	uint16_t green;
	uint16_t yellow;
	uint8_t  duration;
} tStatusLEDParam;

// -------------------------------------------------------------------------------------------------
// Variables (local for access control)
// -------------------------------------------------------------------------------------------------

TaskHandle_t StatusLedTaskHandle;
TaskHandle_t ProgramsTaskHandle;

QueueHandle_t* pStatusLEDQ;
const tStatusLEDParam IdleLEDPattern = {0, 0x3, 0, 0};


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
	*pRemoteCmdQ = xQueueCreate(8, sizeof(tRemoteCmd*));
	
	pStatusLEDQ = pvPortMalloc(sizeof(QueueHandle_t));
	*pStatusLEDQ = xQueueCreate(2, sizeof(tStatusLEDParam));
	
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
		tskIDLE_PRIORITY+3,
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
	static tStatusLEDParam ledPattern = {0, 0, 0, 0};
	static uint8_t shifter = 0;
	tStatusLEDParam* newLEDPattern;
	TickType_t lastWake;
	lastWake = xTaskGetTickCount();
	
	ledPattern = IdleLEDPattern;
	
	while (1) {
		// LED Control
		for (shifter = 0; shifter < 16; ++shifter) {
			if( xQueueReceive(*pStatusLEDQ, &newLEDPattern, (TickType_t) 10 ) == pdPASS )
			{
				ledPattern = *newLEDPattern;
				shifter = 0;
				vPortFree(newLEDPattern);
			}
			
			if (ledPattern.red & (1<<shifter)) {
				ioport_set_pin_level(LED_RED_PIN, LED_ACTIVE);
			} else {
				ioport_set_pin_level(LED_RED_PIN, LED_INACTIVE);
			}
			if (ledPattern.yellow & (1<<shifter)) {
				ioport_set_pin_level(LED_YELLOW_PIN, LED_ACTIVE);
			} else {
				ioport_set_pin_level(LED_YELLOW_PIN, LED_INACTIVE);
			}
			if (ledPattern.green & (1<<shifter)) {
				ioport_set_pin_level(LED_GREEN_PIN, LED_ACTIVE);
			} else {
				ioport_set_pin_level(LED_GREEN_PIN, LED_INACTIVE);
			}
			vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(150));
			
			if (ledPattern.duration > 0) {
				--ledPattern.duration;
			} else {
				ledPattern = IdleLEDPattern;
			}
		}
	}
}


extern void ChangeLEDStatus	(uint16_t patternRed, uint16_t patternGreen, uint16_t patternYellow, uint8_t duration)
{
	tStatusLEDParam* ledPattern = pvPortMalloc(sizeof(tStatusLEDParam));
	ledPattern->red = patternRed;
	ledPattern->green = patternGreen;
	ledPattern->yellow = patternYellow;
	ledPattern->duration = duration;
	xQueueSend(*pStatusLEDQ, &ledPattern, 0);
}


void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
	// TODO: should be disabled in final version!
	
	DBG_ERR (" # Overflow: %s\r\n", pcTaskName);
	
}