//========================================================
#ifndef _TIMER_H_
#define _TIMER_H_
//========================================================
#include "typedef.h"

//State Machines
#define STATE_TIME_START	0
#define STATE_TIME_END		1
#define STATE_TIME_DIFF		2

//Structure Templates
struct st_Timer
{
	_UINT Interval;
	_UINT Count;
	_UBYTE Reserved;
	_UBYTE Reason;
	_UBYTE Enable;
	_UBYTE Timeout;
};

//global fuction
void APP_SchProcess (void);										//Calling from main while(1)
void APP_SchSetInterval (_UINT a_IntervalInMiliSecond);			//Setting required scheduler interval in mili-seconds
void APP_SchSetCallback ( void (*a_pFunc) (void) );				//Setting Callback Function
//========================================================
#endif /* _TIMER_H_ */
//========================================================
