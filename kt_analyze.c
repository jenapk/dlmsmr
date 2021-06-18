
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
extern unsigned char g_rxBuffer[];

kt_st_frameFormat fg_frmInfo;	//frame format info

//some variables are global between functh.c and kt_analyze.c
//=================================================================================================
//all frames are getting analyzed here
void kt_appAnalyze(void)
{
	printf("\nFrame received : ");

	//frame format
	kt_appGetFrameFormat();

	//checking the control byte
	if( (g_rxBuffer[5] == 0x83) || (g_rxBuffer[5] == 0x93) )
	{
		printf("SNRM (set normal response mode)\n");
	}
	else if( (g_rxBuffer[5] == 0x43) || (g_rxBuffer[5] == 0x53) )
	{
		printf("DISC (disconnect)\n");
	}
	else if( (g_rxBuffer[5] == 0x63) || (g_rxBuffer[5] == 0x73) )
	{
		printf("UA (unnumbered association)\n");
	}
	else if( (g_rxBuffer[5] == 0x0F) || (g_rxBuffer[5] == 0x1F) )
	{
		printf("DM (disconnected mode)\n");
	}
	else if( (g_rxBuffer[5] == 0x87) || (g_rxBuffer[5] == 0x97) )
	{
		printf("FRMR (frame reject)\n");
	}
	else if( (g_rxBuffer[5] == 0x03) || (g_rxBuffer[5] == 0x13) )
	{
		printf("UI (unnumbered information)\n");
	}
	else if(g_rxBuffer[5] & 0x0F == 0x00)
	{

	}

	//jena :I, RR and RNR frames are pending


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

void kt_appGetFrameFormat(void)
{
	fg_frmInfo.Format  = (g_rxBuffer[1] >> 4);
	fg_frmInfo.Segmented = (g_rxBuffer[1] & 0x10) >> 3;

	fg_frmInfo.Length = g_rxBuffer[1] & 0x07;
	fg_frmInfo.Length <<= 3;
	fg_frmInfo.Length |= g_rxBuffer[2];

	printf("Frame Format : %02X, %02X, %04X\n", fg_frmInfo.Format, fg_frmInfo.Segmented, fg_frmInfo.Length );
}
