#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "timer.h"
#include "typedef.h"
#include "Config.h"

static struct timeval fg_Start, fg_End, fg_Diff;
static _UBYTE fg_StateScheduler;
static _SLONGLONG fg_Interval = 0;
static _UBYTE fg_Temp = 0;

static void (*CB_SchFunction) (void) = 0x00;		//callback function on receiving
static _SLONGLONG APP_SchGetTimeDiff(struct timeval *a_psDiff, struct timeval *a_psEnd, struct timeval *a_psStart);
//===============================================================================================================
//This function will being called from the main loop.
void APP_SchProcess (void)
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

			if(APP_SchGetTimeDiff (&fg_Diff, &fg_End, &fg_Start) > fg_Interval)
			{
				if (!CB_SchFunction)
				{
					fg_Interval = 0x00;
				}
				else
				{
					CB_SchFunction();
#ifdef LINUX_ATMEL
					fprintf(stderr, "*"); 	//outward symbol
					goto_xy(7,20);
					if(fg_Temp == 0)
					{
						LcdPrintf((_SBYTE*) "|");
						fg_Temp = 1;
					}
						else if(fg_Temp == 1){ LcdPrintf((_SBYTE*) "/");fg_Temp = 2; }
					else if(fg_Temp == 2){ LcdPrintf((_SBYTE*) "-");fg_Temp = 3; }
					else { LcdPrintf((_SBYTE*) "\\");fg_Temp = 0; }
#else
					fprintf(stderr, "*"); 	//outward symbol
#endif
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
void APP_SchSetInterval (_UINT a_IntervalInMiliSecond)
{
	fg_Interval = (a_IntervalInMiliSecond * 1000);

	//if interval is set to 0, do not point to any function
	if (!fg_Interval) CB_SchFunction = 0x00;

	return;
}

//===============================================================================================================
//To set the callback function, on each scheduler interval
void APP_SchSetCallback ( void (*a_pFunc) (void) )
{
	CB_SchFunction = a_pFunc;

	return;
}
//===============================================================================================================
static _SLONGLONG APP_SchGetTimeDiff(struct timeval *a_psDiff, struct timeval *a_psEnd, struct timeval *a_psStart)
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
//================================================================================================================
