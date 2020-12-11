/*
 * dmx_output_task.c
 *
 * Created: 27.10.2020 20:09:28
 *  Author: Franz Forster
 */ 
#include <asf.h>
#include "dmx/dmx_output_task.h"
#include "common/dbg_tools.h"
#include "core/types.h"
#include "core/taskhost.h"

// -------------------------------------------------------------------------------------------------
// Constants (local for access control)
// -------------------------------------------------------------------------------------------------

#define	FRAME_STATUS_BUFFER_BIT	(1<<0)	// Bit specifies back/frontbuffer

// -------------------------------------------------------------------------------------------------
// Variables (local for access control)
// -------------------------------------------------------------------------------------------------

static struct usart_module dmxUsartInstance;

static tRgbwColor	frameBuffer[2][MAXIMUM_NUM_LAMPS];
static uint8_t		frameStatus = 0;

// -------------------------------------------------------------------------------------------------
// Prototypes (local for access control)
// -------------------------------------------------------------------------------------------------

void	_SetDirTransmit(void);
void	_SetDirReceive(void);

void	_ResetBackBuffer (void);
void	_SwapBuffer		(void);

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
/*
	Description: 

*/
void DMX_Init(void)
{
	
	// setup dmx usart interface
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.stopbits	 = USART_STOPBITS_2;
	config_usart.baudrate    = 250000;
	config_usart.mux_setting = DMX_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = DMX_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = DMX_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = DMX_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = DMX_SERCOM_PINMUX_PAD3;
	while (usart_init(&dmxUsartInstance,
		DMX_SERCOM_MODULE, &config_usart) != STATUS_OK) {
	}
	usart_enable(&dmxUsartInstance);
	// TODO: Report success

	// set DMX DIR pin as output
	ioport_set_pin_dir(DMX_DIR_PIN, IOPORT_DIR_OUTPUT);
	_SetDirTransmit();
}

// -------------------------------------------------------------------------------------------------
/*
	Description: 

*/
void DMX_Output_Task (void* param)
{
	EventGroupHandle_t* pRenderOutputCycleEventgroup = (EventGroupHandle_t*)param;
	
	EventBits_t rocBits;
	while (1) {
		// Suspend on entry
		vTaskSuspend(NULL);
		ioport_set_pin_level(DEBUG1_PIN, HIGH);
		// -> Task resumed from Timer ISR
		rocBits = xEventGroupGetBits(*pRenderOutputCycleEventgroup);
		if (rocBits & ROC_RUNNING) {
			// RenderOutputCycle running
			if (!(rocBits & ROC_RENDERING_FINISHED_READY_FOR_OUTPUT)) {
				// Renderer not yet finished
				// TODO: report warning
				rocBits = xEventGroupWaitBits(*pRenderOutputCycleEventgroup, ROC_RENDERING_FINISHED_READY_FOR_OUTPUT, pdTRUE, pdFALSE, pdMS_TO_TICKS(100));
				if (!(rocBits & ROC_RENDERING_FINISHED_READY_FOR_OUTPUT)) {
					// Timeout
					DBG_ERR("Timeout, Renderer not finished");
					// stop ROC
					xEventGroupClearBits(*pRenderOutputCycleEventgroup, ROC_RUNNING);
				}
			}
			
			if (rocBits & ROC_RENDERING_FINISHED_READY_FOR_OUTPUT) {
				
				xEventGroupClearBits(*pRenderOutputCycleEventgroup, ROC_RENDERING_FINISHED_READY_FOR_OUTPUT);
				// ---------------------------------------------------------------------------
				// ### slightly critical! ###
				// Block all other tasks during this following process
				vTaskSuspendAll();
				_SwapBuffer();
				
				PORT->Group[0].DIR.reg |= (1<<DMX_TX_PIN);			// to drive output directly
				PORT->Group[0].PINCFG[DMX_TX_PIN].bit.PMUXEN = 0;	// unmux peripherals from TX-Pin
				ioport_set_pin_level(DMX_TX_PIN, IOPORT_PIN_LEVEL_LOW);		// drive TX-Pin low
				// hard coded DMX reset pulse
				delay_us(150);
				// undo config
				PORT->Group[0].DIR.reg &= ~(1<<DMX_TX_PIN);
				PORT->Group[0].PINCFG[DMX_TX_PIN].bit.PMUXEN = 1;
				
				delay_us(1); // technisch nutzlos, hilft aba beim decoden mit ScanaStudio
				
				// transmit data 
				// TODO: variable Frame size? 
				uint8_t startCode = 0;
				usart_write_job(&dmxUsartInstance, &startCode);
				usart_write_buffer_job(&dmxUsartInstance, (uint8_t*) GetFrontBuffer(), MAXIMUM_NUM_LAMPS*4); 

				xTaskResumeAll();
				// ---------------------------------------------------------------------------
				xEventGroupSetBits(*pRenderOutputCycleEventgroup, ROC_BUFFER_SWAPED_READY_FOR_RENDERING);
			}
			
			// && Ausgabetask wird durch Timer ISR angestoßen!
			// && in Render/output evgrp bit überprüfen ob farben fertig gerendert
			// && wenn ja: Puffer umdrehen, mit getFrontBuff Ausgabe beginnen und fertig hier
			// && Rendertask anstoßen und neue farben werden gerendert
			
		}
		
		ioport_set_pin_level(DEBUG1_PIN, LOW);
	}
 
}


tRgbwColor*	GetBackBuffer	(void)
{
	return frameBuffer[((!frameStatus) & FRAME_STATUS_BUFFER_BIT)];
}

// -------------------------------------------------------------------------------------------------
tRgbwColor*	GetFrontBuffer	(void)
{
	return frameBuffer[(frameStatus & FRAME_STATUS_BUFFER_BIT)];
}

// -------------------------------------------------------------------------------------------------
void	_SwapBuffer		(void)
{
	frameStatus ^= FRAME_STATUS_BUFFER_BIT;
}

// -------------------------------------------------------------------------------------------------
void	_ResetBackBuffer(void)
{
	uint16_t it = 0;
	uint8_t *backBuffer = (uint8_t*) GetBackBuffer;
	for (it = 0; it < MAXIMUM_NUM_LAMPS; ++it) {
		*(backBuffer++) = 0;
	}
}



// -------------------------------------------------------------------------------------------------
/*
	Description: 

*/
void	_SetDirTransmit(void)
{
	ioport_set_pin_level(DMX_DIR_PIN, IOPORT_PIN_LEVEL_HIGH);
}

void	_SetDirReceive(void)
{
	ioport_set_pin_level(DMX_DIR_PIN, IOPORT_PIN_LEVEL_LOW);	
}

