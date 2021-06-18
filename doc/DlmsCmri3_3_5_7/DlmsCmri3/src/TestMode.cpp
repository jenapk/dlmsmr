/*
 * TestMode.cpp
 *
 *  Created on: 26-Sep-2015
 *      Author: mundel
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/rtc.h>
#include <dirent.h>
#include <pthread.h>

#include "display.h"
#include "Config.h"
#include "global.h"
#include "typedef.h"
#include "FUNCTH.h"
#include "rs232.h"
#include "utility.h"
#include "display.h"
#include "hhuLcd.h"
#include "AES128.h"
#include "comm.h"
#include "pdsequivalent.h"
#include "Upload.h"
#include "auth.h"
#include "Program.h"
#include "viewMode.h"
#include "TestMode.h"

static struct rtc_time fg_StartRtc;
static _ULONG fg_StartUTC;
static _ULONG fg_CurrenttUTC;
struct rtc_time fg_LastRtc;
static char fg_Reading;
static char fg_LagFlag;
static char fg_LeadFlag;
static char fg_QuardentPresence;
static double fg_Prv_kwh;
static double fg_Prv_kvah;
static double fg_Prv_kvarhLead;
static double fg_Prv_kvarhLag;
//for Type B meter ParaMeter
static double fg_Prv_kwhImp;
static double fg_Prv_kwhExp;
static double fg_Prv_kvahImp;
static double fg_Prv_kvahExp;
static double fg_Prv_kvarhq1;
static double fg_Prv_kvarhq2;
static double fg_Prv_kvarhq3;
static double fg_Prv_kvarhq4;

static char fg_KeyDetection;
RtcTypeDef fg_RtcTimer;

enum
{
	FIRST_READING = 1,
	FIRST_READING_B_TYPE_NEXT_DATA,
	WAITING_USER_INTERACTION,
	SECOND_READING,
	SECOND_READING_B_TYPE_NEXT_DATA,
	READING_DIFFERENCE
};
void Test_Mode (void)
{

	char l_Start = 0;
	l_Start = 0;
	l_Start = TestModeScr();
    g_UserAssociation = MR_ASSOCIATION;

    if(l_Start == '0')
    {
    	return;
    }

	if(type == 2)
	{
		Test_Mode_TypeB();
	}
	else
	{
		Test_Mode_TypeA_C();
	}
}
//============================================================================
//TSTMD:
char TestModeScr(void)
{
	char l_Choice = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)" * TEST MODE *");
	HAL_DebugPrint(2, (_SBYTE*)"1.Start");
	HAL_DebugPrint(3, (_SBYTE*)"0.Back");
	HAL_DebugPrint(4, (_SBYTE*)"Press 1 for start");

	while((l_Choice != '1') && (l_Choice != '0'))
	{
		l_Choice = _getkey();
	}

	return (l_Choice);
}
//============================================================================
void Test_Mode_TypeB(void)
{
	char l_RetVal = 0;
	LcdClear();
	fg_Reading = FIRST_READING;

	while(1)
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
		rrr_c=0;
		rrr_c1=0;
		rrr_s=0;
		sss_c=0;
		sss_c1=0;
		sss_s=0;

		l_RetVal = SetNormalResponseMode();
		if(l_RetVal == 1)
		{
			l_RetVal = 0;
			l_RetVal = AssociationRequest();
			if(l_RetVal == 1)
			{
				if(TestModeBReading() == 0)
				{
					break;
				}
			}
			else
			{
				LcdClear();
				HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL.");
				HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
				HAL_DebugPrint(3, (_SBYTE*)"PRESS 0 FOR RETURN");
				Disconnect();
				while(l_RetVal != '0')
				{
					l_RetVal = _getkey();
				}
				if(l_RetVal == '0')
				break; //while(1) break
			}
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL.");
			HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
			HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 FOR RETURN");
			Disconnect();
			while(l_RetVal != '0')
			{
				l_RetVal = _getkey();
			}
			if(l_RetVal == '0')
			break;
		}
	}
}
//============================================================================
void Test_Mode_TypeA_C(void)
{
	char l_RetVal = 0;
	LcdClear();
	fg_Reading = FIRST_READING;

	while(1)
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
		rrr_c=0;
		rrr_c1=0;
		rrr_s=0;
		sss_c=0;
		sss_c1=0;
		sss_s=0;

		l_RetVal = SetNormalResponseMode();
		if(l_RetVal == 1)
		{
			l_RetVal = 0;
			l_RetVal = AssociationRequest();
			if(l_RetVal == 1)
			{
				if(TestModeACReading() == 0)
				{
					break;
				}
			}
			else
			{
				LcdClear();
				HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL.");
				HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
				HAL_DebugPrint(3, (_SBYTE*)"PRESS 0 FOR RETURN");
				Disconnect();
				while(l_RetVal != '0')
				{
					l_RetVal = _getkey();
				}
				if(l_RetVal == '0')
				break; //while(1) break
			}
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL.");
			HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
			HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 FOR RETURN");
			Disconnect();
			while(l_RetVal != '0')
			{
				l_RetVal = _getkey();
			}
			if(l_RetVal == '0')
			break;
		}
	}
}
//========================================================================
char TestModeACReading(void)
{
	struct rtc_time l_Rtc;
	char l_RetVal = 0;
	double l_AP_POWER = 0;
	_ULONG l_TempVal = 0;

	switch(fg_Reading)
	{
		case FIRST_READING:
			HAL_DebugPrint(1, (_SBYTE*)"READING(1) [HH:MM:SS]");
			UTL_RTCRead(&l_Rtc);
			fg_StartUTC = UTL_GetHHUUTCTime();
			fg_StartRtc.tm_hour = l_Rtc.tm_hour;
			fg_StartRtc.tm_min = l_Rtc.tm_min;
			fg_StartRtc.tm_sec = l_Rtc.tm_sec;
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((_SBYTE*)g_DispBuffer, "       Time[%02d:%02d:%02d]", fg_StartRtc.tm_hour, fg_StartRtc.tm_min, fg_StartRtc.tm_sec);
			HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);

			l_RetVal = ViewRequest(3,1,0,1,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kwh = l_AP_POWER;
			}
			l_RetVal = ViewRequest(3,1,0,1,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(3, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"");
			}

			l_RetVal = ViewRequest(3,1,0,5,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvarhLag = l_AP_POWER;
			}
			l_RetVal = ViewRequest(3,1,0,5,8,0,255,3);
			if(l_RetVal)
			{
				fg_LagFlag = 1;
				Display_Value(4, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"-Lg");
			}
			else
			{
				fg_LagFlag = 0;
			}

			l_RetVal = ViewRequest(3,1,0,8,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvarhLead = l_AP_POWER;
			}
			l_RetVal = ViewRequest(3,1,0,8,8,0,255,3);

			if(l_RetVal)
			{
				fg_LeadFlag =1;
				Display_Value(5, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"-Ld");
			}
			else
			{
				fg_LeadFlag = 0;
			}

			l_RetVal = ViewRequest(3,1,0,9,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvah = l_AP_POWER;
			}
			l_RetVal = ViewRequest(3,1,0,9,8,0,255,3);

			if(l_RetVal == 1)
			{
				Display_Value(6, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"");
			}

			Disconnect();
			HAL_DebugPrint(7, (_SBYTE*)"press any key..");
			_getkey();
			fg_Reading = WAITING_USER_INTERACTION;
			break;
		//------------------------------------------------------
		case WAITING_USER_INTERACTION:
			Disconnect();
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"           [HH:MM:SS]");
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((_SBYTE*)g_DispBuffer, "Start Time [%02d:%02d:%02d]", fg_StartRtc.tm_hour, fg_StartRtc.tm_min, fg_StartRtc.tm_sec);
			HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
			fg_KeyDetection = 0;
			while(fg_KeyDetection != '1')
			{
				CreateKeyDetectionThread();
				sleep(1);
				UTL_RTCRead(&l_Rtc);
				fg_CurrenttUTC = UTL_GetHHUUTCTime();
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "RTC        [%02d:%02d:%02d]", l_Rtc.tm_hour, l_Rtc.tm_min, l_Rtc.tm_sec);
				HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				l_TempVal = fg_CurrenttUTC -fg_StartUTC;
				ConvertUTCtoRTC(l_TempVal, &fg_RtcTimer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "Timer      [%02d:%02d:%02d]", fg_RtcTimer.Hour, fg_RtcTimer.Minute, fg_RtcTimer.Second);
				HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				//HAL_DebugPrint(5, (_SBYTE*)"Hold Test Time");
				HAL_DebugPrint(6, (_SBYTE*)"Press 1 to Continue");
			}

			if(fg_KeyDetection == '1')
			{
				fg_KeyDetection = 0;
				fg_Reading = SECOND_READING;
			}

			//while(l_RetVal != '1')
			//l_RetVal = _getkey();
			break;
		//------------------------------------------------------
		case SECOND_READING:
			HAL_DebugPrint(1, (_SBYTE*)"READING (2)....");
			l_RetVal = ViewRequest(3,1,0,1,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kwh = l_AP_POWER - fg_Prv_kwh;
			}

			l_RetVal = ViewRequest(3,1,0,1,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(2, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"");
			}

			l_RetVal = ViewRequest(3,1,0,5,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvarhLag = l_AP_POWER - fg_Prv_kvarhLag;
			}
			l_RetVal = ViewRequest(3,1,0,5,8,0,255,3);

			if(l_RetVal)
			{
				Display_Value(3, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"-Lg");
			}

			l_RetVal = ViewRequest(3,1,0,8,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvarhLead = l_AP_POWER-fg_Prv_kvarhLead;
			}

			l_RetVal = ViewRequest(3,1,0,8,8,0,255,3);

			if(l_RetVal)
			{
				Display_Value(4, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"-Ld");
			}

			l_RetVal = ViewRequest(3,1,0,9,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvah = l_AP_POWER - fg_Prv_kvah;
			}

			l_RetVal = ViewRequest(3,1,0,9,8,0,255,3);
			if(l_RetVal)
			{

				Display_Value(5, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"");
			}

			Disconnect();

			HAL_DebugPrint(7, (_SBYTE*)"press any key..");
			_getkey();
			fg_Reading = READING_DIFFERENCE;
			break;
		//------------------------------------------------------
		case READING_DIFFERENCE:
			LcdClear();
			Disconnect();
			HAL_DebugPrint(1, (_SBYTE*)"Difference");
			if(g_displayPoint==3)
			{
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWh = %.3f",(double)fg_Prv_kwh/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);

				if(fg_LagFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.3f",(double)fg_Prv_kvarhLag/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				}

				if(fg_LeadFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.3f",(double)fg_Prv_kvarhLead/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				}
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.3f",(double)fg_Prv_kvah/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
			}
			else if(g_displayPoint==2)
			{
				fg_Prv_kwh *= 10;
				fg_Prv_kvarhLag *= 10;
				fg_Prv_kvarhLead *= 10;
				fg_Prv_kvah *= 10;
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWh=%.2f",(double)fg_Prv_kwh/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);

				if(fg_LagFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.2f",(double)fg_Prv_kvarhLag/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				}
				if(fg_LeadFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.2f",(double)fg_Prv_kvarhLead/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				}
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.2f",(double)fg_Prv_kvah/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
			}
			else if(g_displayPoint==1)
			{
				fg_Prv_kwh *= 100;
				fg_Prv_kvarhLag *= 100;
				fg_Prv_kvarhLead *= 100;
				fg_Prv_kvah *= 100;
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWh=%.1f",(double)fg_Prv_kwh/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);

				if(fg_LagFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.1f",(double)fg_Prv_kvarhLag/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				}
				if(fg_LeadFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.1f\n",(double)fg_Prv_kvarhLead/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				}
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.1f",(double)fg_Prv_kvah/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
			}
			else if(g_displayPoint == 4)//means meter sends 0 decimal
			{
				fg_Prv_kwh *= 1000;
				fg_Prv_kvarhLag *= 1000;
				fg_Prv_kvarhLead *= 1000;
				fg_Prv_kvah *= 1000;
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWh=%g",(double)fg_Prv_kwh/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);

				if(fg_LagFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%g",(double)fg_Prv_kvarhLag/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				}
				if(fg_LeadFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%g",(double)fg_Prv_kvarhLead/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				}
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAh=%g\n",(double)fg_Prv_kvah/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
			}
			else if(g_displayPoint == 6)//means meter sends 0 decimal
			{
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWh=%.6f",(double)fg_Prv_kwh/1000000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
				if(fg_LagFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.6f",(double)fg_Prv_kvarhLag/1000000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				}
				if(fg_LeadFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.6f",(double)fg_Prv_kvarhLead/1000000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				}

				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.6f",(double)fg_Prv_kvah/1000000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
			}
			else if(g_displayPoint == 5)//means meter sends 0 decimal
			{
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWh=%.6f",(double)fg_Prv_kwh/100000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);

				if(fg_LagFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.6f",(double)fg_Prv_kvarhLag/100000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				}
				if(fg_LeadFlag)
				{
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.6f",(double)fg_Prv_kvarhLead/100000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				}

				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.6f",(double)fg_Prv_kvah/100000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
			}

			HAL_DebugPrint(6, (_SBYTE*) "press any key..");
			_getkey();

			l_AP_POWER = 0;
			fg_Prv_kwh = 0;
			fg_Prv_kvah = 0;
			fg_Prv_kvarhLag = 0;
			fg_Prv_kvarhLead = 0;

			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"1.TEST AGAIN");
			HAL_DebugPrint(2, (_SBYTE*)"0.BACK ");
			while((l_RetVal != '1') && (l_RetVal != '0'))
				l_RetVal = _getkey();
			if(l_RetVal == '0')
			{
				return 0;
			}
			else if(l_RetVal == '1')
			{
				fg_Reading = FIRST_READING;
			}

			break;
		//------------------------------------------------------
		default:
			fg_Reading = FIRST_READING;
			break;
	}

	return 1;
}
//============================================================================

char TestModeBReading(void)
{
	struct rtc_time l_Rtc;
	char l_RetVal = 0;
	double l_AP_POWER = 0;
	double l_kvarhq1 = 0;
	double l_kvarhq2 = 0;
	double l_kvarhq3 = 0;
	double l_kvarhq4 = 0;

	_ULONG l_TempVal = 0;

	switch(fg_Reading)
	{
		case FIRST_READING:
			HAL_DebugPrint(1, (_SBYTE*)"READING(1) [HH:MM:SS]");
			UTL_RTCRead(&l_Rtc);
			fg_StartUTC = UTL_GetHHUUTCTime();
			fg_StartRtc.tm_hour = l_Rtc.tm_hour;
			fg_StartRtc.tm_min = l_Rtc.tm_min;
			fg_StartRtc.tm_sec = l_Rtc.tm_sec;
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((_SBYTE*)g_DispBuffer, "       Time[%02d:%02d:%02d]", fg_StartRtc.tm_hour, fg_StartRtc.tm_min, fg_StartRtc.tm_sec);
			HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);

			l_RetVal = ViewRequest(3,1,0,1,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kwhImp = l_AP_POWER;
			}

			l_RetVal = ViewRequest(3,1,0,1,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(3, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Imp)");
			}
			l_RetVal = ViewRequest(3,1,0,2,8,0,255,2);
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kwhExp = l_AP_POWER;
			}
			l_RetVal = ViewRequest(3,1,0,2,8,0,255,3);

			if(l_RetVal)
			{
				Display_Value(4, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Exp)");
			}

			l_RetVal = ViewRequest(3,1,0,9,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvahImp = l_AP_POWER;
			}
			l_RetVal = ViewRequest(3,1,0,9,8,0,255,3);

			if(l_RetVal)
			{
				Display_Value(5, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Imp)");
			}

			l_RetVal = ViewRequest(3,1,0,10,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvahExp = l_AP_POWER;
			}

			l_RetVal = ViewRequest(3,1,0,10,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(6, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Exp)");
			}

			l_RetVal = ViewRequest(3,1,0,5,8,0,255,2); //(Q1)
			if(l_RetVal)
			{
				l_kvarhq1 = fg_RxBuffer[16];
				l_kvarhq1 *= 0x100;
				l_kvarhq1 +=  fg_RxBuffer[17];
				l_kvarhq1 *= 0x100;
				l_kvarhq1 +=  fg_RxBuffer[18];
				l_kvarhq1 *= 0x100;
				l_kvarhq1 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq1 = l_kvarhq1;
			}

			l_RetVal = ViewRequest(3,1,0,6,8,0,255,2); //(Q2)
			if(l_RetVal)
			{
				l_kvarhq2 = fg_RxBuffer[16];
				l_kvarhq2 *= 0x100;
				l_kvarhq2 +=  fg_RxBuffer[17];
				l_kvarhq2 *= 0x100;
				l_kvarhq2 +=  fg_RxBuffer[18];
				l_kvarhq2 *= 0x100;
				l_kvarhq2 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq2 = l_kvarhq2;
			}

			l_RetVal = ViewRequest(3,1,0,7,8,0,255,2); ///(Q3)
			if(l_RetVal)
			{
				l_kvarhq3 = fg_RxBuffer[16];
				l_kvarhq3 *= 0x100;
				l_kvarhq3 +=  fg_RxBuffer[17];
				l_kvarhq3 *= 0x100;
				l_kvarhq3 +=  fg_RxBuffer[18];
				l_kvarhq3 *= 0x100;
				l_kvarhq3 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq3 = l_kvarhq3;
			}

			l_RetVal = ViewRequest(3,1,0,8,8,0,255,2); //(Q4)
			if(l_RetVal)
			{
				l_kvarhq4 = fg_RxBuffer[16];
				l_kvarhq4 *= 0x100;
				l_kvarhq4 +=  fg_RxBuffer[17];
				l_kvarhq4 *= 0x100;
				l_kvarhq4 +=  fg_RxBuffer[18];
				l_kvarhq4 *= 0x100;
				l_kvarhq4 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq4 = l_kvarhq4;
			}

			Disconnect();
			HAL_DebugPrint(7, (_SBYTE*)"press any key..");
			_getkey();
			fg_Reading = FIRST_READING_B_TYPE_NEXT_DATA;
			break;
		//------------------------------------------------------
		case FIRST_READING_B_TYPE_NEXT_DATA:
			HAL_DebugPrint(1, (_SBYTE*)"READING(1) [HH:MM:SS]");
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((_SBYTE*)g_DispBuffer, "       Time[%02d:%02d:%02d]", fg_StartRtc.tm_hour, fg_StartRtc.tm_min, fg_StartRtc.tm_sec);
			HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
			l_RetVal = ViewRequest(3,1,0,5,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(3, (_SBYTE*)"", l_kvarhq1, (_SBYTE*)"(Q1)");
			}

			l_RetVal = ViewRequest(3,1,0,6,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(4, (_SBYTE*)"", l_kvarhq2, (_SBYTE*)"(Q2)");
			}

			l_RetVal = ViewRequest(3,1,0,7,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(5, (_SBYTE*)"", l_kvarhq3, (_SBYTE*)"(Q3)");
			}

			l_RetVal = ViewRequest(3,1,0,8,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(5, (_SBYTE*)"", l_kvarhq4, (_SBYTE*)"(Q4)");
			}

			HAL_DebugPrint(7, (_SBYTE*)"press any key..");
			_getkey();
			fg_Reading = WAITING_USER_INTERACTION;
			break;
		//------------------------------------------------------
		case WAITING_USER_INTERACTION:
			Disconnect();
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"           [HH:MM:SS]");
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((_SBYTE*)g_DispBuffer, "Start Time [%02d:%02d:%02d]", fg_StartRtc.tm_hour, fg_StartRtc.tm_min, fg_StartRtc.tm_sec);
			HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
			fg_KeyDetection = 0;
			while(fg_KeyDetection != '1')
			{
				CreateKeyDetectionThread();
				sleep(1);
				UTL_RTCRead(&l_Rtc);
				fg_CurrenttUTC = UTL_GetHHUUTCTime();
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "RTC        [%02d:%02d:%02d]", l_Rtc.tm_hour, l_Rtc.tm_min, l_Rtc.tm_sec);
				HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				l_TempVal = fg_CurrenttUTC -fg_StartUTC;
				ConvertUTCtoRTC(l_TempVal, &fg_RtcTimer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "Timer      [%02d:%02d:%02d]", fg_RtcTimer.Hour, fg_RtcTimer.Minute, fg_RtcTimer.Second);
				HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				//HAL_DebugPrint(5, (_SBYTE*)"Hold Test Time");
				HAL_DebugPrint(6, (_SBYTE*)"Press 1 to Continue");
			}

			if(fg_KeyDetection == '1')
			{
				fg_KeyDetection = 0;
				fg_Reading = SECOND_READING;
			}
			else
			{
				//return if want to exit test mode
			}

			break;
		//------------------------------------------------------
		case SECOND_READING:
			HAL_DebugPrint(1, (_SBYTE*)"READING (2)....");
			l_RetVal = ViewRequest(3,1,0,1,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kwhImp = l_AP_POWER - fg_Prv_kwhImp;
			}
			l_RetVal = ViewRequest(3,1,0,1,8,0,255,3);

			if(l_RetVal)
			{
				Display_Value(2, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Imp)");
			}

			l_RetVal = ViewRequest(3,1,0,2,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kwhExp = l_AP_POWER - fg_Prv_kwhExp;
			}

			l_RetVal = ViewRequest(3,1,0,2,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(3, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Exp)");
			}

			l_RetVal = ViewRequest(3,1,0,9,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvahImp = l_AP_POWER - fg_Prv_kvahImp;
			}

			l_RetVal = ViewRequest(3,1,0,9,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(4, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Imp)");
			}

			l_RetVal = ViewRequest(3,1,0,10,8,0,255,2);
			if(l_RetVal)
			{
				l_AP_POWER = fg_RxBuffer[16];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[17];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[18];
				l_AP_POWER *= 0x100;
				l_AP_POWER +=  fg_RxBuffer[19];
				fg_Prv_kvahExp = l_AP_POWER - fg_Prv_kvahExp;
			}

			l_RetVal = ViewRequest(3,1,0,10,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(5, (_SBYTE*)"", l_AP_POWER, (_SBYTE*)"(Exp)");
			}

			l_RetVal = ViewRequest(3,1,0,5,8,0,255,2); //(Q1)
			if(l_RetVal)
			{
				l_kvarhq1 = fg_RxBuffer[16];
				l_kvarhq1 *= 0x100;
				l_kvarhq1 +=  fg_RxBuffer[17];
				l_kvarhq1 *= 0x100;
				l_kvarhq1 +=  fg_RxBuffer[18];
				l_kvarhq1 *= 0x100;
				l_kvarhq1 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq1 = l_kvarhq1 - fg_Prv_kvarhq1 ;
			}


			l_RetVal = ViewRequest(3,1,0,6,8,0,255,2);//(Q2)
			if(l_RetVal)
			{
				l_kvarhq2 = fg_RxBuffer[16];
				l_kvarhq2 *= 0x100;
				l_kvarhq2 +=  fg_RxBuffer[17];
				l_kvarhq2 *= 0x100;
				l_kvarhq2 +=  fg_RxBuffer[18];
				l_kvarhq2 *= 0x100;
				l_kvarhq2 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq2 = l_kvarhq2 - fg_Prv_kvarhq2;
			}

			l_RetVal = ViewRequest(3,1,0,7,8,0,255,2);//(Q3)
			if(l_RetVal)
			{
				l_kvarhq3 = fg_RxBuffer[16];
				l_kvarhq3 *= 0x100;
				l_kvarhq3 +=  fg_RxBuffer[17];
				l_kvarhq3 *= 0x100;
				l_kvarhq3 +=  fg_RxBuffer[18];
				l_kvarhq3 *= 0x100;
				l_kvarhq3 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq3 = l_kvarhq3 - fg_Prv_kvarhq3;
			}

			l_RetVal = ViewRequest(3,1,0,8,8,0,255,2);//(Q4)
			if(l_RetVal)
			{
				l_kvarhq4 = fg_RxBuffer[16];
				l_kvarhq4 *= 0x100;
				l_kvarhq4 +=  fg_RxBuffer[17];
				l_kvarhq4 *= 0x100;
				l_kvarhq4 +=  fg_RxBuffer[18];
				l_kvarhq4 *= 0x100;
				l_kvarhq4 +=  fg_RxBuffer[19];
				fg_Prv_kvarhq4 = l_kvarhq4 - fg_Prv_kvarhq4;
			}

			Disconnect();

			HAL_DebugPrint(7, (_SBYTE*)"press any key..");
			_getkey();
			fg_Reading = SECOND_READING_B_TYPE_NEXT_DATA;
			break;
		//------------------------------------------------------
		case SECOND_READING_B_TYPE_NEXT_DATA:
			l_RetVal = ViewRequest(3,1,0,5,8,0,255,3);
			if(l_RetVal)
			{
				fg_QuardentPresence = 1;
				Display_Value(1, (_SBYTE*)"", l_kvarhq1, (_SBYTE*)"(Q1)");
			}
			else
			{
				fg_QuardentPresence = 1;
			}

			l_RetVal = ViewRequest(3,1,0,6,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(4, (_SBYTE*)"", l_kvarhq2, (_SBYTE*)"(Q2)");
			}

			l_RetVal = ViewRequest(3,1,0,7,8,0,255,3);
			if(l_RetVal)
			{
				Display_Value(2, (_SBYTE*)"", l_kvarhq3, (_SBYTE*)"(Q3)");
			}

			l_RetVal = ViewRequest(3,1,0,8,8,0,255,3);

			if(l_RetVal)
			{
				Display_Value(4, (_SBYTE*)"", l_kvarhq4, (_SBYTE*)"(Q4)");
			}

			Disconnect();
			HAL_DebugPrint(7, (_SBYTE*)"press any key..");
			_getkey();
			fg_Reading = READING_DIFFERENCE;
			break;
		//------------------------------------------------------
		case READING_DIFFERENCE:
			LcdClear();
			Disconnect();
			HAL_DebugPrint(1, (_SBYTE*)"Difference");

			if(g_displayPoint == 3)
			{
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%.3f",(double)fg_Prv_kwhImp/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%.3f",(double)fg_Prv_kwhExp/1000);
				HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%.3f",(double)fg_Prv_kvahImp/1000);
				HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%.3f",(double)fg_Prv_kvahExp/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

				if(fg_QuardentPresence)
				{
					HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
					_getkey();
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%.3f",(double)fg_Prv_kvarhq1/1000);
					HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%.3f",(double)fg_Prv_kvarhq2/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%.3f",(double)fg_Prv_kvarhq3/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%.3f",(double)fg_Prv_kvarhq4/1000);
					HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
				}
			}
			else if(g_displayPoint == 2)
			{
				fg_Prv_kwhImp *= 10;
				fg_Prv_kwhExp *= 10;
				fg_Prv_kvahImp *= 10;
				fg_Prv_kvahExp *= 10;
				fg_Prv_kvarhq1 *= 10;
				fg_Prv_kvarhq2 *= 10;
				fg_Prv_kvarhq3 *= 10;
				fg_Prv_kvarhq4 *= 10;
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%.2f",(double)fg_Prv_kwhImp/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%.2f",(double)fg_Prv_kwhExp/1000);
				HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%.2f",(double)fg_Prv_kvahImp/1000);
				HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%.2f",(double)fg_Prv_kvahExp/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

				if(fg_QuardentPresence)
				{
					HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
					_getkey();
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%.2f",(double)fg_Prv_kvarhq1/1000);
					HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%.2f",(double)fg_Prv_kvarhq2/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%.2f",(double)fg_Prv_kvarhq3/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%.2f",(double)fg_Prv_kvarhq4/1000);
					HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
				}
			}
			else if(g_displayPoint == 1)
			{
				fg_Prv_kwhImp *= 100;
				fg_Prv_kwhExp *= 100;
				fg_Prv_kvahImp *= 100;
				fg_Prv_kvahExp *= 100;
				fg_Prv_kvarhq1 *= 100;
				fg_Prv_kvarhq2 *= 100;
				fg_Prv_kvarhq3 *= 100;
				fg_Prv_kvarhq4 *= 100;

				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%.1f",(double)fg_Prv_kwhImp/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%.1f",(double)fg_Prv_kwhExp/1000);
				HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%.1f",(double)fg_Prv_kvahImp/1000);
				HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%.1f",(double)fg_Prv_kvahExp/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

				if(fg_QuardentPresence)
				{
					HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
					_getkey();
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%.1f",(double)fg_Prv_kvarhq1/1000);
					HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%.1f",(double)fg_Prv_kvarhq2/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%.1f",(double)fg_Prv_kvarhq3/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%.1f",(double)fg_Prv_kvarhq4/1000);
					HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
				}
			}
			else if(g_displayPoint == 4)
			{
				fg_Prv_kwhImp *= 1000;
				fg_Prv_kwhExp *= 1000;
				fg_Prv_kvahImp *= 1000;
				fg_Prv_kvahExp *= 1000;
				fg_Prv_kvarhq1 *= 1000;
				fg_Prv_kvarhq2 *= 1000;
				fg_Prv_kvarhq3 *= 1000;
				fg_Prv_kvarhq4 *= 1000;

				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%g",(double)fg_Prv_kwhImp/1000);
				HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%g",(double)fg_Prv_kwhExp/1000);
				HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%g",(double)fg_Prv_kvahImp/1000);
				HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%g",(double)fg_Prv_kvahExp/1000);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

				if(fg_QuardentPresence)
				{
					HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
					_getkey();
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%g",(double)fg_Prv_kvarhq1/1000);
					HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%g",(double)fg_Prv_kvarhq2/1000);
					HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%g",(double)fg_Prv_kvarhq3/1000);
					HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%g",(double)fg_Prv_kvarhq4/1000);
					HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
				}
			}
			HAL_DebugPrint(6, (_SBYTE*) "press any key..");
			_getkey();

			l_AP_POWER = 0;
			fg_Prv_kwh = 0;
			fg_Prv_kvah = 0;
			fg_Prv_kvarhLag = 0;
			fg_Prv_kvarhLead = 0;

			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"1.TEST AGAIN");
			HAL_DebugPrint(2, (_SBYTE*)"0.BACK ");
			while((l_RetVal != '1') && (l_RetVal != '0'))
				l_RetVal = _getkey();
			if(l_RetVal == '0')
			{
				return 0;
			}
			else if(l_RetVal == '1')
			{
				fg_Reading = FIRST_READING;
			}

			break;
		//------------------------------------------------------
		default:
			fg_Reading = FIRST_READING;
			break;
	}

	return 1;
}
//============================================================================

#if 0
        LcdClear();
        while(1)
    	{
			l_result1 = 0;
			if(Read_flag != 2)
			{
				LcdClear();
				HAL_DebugPrint(1, (_SBYTE*)"METER READ...");}
				rrr_c=0;
				rrr_c1=0;
				rrr_s=0;
				sss_c=0;
				sss_c1=0;
				sss_s=0;

				l_Result = SetNormalResponseMode();
				if(l_Result == 1)
				{
					l_Result =0;
					l_Result = AssociationRequest();
					if(l_Result == 1)
					{
 TEST_AGAIN:			LcdClear();
						if(Read_flag == 0)
						{
							HAL_DebugPrint(1, (_SBYTE*)"READING (1)....");
							UTL_RTCRead(&l_Rtc);
							hh1 = l_Rtc.tm_hour;
							mm1 = l_Rtc.tm_min;
							ss1 = l_Rtc.tm_sec;
							HAL_DebugPrint(2, (_SBYTE*)"Time(HH:MM:SS)");
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "(%02d:%02d:%02d)", l_Rtc.tm_hour, l_Rtc.tm_min, l_Rtc.tm_sec);
							HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
							Read_flag = 1;

							l_result = ViewRequest(3,1,0,1,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,1,8,0,255,3);
							Prv_kwhEX = AP_POWER;
							if(l_result)
							{
								Display_Value(4, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(5, (_SBYTE*)"(Imp)");
								HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"READING (1)....");
							}
							l_result = ViewRequest(3,1,0,2,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,2,8,0,255,3);
							Prv_kwhIP = AP_POWER;
							if(l_result)
							{
								Display_Value(2, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(3, (_SBYTE*)"(Exp)");
							}

							l_result = ViewRequest(3,1,0,9,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,9,8,0,255,3);
							Prv_kvahEX = AP_POWER;
							if(l_result)
							{
								Display_Value(4, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(5, (_SBYTE*)"(Imp)");
								HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"READING (1)....");
							}

							l_result = ViewRequest(3,1,0,10,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,10,8,0,255,3);
							Prv_kvahIP = AP_POWER;
							if(l_result)
							{
								Display_Value(2, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(3, (_SBYTE*)"(Exp)");
							}
							l_result = ViewRequest(3,1,0,5,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,5,8,0,255,3);
							Prv_kvarhq1 = AP_POWER;
							if(l_result)
							{
								Display_Value(4, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(5, (_SBYTE*)"(Q1)");
								HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"READING (1)....");
							}

							l_result = ViewRequest(3,1,0,6,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,6,8,0,255,3);
							Prv_kvarhq2 = AP_POWER;
							if(l_result)
							{
								Display_Value(2, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(3, (_SBYTE*)"(Q2)");
							}

							l_result = ViewRequest(3,1,0,7,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,7,8,0,255,3);
							Prv_kvarhq3 = AP_POWER;
							if(l_result)
							{
								Display_Value(4, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(5, (_SBYTE*)"(Q3)");
								HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"READING (1)....");
							}

							l_result = ViewRequest(3,1,0,8,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,8,8,0,255,3);
							Prv_kvarhq4 = AP_POWER;
							if(l_result)
							{
								Display_Value(4, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(5, (_SBYTE*)"(Q4)");
							}

							if(l_result1 == 1)
							{
								Disconnect();
							}

							HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
							_getkey();
							LcdClear();
							UTL_RTCRead(&l_Rtc);
							Prv_Seconds =(long) (l_Rtc.tm_hour)*3600 + (l_Rtc.tm_min)*60 + l_Rtc.tm_sec;
							tt = ((l_Rtc.tm_min)*60 - (mm1)*60)+ (l_Rtc.tm_sec-ss1);

							//while(!kbhit())
							//{
							//	UTL_Wait(1000);
								tt++;
								HAL_DebugPrint(1, (_SBYTE*)"      (HH:MM:SS)");
								l_Rtc.tm_sec++;
								if(l_Rtc.tm_sec == 60)
								{
									l_Rtc.tm_sec = 0;
									l_Rtc.tm_min++;
								}
								if(l_Rtc.tm_min == 60)
								{
									l_Rtc.tm_min = 0;
									l_Rtc.tm_hour++;
								}
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "RTC   %02d:%02d:%02d", l_Rtc.tm_hour, l_Rtc.tm_min, l_Rtc.tm_sec);
								HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "Start   %02d:%02d:%02d",hh1,mm1,ss1);
								HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
								mm2 = tt/60;
								ss2 = tt%60;
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "Timer  00: %02d:%02d", mm2, ss2);
								HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
								HAL_DebugPrint(5, (_SBYTE*)"Hold Test Time");
								HAL_DebugPrint(6, (_SBYTE*)"Press 1 to Continue");
								//if(kbhit())
								//{
									while( l_Start != '1')
									l_Start =_getkey();
									if(l_Start == '1')
									{
										printf(":%02d",l_Start);
										//break;
									}
							//	}
							//}

							goto TEST_AGAIN;
							tt=0;
						}
						else if(Read_flag == 1)
						{
							rrr_c=0;
							rrr_c1=0;
							rrr_s=0;
							sss_c=0;
							sss_c1=0;
							sss_s=0;
							Disconnect();
							l_Result = SetNormalResponseMode();
							if(l_Result == 1)
							{
								l_Result =0;
								l_Result = AssociationRequest();
								if(l_Result == 1)
								{
									HAL_DebugPrint(1, (_SBYTE*)" READING (2)....");
									UTL_RTCRead(&l_Rtc);
									Read_flag = 2;
									AP_POWER1=0;
									l_result = ViewRequest(3,1,0,1,8,0,255,2);
									AP_POWER = fg_RxBuffer[16];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[17];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[18];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[19];
									l_result = ViewRequest(3,1,0,1,8,0,255,3);
									Prv_kwhEX1 = AP_POWER - Prv_kwhEX;
									if(l_result)
									{
										Display_Value(2, (_SBYTE*)"", AP_POWER);
										HAL_DebugPrint(3, (_SBYTE*)"(Imp)");
									}

									l_result = ViewRequest(3,1,0,2,8,0,255,2);
									AP_POWER = fg_RxBuffer[16];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[17];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[18];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[19];
									l_result = ViewRequest(3,1,0,2,8,0,255,3);
									Prv_kwhIP1 = AP_POWER - Prv_kwhIP;
									if(l_result)
									{
										Display_Value(4, (_SBYTE*)"", AP_POWER);
										HAL_DebugPrint(5, (_SBYTE*)"(Exp)");
										HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
										_getkey();
										LcdClear();
										HAL_DebugPrint(1, (_SBYTE*)"READING (2)....");
									}

									l_result = ViewRequest(3,1,0,9,8,0,255,2);
									AP_POWER = fg_RxBuffer[16];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[17];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[18];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[19];
									l_result = ViewRequest(3,1,0,9,8,0,255,3);
									Prv_kvahEX1 = AP_POWER -Prv_kvahEX;
									if(l_result)
									{
										Display_Value(2, (_SBYTE*)"", AP_POWER);
										HAL_DebugPrint(3, (_SBYTE*)"(Imp)");
									}

									l_result = ViewRequest(3,1,0,10,8,0,255,2);
									AP_POWER = fg_RxBuffer[16];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[17];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[18];
									AP_POWER *= 0x100;
									AP_POWER +=  fg_RxBuffer[19];
									l_result = ViewRequest(3,1,0,10,8,0,255,3);
									Prv_kvahIP1 = AP_POWER - Prv_kvahIP;
									if(l_result)
									{
										Display_Value(4, (_SBYTE*)"", AP_POWER);
										HAL_DebugPrint(5, (_SBYTE*)"(Exp)");
										HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
										_getkey();
										LcdClear();
										HAL_DebugPrint(1, (_SBYTE*)"READING (2)....");
									}
								}
							}
							l_result = ViewRequest(3,1,0,5,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,5,8,0,255,3);
							Prv_kvarhq1_1 = AP_POWER-Prv_kvarhq1 ;
							if(l_result)
							{
								l_QuardentPresence=1;
								Display_Value(2, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(3, (_SBYTE*)"(Q1)");
							}

							l_result = ViewRequest(3,1,0,6,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,6,8,0,255,3);
							Prv_kvarhq2_2 = AP_POWER-Prv_kvarhq2;
							if(l_result)
							{
								Display_Value(4, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(5, (_SBYTE*)"(Q2)");
								HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"READING (2)....");
							}
							l_result = ViewRequest(3,1,0,7,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,7,8,0,255,3);
							Prv_kvarhq3_3 = AP_POWER-Prv_kvarhq3;
							if(l_result)
							{
								Display_Value(2, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(3, (_SBYTE*)"(Q3)");
							}

							l_result = ViewRequest(3,1,0,8,8,0,255,2);
							AP_POWER = fg_RxBuffer[16];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[17];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[18];
							AP_POWER *= 0x100;
							AP_POWER +=  fg_RxBuffer[19];
							l_result = ViewRequest(3,1,0,8,8,0,255,3);
							Prv_kvarhq4_4 = AP_POWER-Prv_kvarhq4;
							if(l_result)
							{
								Display_Value(4, (_SBYTE*)"", AP_POWER);
								HAL_DebugPrint(5, (_SBYTE*)"(Q4)");
							}
							HAL_DebugPrint(6, (_SBYTE*)"Press any key..");
							_getkey();
							goto pppp;

							if(l_result1 == 1)
							{
								Disconnect();
							}
							UTL_RTCRead(&l_Rtc);
							l_Seconds = (long) (l_Rtc.tm_hour)*3600 + (l_Rtc.tm_min)*60 + l_Rtc.tm_sec;
						}
						else if(Read_flag == 2)
						{
 pppp:	 	 	 	 	 	LcdClear();
							HAL_DebugPrint(1, (_SBYTE*)"DIFFERENCE..");
							l_Seconds -= Prv_Seconds;
							Disconnect();
							Read_flag = 3;
							if(g_displayPoint == 3)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%.3f",(double)Prv_kwhEX1/1000);
								HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%.3f",(double)Prv_kwhIP1/1000);
								HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%.3f",(double)Prv_kvahEX1/1000);
								HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%.3f",(double)Prv_kvahIP1/1000);
								HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

								if(l_QuardentPresence)
								{
									HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
									_getkey();
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%.3f",(double)Prv_kvarhq1_1/1000);
									HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%.3f",(double)Prv_kvarhq2_2/1000);
									HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%.3f",(double)Prv_kvarhq3_3/1000);
									HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%.3f",(double)Prv_kvarhq4_4/1000);
									HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
								}
								HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"DIFFERENCE..");
							}
							else if(g_displayPoint == 2)
							{
								Prv_kwhEX1 *= 10;
								Prv_kwhIP1 *= 10;
								Prv_kvahEX1 *= 10;
								Prv_kvahIP1 *= 10;
								Prv_kvarhq1_1 *= 10;
								Prv_kvarhq2_2 *= 10;
								Prv_kvarhq3_3 *= 10;
								Prv_kvarhq4_4 *= 10;

								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%.2f",(double)Prv_kwhEX1/1000);
								HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%.2f",(double)Prv_kwhIP1/1000);
								HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%.2f",(double)Prv_kvahEX1/1000);
								HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%.2f",(double)Prv_kvahIP1/1000);
								HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

								if(l_QuardentPresence)
								{
									HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
									_getkey();
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%.2f",(double)Prv_kvarhq1_1/1000);
									HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%.2f",(double)Prv_kvarhq2_2/1000);
									HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%.2f",(double)Prv_kvarhq3_3/1000);
									HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%.2f",(double)Prv_kvarhq4_4/1000);
									HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
								}
								HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"DIFFERENCE..");
							}
							else if(g_displayPoint == 1)
							{
								Prv_kwhEX1 *= 100;
								Prv_kwhIP1 *= 100;
								Prv_kvahEX1 *= 100;
								Prv_kvahIP1 *= 100;
								Prv_kvarhq1_1 *= 100;
								Prv_kvarhq2_2 *= 100;
								Prv_kvarhq3_3 *= 100;
								Prv_kvarhq4_4 *= 100;

								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%.1f",(double)Prv_kwhEX1/1000);
								HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%.1f",(double)Prv_kwhIP1/1000);
								HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%.1f",(double)Prv_kvahEX1/1000);
								HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%.1f",(double)Prv_kvahIP1/1000);
								HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

								if(l_QuardentPresence)
								{
									HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
									_getkey();
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%.1f",(double)Prv_kvarhq1_1/1000);
									HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%.1f",(double)Prv_kvarhq2_2/1000);
									HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%.1f",(double)Prv_kvarhq3_3/1000);
									HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%.1f",(double)Prv_kvarhq4_4/1000);
									HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
								}
								HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"DIFFERENCE..");
							}
							else if(g_displayPoint == 4)
							{
								Prv_kwhEX1 *= 1000;
								Prv_kwhIP1 *= 1000;
								Prv_kvahEX1 *= 1000;
								Prv_kvahIP1 *= 1000;
								Prv_kvarhq1_1 *= 1000;
								Prv_kvarhq2_2 *= 1000;
								Prv_kvarhq3_3 *= 1000;
								Prv_kvarhq4_4 *= 1000;

								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhIMP=%g",(double)Prv_kwhEX1/1000);
								HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kWhEXP=%g",(double)Prv_kwhIP1/1000);
								HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhIMP=%g",(double)Prv_kvahEX1/1000);
								HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kVAhEXP=%g",(double)Prv_kvahIP1/1000);
								HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

								if(l_QuardentPresence)
								{
									HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
									_getkey();
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q1)=%g",(double)Prv_kvarhq1_1/1000);
									HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q2)=%g",(double)Prv_kvarhq2_2/1000);
									HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q3)=%g",(double)Prv_kvarhq3_3/1000);
									HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "kvarh(Q4)=%g",(double)Prv_kvarhq4_4/1000);
									HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
								}
								HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
							}
						}
						l_option =0;
						if(Read_flag == 3)
						{
							Read_flag = 0;AP_POWER1=0;
							Prv_kwh = Prv_kvah =Prv_kvarh = 0;
							LcdClear();
							HAL_DebugPrint(1, (_SBYTE*)"1.TEST AGAIN");
							HAL_DebugPrint(2, (_SBYTE*)"0.BACK ");
							while((l_option != '1') && (l_option != '0'))
							l_option = _getkey();
							if(l_option == '0')
							break;
							else if(l_option == '1')
							{
								tt=0;
								goto TSTMD;
							}
						}
						else
						{
							//printf("\nPress any Key.");
							_getkey();
						}
					}
					else
					{
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL.");
						HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
						HAL_DebugPrint(3, (_SBYTE*)"PRESS 0 FOR RETURN");
						Disconnect();
						while(l_option != '0')
						l_option = _getkey();
						if(l_option == '0')
						break;
					}
				}
				else
                {
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL.");
					HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
					HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
					HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 FOR RETURN");
					Disconnect();
					while(l_option != '0')
					l_option = _getkey();
					if(l_option == '0')
					break;
                }
    	}
	}
    else if(type!= 2)
    {
    	while(1)
    	{
 TEST_AGAIN1:LcdClear();
 	 		l_result1 = 0;
			rrr_c=0;
			rrr_c1=0;
			rrr_s=0;
			sss_c=0;
			sss_c1=0;
			sss_s=0;
			Disconnect();
			l_Result = SetNormalResponseMode();
			if(l_Result == 1)
			{
				l_Result =0;
				l_Result = AssociationRequest();
				LcdClear();
				if(l_Result == 1)
				{
					if(Read_flag ==0)
					{
						HAL_DebugPrint(1, (_SBYTE*)"READING(1) [HH:MM:SS]");
						UTL_RTCRead(&l_Rtc);
						hh1 = l_Rtc.tm_hour;
						mm1 = l_Rtc.tm_min;
						ss1 = l_Rtc.tm_sec;
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "       Time[%02d:%02d:%02d]", hh1, mm1, ss1);
						HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
						Read_flag = 1;

						l_result1 = ViewRequest(3,1,0,1,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,1,8,0,255,3);
						Prv_kwh = AP_POWER1;
						if(l_result1)
						{
							Display_Value(3, (_SBYTE*)"", AP_POWER1);
						}

						l_result1 = ViewRequest(3,1,0,5,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,5,8,0,255,3);
						Prv_kvarh = AP_POWER1;
						if(l_result1)
						{
							l_Lag = 1;
							Display_Value(4, (_SBYTE*)"Lg=", AP_POWER1);
							//HAL_DebugPrint(6, (_SBYTE*)"(Lag)");
							//HAL_DebugPrint(7, (_SBYTE*)"press any key..");
							//_getkey();
							//LcdClear();
							//HAL_DebugPrint(1, (_SBYTE*)"READING (1)....");
						}

						l_result1 = ViewRequest(3,1,0,8,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,8,8,0,255,3);
						Prv_kvarh1 = AP_POWER1;
						if(l_result1)
						{
//							struct time t1;	///mundel
//							gettime(&t1);
							l_Lead =1;
							Display_Value(5, (_SBYTE*)"Ld=", AP_POWER1);
							//HAL_DebugPrint(3, (_SBYTE*)"(Lead)");
						}

						l_result1 = ViewRequest(3,1,0,9,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,9,8,0,255,3);
						Prv_kvah = AP_POWER1;
						if(l_result1 == 1)
						{
							Disconnect();
							Display_Value(6, (_SBYTE*)"", AP_POWER1);
						}
						HAL_DebugPrint(7, (_SBYTE*)"press any key..");
						_getkey();
						LcdClear();
						UTL_RTCRead(&l_Rtc);
						Prv_Seconds =(long) (l_Rtc.tm_hour)*3600 + (l_Rtc.tm_min)*60 + l_Rtc.tm_sec;
						tt=((l_Rtc.tm_min)*60 - ((mm1)*60 ))+ (l_Rtc.tm_sec - ss1);
						//while(1)
						//{
							UTL_Wait(1000);
							tt++;
							HAL_DebugPrint(1, (_SBYTE*)"      (HH:MM:SS)");
							l_Rtc.tm_sec++;
							if(l_Rtc.tm_sec == 60)
							{
								l_Rtc.tm_sec = 0;
								l_Rtc.tm_min++;
							}
							if(l_Rtc.tm_min == 60)
							{
								l_Rtc.tm_hour++;
								l_Rtc.tm_min = 0;
							}
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "      Time[%02d:%02d:%02d]", l_Rtc.tm_hour, l_Rtc.tm_min, l_Rtc.tm_sec);
							HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "Start Time[%02d:%02d:%02d]", hh1, mm1, ss1);
							HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
							mm2 = (tt/60);
							ss2 = (tt%60);
							HAL_DebugPrint(4, (_SBYTE*)"Timer  00:");
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d", mm2, ss2);
							HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
							HAL_DebugPrint(6, (_SBYTE*)"   (S).Stop");
							HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
							HAL_DebugPrint(5, (_SBYTE*)"Hold Test Time");
							HAL_DebugPrint(6, (_SBYTE*)"Press 1 to Continue");
							//if(kbhit())
							//{
								while( l_Start != '1')
								l_Start =_getkey();
								if(l_Start == '1')
								{
									printf(":%02d",l_Start);
									//break;
								}
						//	}
                        //}
						goto TEST_AGAIN1;
                        tt=0;
					}
					else if(Read_flag == 1)
					{

						HAL_DebugPrint(1, (_SBYTE*)"READING (2)....");
						Read_flag = 2;
						AP_POWER1=0;
						l_result1 = ViewRequest(3,1,0,1,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,1,8,0,255,3);
						Prv_kwh = AP_POWER1 - Prv_kwh;
						if(l_result1)
						{
							Display_Value(2, (_SBYTE*)"", AP_POWER1);
						}

						l_result1 = ViewRequest(3,1,0,5,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,5,8,0,255,3);
						Prv_kvarh = AP_POWER1 - Prv_kvarh;
						if(l_result1)
						{
							Display_Value(3, (_SBYTE*)"", AP_POWER1);
							HAL_DebugPrint(4, (_SBYTE*)"(Lag)");
						}

						l_result1 = ViewRequest(3,1,0,8,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,8,8,0,255,3);
						Prv_kvarh1 = AP_POWER1- Prv_kvarh1;
						if(l_result1)
						{
							Display_Value(5, (_SBYTE*)"", AP_POWER1);
							HAL_DebugPrint(6, (_SBYTE*)"(Lead)");
							HAL_DebugPrint(7, (_SBYTE*)"press any key..");
							_getkey();
							LcdClear();
							HAL_DebugPrint(1, (_SBYTE*)"READING (2)....");
						}

						l_result1 = ViewRequest(3,1,0,9,8,0,255,2);
						AP_POWER1 = fg_RxBuffer[16];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[17];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[18];
						AP_POWER1 *= 0x100;
						AP_POWER1 +=  fg_RxBuffer[19];
						l_result1 = ViewRequest(3,1,0,9,8,0,255,3);
						Prv_kvah = AP_POWER1 - Prv_kvah;
						if(l_result1)
						{
							Disconnect();
							Display_Value(5, (_SBYTE*)"", AP_POWER1);
						}
						HAL_DebugPrint(7, (_SBYTE*)"press any key..");
						_getkey();
						UTL_RTCRead(&l_Rtc);
						l_Seconds = (long) (l_Rtc.tm_hour)*3600 + (l_Rtc.tm_min)*60 + l_Rtc.tm_sec;
					}
					else if(Read_flag == 2)
					{
						LcdClear();
						l_Seconds -= Prv_Seconds;
						Disconnect();
						Read_flag = 3;
						l_Line = 2;
						HAL_DebugPrint(1, (_SBYTE*)"Difference");
						if(g_displayPoint==3)
						{
							l_Line = 2;
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kWh = %.3f",(double)Prv_kwh/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);

							if(l_Lag)
							{
								l_Line++;
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.3f",(double)Prv_kvarh/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							}

							if(l_Lead)
							{
								l_Line++;
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.3f",(double)Prv_kvarh1/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							}
							l_Line++;
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.3f",(double)Prv_kvah/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							HAL_DebugPrint(l_Line, (_SBYTE*) "press any key..");
							_getkey();
							LcdClear();
						}
						else if(g_displayPoint==2)
						{
							Prv_kwh *= 10;
							Prv_kvarh *= 10;
							Prv_kvarh1 *= 10;
							Prv_kvah *= 10;
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kWh=%.2f",(double)Prv_kwh/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;

							if(l_Lag)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.2f",(double)Prv_kvarh/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							if(l_Lead)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.2f",(double)Prv_kvarh1/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.2f",(double)Prv_kvah/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							HAL_DebugPrint(l_Line, (_SBYTE*)"press any key..");
							_getkey();
							LcdClear();
						}
						else if(g_displayPoint==1)
						{
							Prv_kwh *= 100;
							Prv_kvarh *= 100;
							Prv_kvarh1 *= 100;
							Prv_kvah *= 100;
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kWh=%.1f",(double)Prv_kwh/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							if(l_Lag)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.1f",(double)Prv_kvarh/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							if(l_Lead)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.1f\n",(double)Prv_kvarh1/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.1f",(double)Prv_kvah/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							HAL_DebugPrint(l_Line, (_SBYTE*)"press any key..");
							_getkey();
							LcdClear();
						}
						else if(g_displayPoint == 4)//means meter sends 0 decimal
						{
							Prv_kwh *= 1000;
							Prv_kvarh *= 1000;
							Prv_kvarh1 *= 1000;
							Prv_kvah *= 1000;
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kWh=%g",(double)Prv_kwh/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							if(l_Lag)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%g",(double)Prv_kvarh/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							if(l_Lead)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%g",(double)Prv_kvarh1/1000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kVAh=%g\n",(double)Prv_kvah/1000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							HAL_DebugPrint(l_Line, (_SBYTE*)"press any key..");
							_getkey();
							LcdClear();
						}
						else if(g_displayPoint == 6)//means meter sends 0 decimal
						{
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kWh=%.6f",(double)Prv_kwh/1000000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							if(l_Lag)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.6f",(double)Prv_kvarh/1000000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							if(l_Lead)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.6f",(double)Prv_kvarh1/1000000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}

							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.6f",(double)Prv_kvah/1000000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							HAL_DebugPrint(l_Line, (_SBYTE*)"press any key..");
							_getkey();
							LcdClear();
						}
						else if(g_displayPoint == 5)//means meter sends 0 decimal
						{
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kWh=%.6f",(double)Prv_kwh/100000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							if(l_Lag)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Lg=%.6f",(double)Prv_kvarh/100000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}
							if(l_Lead)
							{
								memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
								sprintf((_SBYTE*)g_DispBuffer, "kvarh-Ld=%.6f",(double)Prv_kvarh1/100000);
								HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
								l_Line++;
							}

							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "kVAh=%.6f",(double)Prv_kvah/100000);
							HAL_DebugPrint(l_Line, (_SBYTE*) g_DispBuffer);
							l_Line++;
							HAL_DebugPrint(l_Line, (_SBYTE*)"press any key..");
							_getkey();
							LcdClear();
						}
					}
					l_option =0;
					if(Read_flag == 3)
					{
						Read_flag = 0;AP_POWER1=0;
						Prv_kwh = Prv_kvah =Prv_kvarh = 0;
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"1.TEST AGAIN");
						HAL_DebugPrint(2, (_SBYTE*)"0.BACK ");
						while((l_option != '1') && (l_option != '0'))
						l_option = _getkey();
						if(l_option == '0')
						break;
						else if(l_option == '1')
						{
							tt=0;
							goto TSTMD;//again test mode starts
						}
					}
					else
					{
						//printf("\nPress any Key.");
						_getkey();
					}
				}
				else
                {
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL.");
					HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
					HAL_DebugPrint(3, (_SBYTE*)"PRESS 0 FOR RETURN");
					Disconnect();
					while(l_option != '0')
					l_option = _getkey();
					if(l_option == '0')
					break;
                }
			}
        	else
        	{
                LcdClear();
                HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL.");
				HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
				HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
				HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 FOR RETURN");
				Disconnect();
				while(l_option != '0')
                l_option = _getkey();
				if(l_option == '0')
				break;
        	}
    	}
    }
}
#endif


//================================================================
void * RunKeyDetection(void *threadid)
{
	fprintf(stderr, "\n Key detection threads Running");

	while(fg_KeyDetection != '1')
	{
		fg_KeyDetection = _getkey();
	}

	fprintf(stderr, "\n Key detection threads exit456");
	//pthread_exit(threadid);
}
//==============================================================================
void CreateKeyDetectionThread(void)
{
	pthread_t threads[1];
	int l_rc;

	fprintf(stderr, "\nCreate Key detection threads");

	l_rc = pthread_create(&threads[0], NULL, RunKeyDetection, (void *) 0);

	if(l_rc)
	{
		fprintf(stderr, "\n unable to Create Key detection  threads");
		exit(-1);
	}

	fprintf(stderr, "\n Key detection  threads successfully created");
	//pthread_exit(NULL);
}
//==============================================================================
