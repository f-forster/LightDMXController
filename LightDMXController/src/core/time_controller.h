/*
 * time_controller.h
 *
 * Created: 17.03.2018 19:44:06
 *  Author: Franz
 */ 


#ifndef TIME_CONTROLLER_H_
#define TIME_CONTROLLER_H_



// -------------------------------------------------------------------------------------------------

typedef enum {
	TIMER_GENERAL		= (1<<0),
	TIMER_DMX_OUTPUT	= (1<<1),
	
	TIMER_ALL			= 0xFF
} eTimerModule;


// -------------------------------------------------------------------------------------------------

extern enum status_code		TC_Init(void);
extern void					TC_Start(eTimerModule tcModul);
extern uint32_t				TC_GetTb(void);







#endif /* TIME_CONTROLLER_H_ */