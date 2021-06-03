
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include "kt_libScheduler.h"

static struct timeval fg_Start, fg_End, fg_Diff;
static uint8_t fg_StateScheduler = 0;
static uint64_t fg_Interval = 0;
//static uint8_t fg_schCharCount;
static void (*CB_SchFunction) (void) = 0x00;		//callback function on receiving

static void kt_libSchProcess (void);
static uint64_t kt_libSchGetTimeDiff(struct timeval *a_psDiff, struct timeval *a_psEnd, struct timeval *a_psStart);
//===============================================================================================================
//This function will being called from the main loop.
void* kt_libSchLoop(void)
{
	while(1)
	{
		kt_libSchProcess();
	}
}

//===============================================================================================================
static void kt_libSchProcess (void)
{
	if (fg_Interval == 0x00) return;

	switch (fg_StateScheduler)
	{
		//------------------------------------------------------------------------
		case STATE_TIME_START:
			gettimeofday(&fg_Start, NULL);
			fg_StateScheduler = STATE_TIME_END;
			break;
		//------------------------------------------------------------------------
		case STATE_TIME_END:
			gettimeofday (&fg_End, NULL);
			fg_StateScheduler = STATE_TIME_DIFF;
			break;
		//------------------------------------------------------------------------
		case STATE_TIME_DIFF:
			fg_StateScheduler = STATE_TIME_END;

			if(kt_libSchGetTimeDiff (&fg_Diff, &fg_End, &fg_Start) > fg_Interval)
			{
				if (!CB_SchFunction)
				{
					fg_Interval = 0x00;
				}
				else
				{
					CB_SchFunction();
//					fg_schCharCount++;
//
//					if(fg_schCharCount >= 80)
//					{
//						printf("\n"); 	//outward symbol
//						fg_schCharCount = 0x00;
//					}
//					else
//					{
//						printf("=");
//					}
				}

				fg_StateScheduler = STATE_TIME_START;
			}
			break;
		//------------------------------------------------------------------------
		default:
			fg_Interval = 0x00;		//Stop the Scheduler
			fg_StateScheduler = STATE_TIME_START;
			break;
	}

	return;
}

//===============================================================================================================
//To set the Interval of scheduler. resolution in 1uSec. If 0 set, scheduler will be disabled
void kt_libSchSetInterval (uint16_t a_IntervalInMiliSecond)
{
	fg_Interval = (a_IntervalInMiliSecond * 1000);

	//if interval is set to 0, do not point to any function
	if (!fg_Interval) CB_SchFunction = 0x00;

	return;
}

//===============================================================================================================
//To set the callback function, on each scheduler interval
void kt_libSchSetCallback ( void (*a_pFunc) (void) )
{
	CB_SchFunction = a_pFunc;

	return;
}
//===============================================================================================================
static uint64_t kt_libSchGetTimeDiff(struct timeval *a_psDiff, struct timeval *a_psEnd, struct timeval *a_psStart)
{
	a_psDiff->tv_sec = a_psEnd->tv_sec - a_psStart->tv_sec ;
	a_psDiff->tv_usec = a_psEnd->tv_usec - a_psStart->tv_usec;

	//Using while instead of if below makes the code slightly more robust
	while(a_psDiff->tv_usec < 0)
	{
		a_psDiff->tv_usec += 1000000;
		a_psDiff->tv_sec -= 1;
	}

	return (1000000LL * a_psDiff->tv_sec + a_psDiff->tv_usec);
}
