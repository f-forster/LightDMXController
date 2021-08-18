/*
 * time_controller.c
 *
 * Created: 17.03.2018 19:43:52
 *  Author: Franz
 */ 


// -------------------------------------------------------------------------------------------------
#include <asf.h>
#include "user_board.h"
#include "time_controller.h"
#include "core/taskhost.h"

// -------------------------------------------------------------------------------------------------
// Constants (local for access control)
// -------------------------------------------------------------------------------------------------

#define CONF_GENERAL_TIMER_TC_MODULE		TC3
#define CONF_DMX_OUTPUT_CYCLE_TC_MODULE		TC4

// -------------------------------------------------------------------------------------------------
// Variables (local for access control)
// -------------------------------------------------------------------------------------------------

static struct tc_module tcmGeneral;
static struct tc_module tcmDMXOutput;

uint32_t timebase = 0;

// -------------------------------------------------------------------------------------------------
// Prototypes (local for access control)
// -------------------------------------------------------------------------------------------------


void tc_callback_increment_timebase (struct tc_module *const module_inst);
void tc_callback_dmx_output_cycle (struct tc_module *const module_inst);

void CheckResetButton(void);

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
status_code_genare_t TC_Init (void)
{
	struct tc_config config_tc;
	
	// General Timer ----------------------------
	tc_get_config_defaults(&config_tc);

	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_1; // -> 32768 Hz
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
	config_tc.counter_8_bit.period = 32; // -> ~1kHz

	tc_init(&tcmGeneral, CONF_GENERAL_TIMER_TC_MODULE, &config_tc);
	
	
	tc_register_callback(&tcmGeneral, tc_callback_increment_timebase, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tcmGeneral, TC_CALLBACK_OVERFLOW);
	// /General Timer ----------------------------
	
	
	// DMX Output Cycle Timer --------------------
	tc_get_config_defaults(&config_tc);

	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_1; // ->32768 Hz
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV8; // sollte div 8 sein
	config_tc.counter_8_bit.period = 123; // mit (123) wirds ca 33 hz-> ~33Hz

	tc_init(&tcmDMXOutput, CONF_DMX_OUTPUT_CYCLE_TC_MODULE, &config_tc);
	
	
	tc_register_callback(&tcmDMXOutput, tc_callback_dmx_output_cycle, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tcmDMXOutput, TC_CALLBACK_OVERFLOW);
	
	// DMX Output Cycle Timer --------------------

	return STATUS_OK;
}

// -------------------------------------------------------------------------------------------------
void	TC_Start(eTimerModule tcModul)
{
	if (tcModul & TIMER_GENERAL) {
		tc_enable(&tcmGeneral);
	}
	if (tcModul & TIMER_DMX_OUTPUT) {
		tc_enable(&tcmDMXOutput);	
	}
}


// -------------------------------------------------------------------------------------------------
void tc_callback_increment_timebase (struct tc_module *const module_inst)
{
	timebase++;
	CheckResetButton();
}


// -------------------------------------------------------------------------------------------------
void tc_callback_dmx_output_cycle (struct tc_module *const module_inst)
{
	// ISR feuert mit ca 33Hz und stößt den DMX-Output Task an
	
	// We should switch context so the ISR returns to a different task.
	// NOTE:  How this is done depends on the port you are using.  Check
	// the documentation and examples for your port.
	// see https://www.freertos.org/taskresumefromisr.html
	portYIELD_FROM_ISR(xTaskResumeFromISR(DMXOutputTaskHandle));
}

// -------------------------------------------------------------------------------------------------
uint32_t	TC_GetTb(void)
{
	return timebase;
}

// -------------------------------------------------------------------------------------------------
void CheckResetButton(void)
{
	static uint32_t resetTimer = 0;
	
	if (ioport_get_pin_level(BUTTON1_PIN) == BUTTON1_PRESSED) {
		++resetTimer;
		if (resetTimer > BUTTON1_TIME_TO_RESET) {
			NVIC_SystemReset();
			while (1) {
				asm volatile ("nop");
			}
		}
	} else {
		resetTimer = 0;
	}
	
}

