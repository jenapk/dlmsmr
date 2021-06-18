
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include "kt_misc_func.h"
#include "functh.h"
#include "kt_global.h"
#include "kt_analyze.h"


extern uint8_t fg_appFnIndex;


//some variables are global between functh.c and kt_analyze.c
//=================================================================================================
//all frames are getting analyzed here
void kt_appAnalyze(void)
{
	printf("Frame analyzed inside kt_appAnalyze()\n");

	if(fg_appFnIndex == 0x00)
	{
		//check validity
	}

	fg_appFnIndex++;

	if(fg_appFnIndex < 2)
	{
		printf("Next Function\n");
		kt_appSetStateNext();
	}
	else
	{
		kt_appSetStateIdle();
		printf("Downloading finished\n");
	}
}
