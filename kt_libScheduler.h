//========================================================
#ifndef _KT_LIB_SCHEDULER_H_
#define _KT_LIB_SCHEDULER_H_
//========================================================
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

//State Machines
#define STATE_TIME_START	0
#define STATE_TIME_END		1
#define STATE_TIME_DIFF		2

//Structure Templates
struct st_Timer
{
	uint16_t Interval;
	uint16_t Count;
	uint8_t Reserved;
	uint8_t Reason;
	uint8_t Enable;
	uint8_t Timeout;
};

//global function
void* kt_libSchLoop(void);										//called from thread
void kt_libSchSetInterval (uint16_t a_IntervalInMiliSecond);	//Setting required scheduler interval in mili-seconds
void kt_libSchSetCallback ( void (*a_pFunc) (void) );			//Setting Callback Function
//========================================================
#endif /* _KT_LIB_SCHEDULER_H_ */
//========================================================
