/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	// for LEDs
	ioport_set_pin_dir(LED_RED_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_GREEN_PIN, IOPORT_DIR_OUTPUT);
	// LED Test
	ioport_set_pin_level(LED_RED_PIN, LED_ACTIVE);
	ioport_set_pin_level(LED_GREEN_PIN, LED_ACTIVE);
	
		
}