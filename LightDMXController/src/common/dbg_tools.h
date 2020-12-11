/*
 * dbg_tools.h
 *
 * Created: 05.12.2018 21:33:22
 *  Author: Franz Forster
 */ 


#ifndef DBG_TOOLS_H_
#define DBG_TOOLS_H_

#include "common/printf.h"

// -------------------------------------------------------------------------------------------------
// Debug levels/classes 
// -------------------------------------------------------------------------------------------------

#define DBG_LEVEL						DBG_LEVEL_INFO

// -------------------------------------------------------------------------------------------------

#define DBG_LEVEL_NO_DEBUG_OUTPUT		0
#define DBG_LEVEL_ERROR					1
#define DBG_LEVEL_WARNING				2
#define DBG_LEVEL_INFO					3
#define DBG_LEVEL_TRACE					4


#define DBG_PRINT(...)
#define DBG_ERR(...)
#define DBG_WARN(...)
#define DBG_INFO(...)
#define DBG_TRACE(...)


#define DBG_OUT_PRINTF		printf_
#define DBG_OUT_PRINTSTR	dbg_print_str

#if (DBG_LEVEL > DBG_LEVEL_NO_DEBUG_OUTPUT)
#undef DBG_PRINT
#define DBG_PRINT(...)							do{DBG_OUT_PRINTF(__VA_ARGS__);DBG_OUT_PRINTSTR("\r");}while(0)
#if (DBG_LEVEL >= DBG_LEVEL_ERROR)
#undef DBG_ERR
#define DBG_ERR(...)							do{DBG_OUT_PRINTF("[LI](ERR)[%s][%d] ",__FUNCTION__,__LINE__); DBG_OUT_PRINTF(__VA_ARGS__);DBG_OUT_PRINTSTR("\r");}while(0)
#if (DBG_LEVEL >= DBG_LEVEL_WARNING)
#undef DBG_WARN
#define DBG_WARN(...)							do{DBG_OUT_PRINTSTR("[LI](WARN) "); DBG_OUT_PRINTF(__VA_ARGS__);DBG_OUT_PRINTSTR("\r");}while(0)
#if (DBG_LEVEL >= DBG_LEVEL_INFO)
#undef DBG_INFO
#define DBG_INFO(...)							do{DBG_OUT_PRINTSTR("[LI](INFO) "); DBG_OUT_PRINTF(__VA_ARGS__);DBG_OUT_PRINTSTR("\r");}while(0)
#if (DBG_LEVEL >= DBG_LEVEL_TRACE)
#undef DBG_TRACE
#define DBG_TRACE(...)							do{DBG_OUT_PRINTF("[LI](TRACE)[%s][%d] ",__FUNCTION__,__LINE__); DBG_OUT_PRINTF(__VA_ARGS__);DBG_OUT_PRINTSTR("\r");}while(0)
#endif /*DBG_TRAC*/
#endif /*DBG_INFO*/
#endif /*DBG_WARN*/
#endif /*DBG_ERR*/
#endif /*CONF_WINC_DEBUG */




extern void DBG_Init(void);


/*
// outdated. The macros above are way better... 
extern void			DBG_Msg	(uint8_t dbgClass, const char* str);
extern void			DBG_ValD	(uint8_t dbgClass, const char* str, uint32_t val);
extern void			DBG_ValSD	(uint8_t dbgClass, const char* str, int32_t val);
extern void			DBG_ValH	(uint8_t dbgClass, const char* str, uint32_t val);
extern status_code_genare_t DBG_Report	(uint8_t dbgClas, status_code_genare_t status);
*/






#endif /* DBG_TOOLS_H_ */