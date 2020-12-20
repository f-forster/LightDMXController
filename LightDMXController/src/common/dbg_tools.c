/*
 * dbg_tools.c
 *
 * Created: 05.12.2018 21:33:11
 *  Author: Franz Forster
 */ 

#include <asf.h>
#include "printf.h"
#include "dbg_tools.h"

#ifndef DBG_LEVEL
#define DBG_LEVEL DBG_WARNING
#endif


#define FORMAT_DEC			0
#define FORMAT_DEC_SIGNED	1
#define FORMAT_HEX			2


// -------------------------------------------------------------------------------------------------
/*
	Description: 

*/
void DBG_Init()
{
	dbg_init();
	printf("\r\n\v\v\v-------------------------------------------------------------\r\nLINAR Controller (alpha) starting...\r\n");
	
}

// -------------------------------------------------------------------------------------------------
// putchar implementation for printf output
void _putchar(char character)
{
	dbg_print_char(character);
}


