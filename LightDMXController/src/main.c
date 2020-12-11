/************************************************************************/
/* License here                                                                 */
/************************************************************************/

#include <asf.h>
#include "common/dbg_tools.h"
#include "dmx/dmx_output_task.h"
#include "core/taskhost.h"
#include "core/time_controller.h"



// -------------------------------------------------------------------------------------------------

int main (void)
{
	// 1st
	// Fundamental initialisation (System! not my stuff!)
	// Clocks Status LEDs, Debug pins
	system_init();

	// 2nd
	// Init of Debug output (serial interface)
	DBG_Init();
	
	// 3rd
	// DMX Output Interface
	DMX_Init();
	
	// 4th
	// Init of Timer Counter Module
	TC_Init();
	
	
	// 5th
	// Tasker Initialisation
	Taskhost_Start();


	TC_Start(TIMER_ALL);
	
	
	// LAST!
	// ..and let FreeRTOS run tasks!
	vTaskStartScheduler();

	while (1) {
		// infinity
	}
}


/*
Git Push Commands, weil ich mir es nicht merken kann....

git push origin --all

*/