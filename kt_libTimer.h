/*
 * kt_libTimer.h
 *
 *  Created on: Nov 5, 2020
 *      Author: pkjena
 */
//=============================================================================
#ifndef _KT_LIB_TIMER_H_
#define _KT_LIB_TIMER_H_
//=============================================================================

#define SW_TIMER_SUCCESS 	1
#define SW_TIMER_FAILED 	0


typedef struct
{
	uint8_t Index;			//index number
	uint8_t Enabled;		//currently enabled
	uint8_t Reason;			//whats the reason
	uint8_t Timeout;		//timeout flag
	uint16_t Interval;		//Interval of timer to timeout
	uint16_t Counter;		//counter till Interval
	void (*cbFunc)(void);	//callback function
}st_swTimer;


uint8_t kt_libTimerCreate(st_swTimer* a_pswT, void (a_apFunc)(void));
void kt_libTimerDelete(st_swTimer* a_pswT);
void kt_libTimerStart(st_swTimer* a_pswT, uint8_t a_Reason, uint16_t a_Interval);
void kt_libTimerStop(st_swTimer* a_pswT);
uint8_t kt_libTimerIsEnabled(st_swTimer* a_pswT);
//=============================================================================
#endif /* _KT_LIB_TIMER_H_ */
//=============================================================================
