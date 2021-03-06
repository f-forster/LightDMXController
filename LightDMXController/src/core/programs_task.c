/*
 * programms_task.c
 *
 * Created: 27.10.2020 23:14:52
 *  Author: Franz Forster
 */ 
#include <asf.h>
#include "common/dbg_tools.h"
#include "dmx/dmx_output_task.h"
#include "core/recv_remote_task.h"
#include "core/types.h"
#include "core/programs_task.h"
#include "core/programm_presets.h"
#include "core/taskhost.h"

// -------------------------------------------------------------------------------------------------
// Constants (local for access control)
// -------------------------------------------------------------------------------------------------


#define NUM_LAMPS					2


#define PRG_MAX_TRANSITION_STEPS	4096UL


typedef struct {
	uint16_t	Pos;
	uint8_t		nextColorIndex;
} tTransitionVar;

// -------------------------------------------------------------------------------------------------
// Variables (local for access control)
// -------------------------------------------------------------------------------------------------
tRgbwColor tmpFrame[MAXIMUM_NUM_LAMPS];


// -------------------------------------------------------------------------------------------------
// Prototypes (local for access control)
// -------------------------------------------------------------------------------------------------


 

// -------------------------------------------------------------------------------------------------
void	Programs_Task (void* param) 
{
	tProgramsTaskParams* prgTaskParam = (tProgramsTaskParams*)param;
	tRemoteCmd *newRemoteCmd;
	EventBits_t rocBits = ROC_RUNNING;
	
	tProgram currentPrg;
	tRgbwColor	_tmpColor;
	
	tTransitionVar renderTransition[NUM_LAMPS];
	
	
	// Laufindex
	uint8_t itLamp = 0;
	uint8_t itColor = 0;
	uint8_t itOffset = 0;
	
	
	

	
	// Init
	currentPrg = prg1;
	for (itLamp = 0; itLamp < NUM_LAMPS; ++itLamp) {
		renderTransition[itLamp].nextColorIndex = 0;
		renderTransition[itLamp].Pos = 0;
	}
	
	// renderTransition[].Pos initialisieren:
	for (itOffset = 0; itOffset < currentPrg.offsets; ++itOffset) {
		renderTransition[itOffset].Pos = itOffset * (PRG_MAX_TRANSITION_STEPS/currentPrg.offsets);
	}
	
	
	while (1) {
		// Evaluate Remote Control
		if( xQueueReceive( *(prgTaskParam->pRemoteCommandQueue), &newRemoteCmd, (TickType_t) 10 ) == pdPASS )
		{
			if (newRemoteCmd->cmdCode == CMD_OFF) {
				currentPrg.propertys |= PRG_PROP_OFF;
			}
			else if (newRemoteCmd->cmdCode == CMD_ON) {
				currentPrg.propertys &= ~PRG_PROP_OFF;
			}
			else if (newRemoteCmd->cmdCode == CMD_SPEED) {
				currentPrg.speed = newRemoteCmd->speed;
			}
			else if (newRemoteCmd->cmdCode == CMD_STATIC_COLOR) {
				currentPrg = constColorProgram;
				currentPrg.colorList[0]._dword = newRemoteCmd->color._dword;
			} 
			else if (newRemoteCmd->cmdCode == CMD_DYNAMIC_PROG) {
				currentPrg = *(programList[newRemoteCmd->dynamicProgNr % PRESET_PROG_COUNT]); // Limit to count in programList
			}
			
			vPortFree(newRemoteCmd);
		}
		
		
		
		// Suspend until ready for new frame
		rocBits = xEventGroupWaitBits(*(prgTaskParam->pRenderOutputCycleEventgroup), ROC_BUFFER_SWAPED_READY_FOR_RENDERING, pdTRUE, pdFALSE, pdMS_TO_TICKS(1000));
		if (!(rocBits & ROC_RUNNING)) {
			// ROC not running, just continue anyway
			DBG_INFO("ROC not running");
			continue;
		} else if (!(rocBits & ROC_BUFFER_SWAPED_READY_FOR_RENDERING)) {
			// Timeout
			DBG_ERR("Timeout, Data-Output not finished");
			continue;
		}
		// -> Task resumed
		// Render Data: ---------------------------------
		/*	Brief:
			Render Task is called every ~33ms, this gives the time basis for the 
			calculation of the individual colors
			
		*/
		if (currentPrg.propertys & PRG_PROP_OFF) {
			// Every Lamp off
			for (itLamp = 0; itLamp < NUM_LAMPS; ++itLamp) {
				GetBackBuffer()[itLamp]._dword = 0;
			}
		}
		else if (currentPrg.propertys & PRG_PROP_STATIC_COLOR) {
			// Every Lamp the same static color, no dimming
			for (itLamp = 0; itLamp < NUM_LAMPS; ++itLamp) {
				GetBackBuffer()[itLamp]._dword = currentPrg.colorList[0]._dword;
			}
			
		} 
		else if (currentPrg.propertys & PRG_PROP_DYNAMIC) {
			
			if (currentPrg.propertys & PRG_PROP_SIMULTAN) {
				// Every Lamp the same color
				// Using only transitionPos[0]
				if (renderTransition[0].Pos == 0) {
					// Save current showed color
					tmpFrame[0]._dword = GetFrontBuffer()[0]._dword;
					if (renderTransition[0].nextColorIndex >= (currentPrg.numColors)-1) {
						renderTransition[0].nextColorIndex = 0; // 1?
					} else {
						++(renderTransition[0].nextColorIndex);
					}
				}
				
				for (itColor = 0; itColor < 4; ++itColor) {
					// linear interpolation between colors
					// out = old * MaxSteps-TrPos + new * TrPos
					_tmpColor._byte[itColor] = (uint8_t) ((	(uint32_t)tmpFrame[0]._byte[itColor] * (PRG_MAX_TRANSITION_STEPS - (uint32_t)(renderTransition[0].Pos) ) +
														(uint32_t)currentPrg.colorList[renderTransition[0].nextColorIndex]._byte[itColor] * (uint32_t)(renderTransition[0].Pos) ) / PRG_MAX_TRANSITION_STEPS);
				}
				
				for (itLamp = 0; itLamp < NUM_LAMPS; ++itLamp) {
					GetBackBuffer()[itLamp]._dword = _tmpColor._dword;
				}
				
				renderTransition[0].Pos += currentPrg.speed;
				if (renderTransition[0].Pos >= PRG_MAX_TRANSITION_STEPS) {
					renderTransition[0].Pos = 0;
				}

			} else {
				// multiple color rendering
				for (itOffset = 0; itOffset < currentPrg.offsets; ++itOffset) {
					
					// Using only transitionPos[0]
					if (renderTransition[itOffset].Pos == 0) {
						// Save current showed color
						tmpFrame[itOffset]._dword = GetFrontBuffer()[itOffset]._dword;
						if (renderTransition[itOffset].nextColorIndex >= (currentPrg.numColors)-1) {
							renderTransition[itOffset].nextColorIndex = 0; // 1?
							} else {
							++(renderTransition[itOffset].nextColorIndex);
						}
					}
				
					for (itColor = 0; itColor < 4; ++itColor) {
						// linear interpolation between colors
						// out = old * MaxSteps-TrPos + new * TrPos
						_tmpColor._byte[itColor] = (uint8_t) ((	(uint32_t)tmpFrame[itOffset]._byte[itColor] * (PRG_MAX_TRANSITION_STEPS - (uint32_t)(renderTransition[itOffset].Pos) ) +
						(uint32_t)currentPrg.colorList[renderTransition[itOffset].nextColorIndex]._byte[itColor] * (uint32_t)(renderTransition[itOffset].Pos) ) / PRG_MAX_TRANSITION_STEPS);
					}

					GetBackBuffer()[itOffset]._dword = _tmpColor._dword;
					
					renderTransition[itOffset].Pos += currentPrg.speed;
					if (renderTransition[itOffset].Pos >= PRG_MAX_TRANSITION_STEPS) {
						renderTransition[itOffset].Pos = 0;
					}
				}
			}
		}
		// finished -------------------------------------
		
		
		// in Render/Output Eventgroup Bit setzen
		xEventGroupSetBits(*(prgTaskParam->pRenderOutputCycleEventgroup), ROC_RENDERING_FINISHED_READY_FOR_OUTPUT);
	}
	
}

// -------------------------------------------------------------------------------------------------