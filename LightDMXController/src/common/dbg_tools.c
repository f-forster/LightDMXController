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





//////////////////////////////////////////////////////////////////////////
// OUTDATED 

/*


static void _DBG_Val (uint8_t dbgClass, const char* str, uint32_t val, uint8_t format);


void DBG_Msg		(uint8_t dbgClass, const char* str)
{
	char buffer[20]; // enough for signed uint32_t
	
	if (dbgClass >= DBG_LEVEL) {

		sprintf(buffer, "%10u: ", (unsigned int) xTaskGetTickCount());
		dbg_print_str(buffer);
		switch (dbgClass) {
			case DBG_ERROR:
			dbg_print_str("[ERROR] ");
			break;
			case DBG_WARNING:
			dbg_print_str("[WARN]  ");
			break;
			case DBG_INFO:
			dbg_print_str("[INFO]  ");
			break;
			case DBG_TRACE:
			dbg_print_str("[TRACE] ");
			break;
		}

		dbg_print_str(str);
		dbg_print_str("\r\n");
	}
	

}




void _DBG_Val (uint8_t dbgClass, const char* str, uint32_t val, uint8_t format)
{
	char buffer[20]; // enough for signed uint32_t
	
	if (dbgClass >= DBG_LEVEL) {
		
		sprintf(buffer, "%10u: ", (unsigned int) xTaskGetTickCount());
		dbg_print_str(buffer);
		switch (dbgClass) {
			case DBG_ERROR:
				dbg_print_str("[ERROR] ");
			break;
			case DBG_WARNING:
				dbg_print_str("[WARN]  ");
			break;
			case DBG_INFO:
				dbg_print_str("[INFO]  ");
			break;
			case DBG_TRACE:
				dbg_print_str("[TRACE] ");
			break;
		}
		dbg_print_str(str);
		
		switch (format) {
			case FORMAT_DEC:
				utoa(val, buffer, 10);
			break;
			case FORMAT_DEC_SIGNED:
				itoa((int32_t)val, buffer, 10);
			break;
			case FORMAT_HEX:
				utoa(val, buffer, 16);
			break;
		}

		dbg_print_str(buffer);
		dbg_print_str("\r\n");
	}

}


inline void	DBG_ValD	(uint8_t dbgClass, const char* str, uint32_t val)
{
	_DBG_Val(dbgClass, str, val, FORMAT_DEC);
}

inline void	DBG_ValSD	(uint8_t dbgClass, const char* str, int32_t val)
{
	_DBG_Val(dbgClass, str, (uint32_t)val, FORMAT_DEC_SIGNED);
}

inline void	DBG_ValH	(uint8_t dbgClass, const char* str, uint32_t val)
{
	_DBG_Val(dbgClass, str, val, FORMAT_HEX);
}



status_code_genare_t DBG_Report	(uint8_t dbgClas, status_code_genare_t status)
{


	return status;
}


*/