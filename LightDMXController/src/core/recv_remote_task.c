/*
 * recv_remote_task.c
 *
 * Created: 28.10.2020 09:55:43
 *  Author: Franz Forster
 */ 
#include <asf.h>
#include "core/recv_remote_task.h"
#include "core/LT89xx.h"
#include "dmx/dmx_output_task.h"
#include "common/dbg_tools.h"

// -------------------------------------------------------------------------------------------------
// Constants (local for access control)
// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------
// Variables (local for access control)
// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------
// Prototypes (local for access control)
// -------------------------------------------------------------------------------------------------

void _RemoteSetup(void);

// -------------------------------------------------------------------------------------------------
/*
	Description: 

*/
void Recv_Remote_Task(void* param)
{
	QueueHandle_t* pRemoteCommandQueue = (QueueHandle_t*)param;
	
	_RemoteSetup();
	
	while (1)
	{
		
		
	}
}


void _RemoteSetup(void)
{
	LT89XX_Init();
	
	
}