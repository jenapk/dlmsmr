/*
 * kt_libTimer.c
 *
 *  Created on: Nov 5, 2020
 *      Author: pkjena
 */

#include <stdio.h>
#include <stdint.h>
#include "kt_config_app.h"
#include "kt_libScheduler.h"
#include "kt_misc_func.h"
#include "kt_libTimer.h"

#define MAX_SW_TIMER		8
st_swTimer* fg_pTmr[MAX_SW_TIMER];			//array of timers
uint8_t fg_tmrCount;						//keep track of timer counts

static uint8_t kt_tmrGetFreeIndex(void);
//=============================================================================
//creates and initialize a sw timer. returns 1 : Success, 0 : Failed
uint8_t kt_libTimerCreate(st_swTimer* a_pswT, void (a_apFunc)(void))
{
	uint8_t l_retVal = 0;

	l_retVal = kt_tmrGetFreeIndex();

	if(l_retVal < MAX_SW_TIMER)
	{
		a_pswT->Index = l_retVal;
		a_pswT->Enabled = 0;
		a_pswT->Reason = 0;
		a_pswT->Counter = 0;
		a_pswT->Interval = 0;
		a_pswT->Timeout = 0;
		a_pswT->cbFunc = a_apFunc;

		return(SW_TIMER_SUCCESS);		//successful
	}
	else
	{
		return(SW_TIMER_FAILED);		//no free timer available
	}
}

//=============================================================================
//deletes a timer
void kt_libTimerDelete(st_swTimer* a_pswT)
{

}

//=============================================================================
// starts the timer
void kt_libTimerStart(st_swTimer* a_pswT, uint8_t a_Reason, uint16_t a_Interval)
{
	a_pswT->Reason = a_Reason;
	a_pswT->Counter = 0;
	a_pswT->Interval = a_Interval;
	a_pswT->Timeout = 0;

	a_pswT->Enabled = 1;
}

//=============================================================================
//stops the timer
void kt_libTimerStop(st_swTimer* a_pswT)
{
	a_pswT->Enabled = 0;
}

//=============================================================================
// returns enable status of timer
uint8_t kt_libTimerIsEnabled(st_swTimer* a_pswT)
{
	return(a_pswT->Enabled);
}

//=============================================================================
// returns next available free timer Index
static uint8_t kt_tmrGetFreeIndex(void)
{
	uint8_t l_retVal = 0xFF;

	for(uint8_t l_Count = 0; l_Count < MAX_SW_TIMER; l_Count++)
	{
		if(fg_pTmr[l_Count] == NULL)
		{
			l_retVal = l_Count;
			break;
		}
	}

	return(l_retVal);
}
