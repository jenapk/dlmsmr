/*
 * dlmsfunc.cpp
 *
 *  Created on: 02-Sep-2015
 *      Author: mundel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/rtc.h>
#include <dirent.h>

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

#define isleapyear(year) ((!(year % 4) && (year % 100)) || (!(year % 400)))
#define PPPINITFCS16 			0xFFFF 		/*initial FCS value*/
#define PPPGOODFCS16 			0xF0B8 		/*Good final FCS value*/

FILE * fp;
FILE * stream_r;
FILE * fp1;


unsigned char cmrisno[14];
static unsigned char fg_MtrSlnoInString;
static unsigned char MtrSlNo[10];
static unsigned char MtrSlNoNew[10];
static unsigned char fg_LLSSecret[17];
static unsigned char fg_ctratio = 0;

//========================================================
int  isdatevalid(int a_Month, int a_Day, int a_Year)
{
	if( !( (a_Year >= 2000) && (a_Year <= 2020) ) )
	{
		return 0;
	}

	if (a_Day <= 0)
	{
		return 0 ;
	}

	switch (a_Month )
	{
		case 1  :
		case 3  :
		case 5  :
		case 7  :
		case 8  :
		case 10 :
		case 12 :
			if (a_Day > 31)
			{
				return 0;
			}
			else
			{
				return 1 ;
			}
		case 4  :
		case 6  :
		case 9  :
		case 11 :
			if (a_Day > 30)
			{
				return 0 ;
			}
			else
			{
				return 1 ;
			}
		case 2  :
			if ( a_Day > 29 )
			{
				return 0 ;
			}

			if ( a_Day < 29 )
			{
				return 1 ;
			}

			if (isleapyear(a_Year))
			{
				return 1 ;   // leap year
			}
			else
			{
				return 0 ;
			}
	}
	return 0 ;
}
int SANDS (void)
{
	return 0;
}
//=============================================================
void CommError(void)
{
	HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION ERROR");
	HAL_DebugPrint(2, (_SBYTE*)"TRY AGAIN");
	HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
	//fclose(fp);
	remove(RAW_FILE_NAME);
	_getkey();
}





int ReadPrepaymentData(void)
{
    char result =0;
    result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x00, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
    result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x01, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
    result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x02, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
    result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x03, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
    result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x04, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
    result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x05, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
    return 1;
}

int ReadInstantData(void)
{
	unsigned char l_loop=0,l_loop1=0;
	int result = 0;
    struct rtc_time l_Rtc;;
    time_t t;    struct tm* tm;
    char Time[15];
    _UBYTE l_MeterSlNo[20];

	memset(FileName, '0', sizeof(FileName) );
	memset(l_MeterSlNo, 0, sizeof(l_MeterSlNo) );

	result = GetRequest(0x01, 0x00, 0x00, 0x60, 0x01, 0x00, 0xff, 0x02);//serial number command

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}
	if(( fg_RxBuffer[15]==0x06 ) || ( fg_RxBuffer[15]==0x05 ))
	{
		Serialno = (long)((long)fg_RxBuffer[16]*65536*256 + (long)fg_RxBuffer[17]*65536 + (long)fg_RxBuffer[18]*256 + (long)fg_RxBuffer[19]);
		sprintf((_SBYTE*)&FileName[6], "%08lu", Serialno);
	}

	else if(( fg_RxBuffer[15] == 0x09 ) || ( fg_RxBuffer[15] == 0x0A ) || ( fg_RxBuffer[15] == 0x08 ))
	{
		for(l_loop = 0; l_loop < fg_RxBuffer[16]; l_loop++)
		{
			if((fg_RxBuffer[17+l_loop] == 0x20) || ((fg_RxBuffer[17+l_loop] >= 0x30 ) && (fg_RxBuffer[17+l_loop] <= 0x39)))
			{
				if(fg_RxBuffer[17+l_loop] != 0x20)
				{
					l_MeterSlNo[l_loop1] = fg_RxBuffer[17+l_loop];
					l_loop1++;
				}
			}
		}
		sprintf((_SBYTE*) &FileName[6], "%08s", l_MeterSlNo);
	}

	FileName[0] = '/';
	FileName[1] = 'D';
	FileName[2] = 'L';
	FileName[3] = 'M';
	FileName[4] = 'S';
	FileName[5] = '/';
	FileName[14] = '.';
	FileName[15] = 'd';
	FileName[16] = 'd';
	FileName[17] = 't';
	FileName[18] = '\0';

	result = GetRequest(0x07, 0x00, 0x00, 0x5E, 0x5B, 0x0A, 0xff, 0x03);//Name plate profile wise
	if(result == 0)
	{
		CommError();
		return 0;
	}
	if(result != 4)
	{
		result=0;
		result = GetRequest(0x07, 0x00, 0x00, 0x5E, 0x5B, 0x0A, 0xff, 0x02);//Name plate profile wise
		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		UTL_RTCRead (&l_Rtc);		//Read RTC of HHU
		fp =  fopen(RAW_FILE_NAME, "a");
		if(fp == NULL) fprintf(stderr, "\nfile open error append mode1\n");

		fprintf(fp, "\nFFFFFFFFFFFF 01 "); //20/10/2012 1:42:11 PM
		fprintf(fp,"%02d/%02d/%02d ",(l_Rtc.tm_mday),(l_Rtc.tm_mon+1),(l_Rtc.tm_year+1900));
		time(&t);
		tm = localtime(&t);
		memset(Time,0,sizeof(Time));
		strftime(Time, sizeof Time, "%I:%M:%S %p", tm);
		fprintf(fp,"%s", Time);
		///SANDS();///mundel
		APP_GetHHUSlNo(&cmrisno[0]);
		cmrisno[8] = NULL;
		fprintf(fp, "\nFFFFFFFFFFF1 02 09%02x AnalogicsHHU %s",strlen((const char*)cmrisno)+14, cmrisno);
	    fclose(fp);
		result = 0;

	    result = GetRequest(0x16, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xff, 0x04);//0x16, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xff, 0x04//auto bill bate22,0,0,15,0,0,255,4

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	}
	else
	{
		result=0;
		result = GetRequest(0x01, 0x00, 0x00, 0x60, 0x01, 0x01, 0xff, 0x02);//Manufacturing month

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x02, 0x00, 0xff, 0x02);//Firmware

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		//FileName[5] = fg_RxBuffer[17];


		result = GetRequest(0x01, 0x00, 0x00, 0x5E, 0x5B, 0x09, 0xff, 0x02);//Meter Type

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	    result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x08, 0x00, 0xff, 0x02);//kuldeep  DIP
		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
		result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x08, 0x04, 0xff, 0x02);//kuldeep  + punnet 21 01 15 LPIP
		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	    fg_ctratio =1;
		result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x04, 0x02, 0xff, 0x02);//1.0.0.4.2.255   CT ratio

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

	    // printf("\nCT aj gay\n");_getkey();

		result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x04, 0x03, 0xff, 0x02);//PT RATIO

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		result = GetRequest(0x01, 0x00, 0x00, 0x60, 0x01, 0x04, 0xff, 0x02);//Meter Year Manufature
		fprintf(stderr,"test point 123..\n");
		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			UTL_RTCRead(&l_Rtc);
			fp =  fopen(RAW_FILE_NAME, "a");
			if(fp == NULL) fprintf(stderr, "\nfile open error append mode2\n");
	        fprintf(fp, "\nFFFFFFFFFFFF 01 "); //20/10/2012 1:42:11 PM
	        fprintf(fp,"%02d/%02d/%02d ",(l_Rtc.tm_mday),(l_Rtc.tm_mon+1),(l_Rtc.tm_year+1900));
	        time(&t);
	        tm = localtime(&t);
	        memset(Time,0,sizeof(Time));
	        strftime(Time, sizeof Time, "%I:%M:%S %p", tm);
	        fprintf(fp,"%s", Time);
	        ///SANDS(); ///mundel
	        APP_GetHHUSlNo(&cmrisno[0]);
	        cmrisno[8] = NULL;
	        fprintf(fp, "\nFFFFFFFFFFF1 02 09%02x AnalogicsHHU %s",strlen((const char*)cmrisno)+14,cmrisno);
	        fclose(fp);
	        result = 0;
	        fprintf(stderr,"test point 123458\n");
		}

	    result = GetRequest(0x16, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xff, 0x04);//0x16, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xff, 0x04//auto bill bate22,0,0,15,0,0,255,4

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	    ////////For phasor  obis for angle and quadrant

	    result = GetRequest(0x03, 0x01, 0x00, 0x51, 0x07, 0x01, 0xff, 0x03);//

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	    result = GetRequest(0x03, 0x01, 0x00, 0x51, 0x07, 0x01, 0xff, 0x02);//

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	    result = GetRequest(0x03, 0x01, 0x00, 0x51, 0x07, 0x02, 0xff, 0x03);//0x16, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xff, 0x04//auto bill bate22,0,0,15,0,0,255,4

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	    result = GetRequest(0x03, 0x01, 0x00, 0x51, 0x07, 0x02, 0xff, 0x02);//0x16, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xff, 0x04//auto bill bate22,0,0,15,0,0,255,4

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	    result = GetRequest(0x01, 0x01, 0x00, 0x60, 0x07, 0x82, 0xff, 0x02);//0x16, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xff, 0x04//auto bill bate22,0,0,15,0,0,255,4

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x03, 0x00, 0xff, 0x02);//Meter Constant

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
		result = GetRequest(0x01, 0x00, 0x00, 0x5E, 0x5B, 0x0B, 0xff, 0x02);//Meter CAtegory

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
		result = GetRequest(0x01, 0x00, 0x00, 0x5E, 0x5B, 0x0C, 0xff, 0x02);//Meter Constant

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}
	      ////////For phasor  obis for angle and quadrant
	}
			
		result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x05, 0x81, 0xff, 0x02);//Bill type 160915							
		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}	
	
		result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x05, 0x80, 0xff, 0x02);//fifo 160915					
		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}	
		
		result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x03, 0xff, 0x03);

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x03, 0xff, 0x02);

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x00, 0xff, 0x03);

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

		result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x00, 0xff, 0x02);

		if(result == 0)
		{
			CommError();
			return 0;
		}
		else
		{
			result = 0;
		}

    ReadHplSpecific(); // for reading HPL specific data

	return 1;
}
//==============================================================
_UINT pppfcs16(_UINT fcs, _UBYTE *cp, int length)
{
	while(length--)
	{
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
	}

	return fcs;
}

int fcs(_UBYTE *cp, int length, int flag)
{
	_UINT trialfcs;

	if(flag == 1)
	{
		trialfcs = pppfcs16(PPPINITFCS16, cp, length);
		trialfcs ^= 0xffff;
		cp[length] = (trialfcs & 0x00ff);
		cp[length+1] = ((trialfcs >> 8) & 0x00ff);
		return 1;
	}
	else
	{
		trialfcs = pppfcs16(PPPINITFCS16, cp, length+2);

		if(trialfcs == PPPGOODFCS16)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}



int SetNormalResponseMode (void)
{
	ClearPacket();

	fg_SendBuffer[0] = 0x7e;
	fg_SendBuffer[1] = 0xa0;
	fg_SendBuffer[2] = 0x07;
	fg_SendBuffer[3] = 0x03;
	fg_SendBuffer[4] = g_UserAssociation;
	fg_SendBuffer[5] = 0x93;
	fg_SendBuffer[6] = 0x00;
	fg_SendBuffer[7] = 0x00;
	fg_SendBuffer[8] = 0x7e;
	fcs(fg_SendBuffer+1, 5, 1);
	SendPkt(9);
	Read(fg_RxBuffer);
	if(fg_RxBuffer[5] == 0x73)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int AssociationRequest(void)
{
	unsigned int c = 0;
	ClearPacket();
	fg_SendBuffer[c++] = 0x7E;
	fg_SendBuffer[c++] = 0xA0;	//Frame type
	fg_SendBuffer[c++] = 0x00;	//Frame Length
	fg_SendBuffer[c++] = 0x03;	//Destination Address
	fg_SendBuffer[c++] = g_UserAssociation;	//Source Address
	fg_SendBuffer[c++] = 0x10;	//Frame Type : I frame
	fg_SendBuffer[c++] = 0x00;	//FCS
	fg_SendBuffer[c++] = 0x00;	//FCS
	fg_SendBuffer[c++] = 0xE6;	//LLC Bytes
	fg_SendBuffer[c++] = 0xE6;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x60; 	//AssociationRequest
	fg_SendBuffer[c++] = 0x11;	//Assign Below
	fg_SendBuffer[c++] = 0xA1;  //Application Context Name Tag
	fg_SendBuffer[c++] = 0x09;
	fg_SendBuffer[c++] = 0x06;
	fg_SendBuffer[c++] = 0x07;
	fg_SendBuffer[c++] = 0x60;
	fg_SendBuffer[c++] = 0x85;
	fg_SendBuffer[c++] = 0x74;
	fg_SendBuffer[c++] = 0x05;
	fg_SendBuffer[c++] = 0x08;
	fg_SendBuffer[c++] = 0x01;
	fg_SendBuffer[c++] = 0x01;

	if(g_UserAssociation != PC_ASSOCIATION)
	{
		fg_SendBuffer[c++] = 0x8A;
		fg_SendBuffer[c++] = 0x02;
		fg_SendBuffer[c++] = 0x07;
		fg_SendBuffer[c++] = 0x80;
		fg_SendBuffer[c++] = 0x8B;
		fg_SendBuffer[c++] = 0x07;
		fg_SendBuffer[c++] = 0x60;
		fg_SendBuffer[c++] = 0x85;
		fg_SendBuffer[c++] = 0x74;
		fg_SendBuffer[c++] = 0x05;
		fg_SendBuffer[c++] = 0x08;
		fg_SendBuffer[c++] = 0x02;
		if(g_UserAssociation == US_ASSOCIATION)
		{
			fg_SendBuffer[c++] = 0x02;
			fg_SendBuffer[c++] = 0xAC;
			fg_SendBuffer[c++] = strlen((const char*)CMRI_Challenge)+2;
			fg_SendBuffer[c++] = 0x80;
			fg_SendBuffer[c++] = strlen((const char*)CMRI_Challenge);
			for (int l_Count = 0; l_Count < (strlen((const char*)CMRI_Challenge)); l_Count++)
			fg_SendBuffer[c++] = CMRI_Challenge[l_Count];
		}
		else
		{
			fg_SendBuffer[c++] = 0x01;
			fg_SendBuffer[c++] = 0xAC;
			fg_SendBuffer[c++] = strlen((const char*)fg_LLSSecret) + 2;
			fg_SendBuffer[c++] = 0x80;
			fg_SendBuffer[c++] = strlen((const char*)fg_LLSSecret);

			for (int l_Count = 0; l_Count < (strlen((const char*)fg_LLSSecret)); l_Count++)		//Copying LSS Secret
			{
				fg_SendBuffer[c++] = fg_LLSSecret[l_Count];
			}
		}
	}
	fg_SendBuffer[c++] = 0xBE;
	fg_SendBuffer[c++] = 0x10;
	fg_SendBuffer[c++] = 0x04;
	fg_SendBuffer[c++] = 0x0E;
	fg_SendBuffer[c++] = 0x01;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x06;
	fg_SendBuffer[c++] = 0x5F;
	fg_SendBuffer[c++] = 0x1F;
	fg_SendBuffer[c++] = 0x04;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x18;
	fg_SendBuffer[c++] = 0x1D;
	fg_SendBuffer[c++] = 0xFF;
	fg_SendBuffer[c++] = 0xFF;

	fg_SendBuffer[2] = c + 1;
	fg_SendBuffer[12] = c - 13;
	fg_SendBuffer[c+2] = 0x7E;

	fcs(fg_SendBuffer+1, 5, 1);
	fcs(fg_SendBuffer+1,c-1, 1);

	SendPkt(c+3);
	Read(fg_RxBuffer);
	if(g_UserAssociation == US_ASSOCIATION)
	{
		for(int l_Count = 0;l_Count < 16; l_Count++)
	    MeterChallenge[l_Count]=fg_RxBuffer[53+l_Count];
	}
	if(fg_RxBuffer[28] == 0x00)
	{
		FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
		return 1;
	}
	else
	{
		return 0;
	}
}

//===============================================================
//Disconnect from Meter
int Disconnect (void)
{
	ClearPacket();
	fg_SendBuffer[0] = 0x7e;
	fg_SendBuffer[1] = 0xa0;
	fg_SendBuffer[2] = 0x07;
	fg_SendBuffer[3] = 0x03;
	fg_SendBuffer[4] = g_UserAssociation;
	fg_SendBuffer[5] = 0x53;
	fg_SendBuffer[6] = 0x00;
	fg_SendBuffer[7] = 0x00;
	fg_SendBuffer[8] = 0x7e;
	fcs(fg_SendBuffer+1, 5, 1);
	SendPkt(9);
	Read(fg_RxBuffer);
// puts("GET");_getkey();
	if(fg_RxBuffer[5] == 0x73)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//===============================================================
//Requesting to the Meter with OBIS Code
int GetRequest(_UBYTE classID,_UBYTE Obis_a,_UBYTE Obis_b,_UBYTE Obis_c,_UBYTE Obis_d,_UBYTE Obis_e,_UBYTE Obis_f,_UBYTE Attr)
{
	_ULONG datablockno = 0,ctratio=0;
	_UBYTE moreblock = 0, ret_lsh=0;
	int counter = 0,l_Bytes = 0;
	_UBYTE l_RetVal = 0x00;

	ClearPacket();

	if(PGbar % 20 == 0)
	{
		HAL_DebugPrint(1, (_SBYTE*) "DATA DOWNLOADING....");
		HAL_DebugPrint(2, (_SBYTE*) "VOLUME:");
		HAL_DebugPrint(3, (_SBYTE*) "DON'T PRESS ANY KEY");
		HAL_DebugPrint(4, (_SBYTE*) "DURING DOWNLOADING");
		PGbar = 0;
	}
	memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
	sprintf((_SBYTE*)g_DispBuffer, "VOLUME:%lu BYTES", VolumeOfData);
	HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
		PGbar++;

	fg_SendBuffer[0] = 0x7E;
	fg_SendBuffer[1] = 0xA0;
	fg_SendBuffer[3] = 0x03;
	fg_SendBuffer[4] = g_UserAssociation;

	ret_lsh = (rrr_s << 5);
	fg_SendBuffer[5] = (ret_lsh | 0x10);
	ret_lsh = sss_s << 1;
	fg_SendBuffer[5] = ret_lsh | fg_SendBuffer[5];

	fg_SendBuffer[6] = 0x00;
	fg_SendBuffer[7] = 0x00;
	fg_SendBuffer[8] = 0xE6;
	fg_SendBuffer[9] = 0xE6;
	fg_SendBuffer[10] = 0x00;
	fg_SendBuffer[11] = 0xC0;
	fg_SendBuffer[12] = 0x01;
	fg_SendBuffer[13] = 0x81;
	fg_SendBuffer[14] = 0x00;
	fg_SendBuffer[15] = classID;
	fg_SendBuffer[16] = Obis_a;
	fg_SendBuffer[17] = Obis_b;
	fg_SendBuffer[18] = Obis_c;
	fg_SendBuffer[19] = Obis_d;
	fg_SendBuffer[20] = Obis_e;
	fg_SendBuffer[21] = Obis_f;
	fg_SendBuffer[22] = Attr;

	if(LoadSurveyFlag == 0)
	{
		fg_SendBuffer[23] = 0x00;
		fg_SendBuffer[2] = 0x19;
		fg_SendBuffer[26] = 0x7e;
		fcs(fg_SendBuffer+1, 5, 1);
		fcs(fg_SendBuffer+1, 23, 1);
		SendPkt(27);
	}
	else
	{
		LoadSurveyFlag = 0;

		fg_SendBuffer[23] = 0x01;
		fg_SendBuffer[24] = 0x01;
		fg_SendBuffer[25] = 0x02;
		fg_SendBuffer[26] = 0x04;
		fg_SendBuffer[27] = 0x02;
		fg_SendBuffer[28] = 0x04;
		fg_SendBuffer[29] = 0x12;
		fg_SendBuffer[30] = 0x00;
		fg_SendBuffer[31] = 0x08;
		fg_SendBuffer[32] = 0x09;
		fg_SendBuffer[33] = 0x06;
		fg_SendBuffer[34] = 0x00;
		fg_SendBuffer[35] = 0x00;
		fg_SendBuffer[36] = 0x01;
		fg_SendBuffer[37] = 0x00;
		fg_SendBuffer[38] = 0x00;
		fg_SendBuffer[39] = 0xFF;
		fg_SendBuffer[40] = 0x0F;
		fg_SendBuffer[41] = 0x02;
		fg_SendBuffer[42] = 0x12;
		fg_SendBuffer[43] = 0x00;
		fg_SendBuffer[44] = 0x00;

		fg_SendBuffer[45] = 0x09;
		fg_SendBuffer[46] = 0x0C;
		fg_SendBuffer[47] = (YYYY1 / 256);
		fg_SendBuffer[48] = (_UBYTE) (YYYY1 % 256);
		fg_SendBuffer[49] = (_UBYTE) MM1;
		fg_SendBuffer[50] = (_UBYTE) DD1;
		fg_SendBuffer[51] = 0xFF;
		fg_SendBuffer[52] = 0x00;
		fg_SendBuffer[53] = 0x00;
		fg_SendBuffer[54] = 0x00;
		fg_SendBuffer[55] = 0x00;
		fg_SendBuffer[56] = 0x80;//0xFF;//
		fg_SendBuffer[57] = 0x00;//0xFF;
		fg_SendBuffer[58] = 0x00;//0xFF;

		fg_SendBuffer[59] = 0x09;
		fg_SendBuffer[60] = 0x0C;
		fg_SendBuffer[61] = (YYYY2 / 256);
		fg_SendBuffer[62] = (_UBYTE) (YYYY2 % 256);
		fg_SendBuffer[63] = (_UBYTE) MM2;
		fg_SendBuffer[64] = (_UBYTE) DD2;
		fg_SendBuffer[65] = 0xFF;				//Day of Week
		fg_SendBuffer[66] = 23;//23;	//0x00;				//Hour
		fg_SendBuffer[67] = 59;	//0x00;				//Minute
		fg_SendBuffer[68] = 0x00;				//Second
		fg_SendBuffer[69] = 0x00;				//Hundredth of Second
		fg_SendBuffer[70] = 0x80;//0xFF;				//Deviation High Byte
		fg_SendBuffer[71] = 0x00;//0xFF;				//Deviation Low Byte
		fg_SendBuffer[72] = 0x00;//0xFF;				//Clock Status

		fg_SendBuffer[73] = 0x01;
		fg_SendBuffer[74] = 0x00;
		fg_SendBuffer[75] = 0x00;
		fg_SendBuffer[75] = 0x00;
		fg_SendBuffer[76] = 0x00;
		fg_SendBuffer[77] = 0x7E;

		fg_SendBuffer[2] = 0x4C;
		fcs( (fg_SendBuffer + 1), 5, 1);
		fcs( (fg_SendBuffer + 1), 74, 1);
		SendPkt(78);
	}

	l_RetVal = Read(fg_RxBuffer);

	if(!l_RetVal)
	{
		return(0);
	}
    if(fg_ctratio == 1)
    {
        fg_ctratio=0;
        if(fg_RxBuffer[15] != 0x0B)// 0B
        {
          if((fg_RxBuffer[15] == 0x12))
          {
            ctratio = (long)fg_RxBuffer[16]*256 + (long)fg_RxBuffer[17];
          }
          else if((fg_RxBuffer[15] != 0x06)||(fg_RxBuffer[15] != 0x05))
          {
            ctratio = (long)((long)fg_RxBuffer[16]*65536*256 + (long)fg_RxBuffer[17]*65536 + (long)fg_RxBuffer[18]*256 + (long)fg_RxBuffer[19]);
          }
        }
        if( ctratio ==1)
        fg_CtSignflag = 1;
    }

	l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);

	fp =  fopen(RAW_FILE_NAME, "a");
	if(fp == NULL)
		fprintf(stderr, "\nfile open error append mode\n");

	fprintf(fp, "\n");

	for(counter = 16; counter <= 21; counter++)//obis code write
	{
		fprintf(fp, "%02X", fg_SendBuffer[counter]);
		VolumeOfData +=2;
	}

	fprintf(fp," %02X ",fg_SendBuffer[22]);

	FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

	if ((fg_RxBuffer[11] == 0xC4) && (fg_RxBuffer[12] == 0x02) )
	{
		datablockno = (_ULONG) (fg_RxBuffer[15]*0 + ((_ULONG) fg_RxBuffer[16])*65536 +((_ULONG) fg_RxBuffer[17])*256 + fg_RxBuffer[18]);
		moreblock = ! fg_RxBuffer[14];
	}

	if (!(fg_RxBuffer[1] & 0x08) && (moreblock == 0) ) //== 0xA0
	{
        if( datablockno != 0x00 )
        {
            if(fg_RxBuffer[19] == 0x00 )
            {
                if(fg_RxBuffer[20] == 0x81 )
                {
                    for(counter=22; counter < l_Bytes; counter++)
                    {
                        fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }
                else if(fg_RxBuffer[20] == 0x82 )
                {
                    for(counter=23; counter < l_Bytes; counter++)
                    {
                        fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }

                else
                {
                    for(counter=21; counter < l_Bytes; counter++)
                    {
                        fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }
            }
        }
        else
        {
            for(counter=15; counter < l_Bytes; counter++)
            {
                fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
				if((classID==0x07)&&(Obis_a ==0x00)&&(Obis_b == 0x00)&&(Obis_c == 0x5E)&&(Obis_d==0x5B)&&(Obis_e == 0x0A)&&(Obis_f == 0xFF)&&(Attr==0x03))
				{
					if((fg_RxBuffer[counter] == 4)&&(l_Bytes == 16))
					{
						return 4;
						fclose(fp);
					}
				}
            }
        }
        fclose(fp);
		return 1;
	}
	if (moreblock == 1)
	{
        if(fg_RxBuffer[19] == 0x00 )
        {
            if(fg_RxBuffer[20] == 0x81 )
            {
                for(counter=22; counter < l_Bytes; counter++)
                {
                    fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                }
            }
            else if(fg_RxBuffer[20] == 0x82 )
            {
                for(counter=23; counter < l_Bytes; counter++)
                {
                    fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                }
            }
            else
            {
                for(counter=21; counter < l_Bytes; counter++)
                {
                    fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                }
            }
        }
	}
	else
	{
		for(counter=15; counter < l_Bytes; counter++)
		{
			fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
		}
	}

	while (fg_RxBuffer[1] & 0x08) //== 0xA8
	{
		ClearPacket();

	if(PGbar % 20 == 0)
	{
		HAL_DebugPrint(1, (_SBYTE*) "DATA DOWNLOADING....");
		HAL_DebugPrint(2, (_SBYTE*) "VOLUME:");
		HAL_DebugPrint(3, (_SBYTE*) "DON'T PRESS ANY KEY");
		HAL_DebugPrint(4, (_SBYTE*) "DURING DOWNLOADING");
		PGbar = 0;
	}
	memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
	sprintf((_SBYTE*)g_DispBuffer, "VOLUME:%lu BYTES", VolumeOfData);
	HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
		PGbar++;

		fg_SendBuffer[0] = 0x7E;
		fg_SendBuffer[1] = 0xA0;
		fg_SendBuffer[2] = 0x07;
		fg_SendBuffer[3] = 0x03;
		fg_SendBuffer[4] = g_UserAssociation;
		ret_lsh = rrr_s << 5;
		fg_SendBuffer[5] = (ret_lsh | 0x10);
		fg_SendBuffer[5] = fg_SendBuffer[5] | 0x01;
		fcs( (fg_SendBuffer + 1), 5, 1);
		fg_SendBuffer[8] = 0x7E;
		SendPkt(9);

		l_RetVal = Read(fg_RxBuffer);

		if(!l_RetVal)
		{
			fclose(fp);
			return(0);
		}
		l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);
		FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

		if (!( fg_RxBuffer[1] & 0x08)) //0xA0
		{
			for(counter=8; counter < l_Bytes; counter++)
			{
				fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
			}

			break;
		}

		if (fg_RxBuffer[1] & 0x08) //== 0xA8
		{
			for(counter=8; counter < l_Bytes; counter++)
			{
				fprintf(fp, "%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
			}
		}
	}//While(0xA8)

	while (moreblock)
	{
		moreblock = 0;
		ClearPacket();

		if(PGbar % 20 == 0)
		{
			HAL_DebugPrint(1, (_SBYTE*) "DATA DOWNLOADING....");
			HAL_DebugPrint(2, (_SBYTE*) "VOLUME:");
			HAL_DebugPrint(3, (_SBYTE*) "DON'T PRESS ANY KEY");
			HAL_DebugPrint(4, (_SBYTE*) "DURING DOWNLOADING");
			PGbar = 0;
		}
		memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		sprintf((_SBYTE*)g_DispBuffer, "VOLUME:%lu BYTES", VolumeOfData);
		HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
			PGbar++;

		fg_SendBuffer[0] = 0x7E;
		fg_SendBuffer[1] = 0xA0;
		fg_SendBuffer[2] = 0x13;
		fg_SendBuffer[3] = 0x03;
		fg_SendBuffer[4] = g_UserAssociation;

		ret_lsh = (rrr_s << 5);
		fg_SendBuffer[5] = (ret_lsh | 0x10);
		ret_lsh = (sss_s << 1);
		fg_SendBuffer[5] = (ret_lsh | fg_SendBuffer[5]);

		fg_SendBuffer[8] = 0xE6;
		fg_SendBuffer[9] = 0xE6;
		fg_SendBuffer[10] = 0x00;
		fg_SendBuffer[11] = 0xC0;
		fg_SendBuffer[12] = 0x02;
		fg_SendBuffer[13] = 0x81;
		fg_SendBuffer[14] = 0x00;
		fg_SendBuffer[15] = 0x00;
		fg_SendBuffer[16] = (datablockno / 256);
		fg_SendBuffer[17] = (datablockno % 256);
		fcs( (fg_SendBuffer + 1), 5, 1);
		fcs( (fg_SendBuffer + 1), 17, 1);
		fg_SendBuffer[20] = 0x7E;

		SendPkt(21);

		l_RetVal = Read(fg_RxBuffer);

		if(!l_RetVal)
		{
			fclose(fp);
			return(0);
		}
		l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);
		FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

		if ((fg_RxBuffer[11] == 0xC4) && (fg_RxBuffer[12] == 0x02) )
		{
			datablockno = (_ULONG) (fg_RxBuffer[15]*0 +((_ULONG) fg_RxBuffer[16])*65536 +((_ULONG) fg_RxBuffer[17])*256 + fg_RxBuffer[18]);
			moreblock = ! fg_RxBuffer[14];
		}

		if(!(fg_RxBuffer[1] & 0x08)) //==0xA0
		{
            if(fg_RxBuffer[19] == 0x00 )
            {
                if(fg_RxBuffer[20] == 0x81 )
                {
                    for(counter=22; counter < l_Bytes; counter++)
                    {
                        fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }
                else if(fg_RxBuffer[20] == 0x82 )
                {
                    for(counter=23; counter < l_Bytes; counter++)
                    {
                        fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }
                else
                {
                    for(counter=21; counter < l_Bytes; counter++)
                    {
                        fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }
            }
		}
		if (fg_RxBuffer[1] & 0x08)  //== 0xA8
		{
			if (moreblock == 1)
			{
				if(fg_RxBuffer[19] == 0x00 )
                {
                    if(fg_RxBuffer[20] == 0x81 )
                    {
                        for(counter=22; counter < l_Bytes; counter++)
                        {
                            fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }
                    else if(fg_RxBuffer[20] == 0x82 )
                    {
                        for(counter=23; counter < l_Bytes; counter++)
                        {
                            fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }
                    else
                    {
                        for(counter=21; counter < l_Bytes; counter++)
                        {
                            fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }
                }
			}
			else
			{
				if(fg_RxBuffer[19] == 0x00 )
                {
                    if(fg_RxBuffer[20] == 0x81 )
                    {
                        for(counter=22; counter < l_Bytes; counter++)
                        {
                            fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }
                    else if(fg_RxBuffer[20] == 0x82 )
                    {
                        for(counter=23; counter < l_Bytes; counter++)
                        {
                            fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }
                    else
                    {
                        for(counter=21; counter < l_Bytes; counter++)
                        {
                            fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }
                }
			}
		}

		while (fg_RxBuffer[1] & 0x08)  //== 0xA8
		{
			ClearPacket();

			if(PGbar % 20 == 0)
			{
				HAL_DebugPrint(1, (_SBYTE*) "DATA DOWNLOADING....");
				HAL_DebugPrint(2, (_SBYTE*) "VOLUME:");
				HAL_DebugPrint(3, (_SBYTE*) "DON'T PRESS ANY KEY");
				HAL_DebugPrint(4, (_SBYTE*) "DURING DOWNLOADING");
				PGbar = 0;
			}
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((_SBYTE*)g_DispBuffer, "VOLUME:%lu BYTES", VolumeOfData);
			HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
				PGbar++;

			fg_SendBuffer[0] = 0x7E;
			fg_SendBuffer[1] = 0xA0;
			fg_SendBuffer[2] = 0x07;
			fg_SendBuffer[3] = 0x03;
			fg_SendBuffer[4] = g_UserAssociation;
			ret_lsh = rrr_s << 5;
			fg_SendBuffer[5] = ret_lsh | 0x10;
			fg_SendBuffer[5] = fg_SendBuffer[5] | 0x01;
			fcs(fg_SendBuffer+1, 5, 1);
			fg_SendBuffer[8] = 0x7E;

			SendPkt(9);
			l_RetVal = Read(fg_RxBuffer);

			if(!l_RetVal)
			{
				return(0);
				fclose(fp);
			}
			l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);
			FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

			for(counter=8; counter < l_Bytes; counter++)
			{
				fprintf(fp,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
			}
		}//While(0xA8)
	}//while(moreblock)

	fclose(fp);
	return 1;
}
void ClearPacket(void)
{
	head = tail = 0;//errFlg=0; intFlg count
	memset(fg_SendBuffer, 255, sizeof(fg_SendBuffer));
	memset(fg_RxBuffer, 255, sizeof(fg_RxBuffer));
}


int countDigits(long inteiro,int max)
{
	_SBYTE *string;
	string = (_SBYTE *) calloc(max+1,sizeof(_SBYTE));
	sprintf(string,"%lu",inteiro);
	return strlen(string);
}

int DefaultDate(void)
{
	struct rtc_time l_Rtc;
    int y,i;
	//getdate(&d);
	UTL_RTCRead(&l_Rtc);
	fflush(stdin);
    LcdClear();
gggg: 	HAL_DebugPrint(1, (_SBYTE*)"ENTER NUMBER OF DAYS=");
		HAL_DebugPrint(2, (_SBYTE*)"Press ENT after ");
		HAL_DebugPrint(3, (_SBYTE*)"Entry");
    y = APP_GetUserAlphaNumeric();

    y =y-1;
    if(y>=365)
    {
		 LcdClear();
		 y=y+1;
		 memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		 sprintf((_SBYTE*)g_DispBuffer, "You Entered %d", y);
		 HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
		 HAL_DebugPrint(2, (_SBYTE*)"Out Of Limit");
		 HAL_DebugPrint(3, (_SBYTE*)"Press a key.. ");
		 _getkey();
		 LcdClear();
		 goto gggg;
    }
    if(y<=0)
    return 0;
	DD2 = l_Rtc.tm_mday;//03
	MM2 = (l_Rtc.tm_mon + 1);//01
	YYYY2 = (l_Rtc.tm_year+1900);//2014
	DD1 = DD2 -y;

	if(DD1 < 1)
	{

       	MM1 = (MM2 - 1);
		if(MM1 <= 0)
		{
			MM1 = 12 ;
			YYYY1 = YYYY2-1;
		}
		else
		{
			YYYY1 = YYYY2;
		}

		if( (MM1 == 1) || (MM1 == 3) || (MM1 == 5) || (MM1 == 7) || (MM1 == 8) || (MM1 == 10) || (MM1 == 12) )
		{
            i=0;
			DD1 = 31 + DD1;


    x1:            if(DD1 < 1)
                    {
                        MM1 = (MM2 - 2 -i);

                        if(MM1 <= 0)
                        {
                            MM1 = 12+MM1;
                            YYYY1 = YYYY2-1;
                        }
                        else
                        {
                            YYYY1 = YYYY2;
                        }

                        if( (MM1 == 1) || (MM1 == 3) || (MM1 == 5) || (MM1 == 7) || (MM1 == 8) || (MM1 == 10) || (MM1 == 12) )
                        {
                            DD1 = 31 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x1;
                            }
                        }
                        else if( (MM1 == 4) || (MM1 == 6) || (MM1 == 9) || (MM1 == 11) )
                        {
                            DD1 = 30 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x1;
                            }
                        }
                       else if(MM1 == 2)
                        {
                            if (isleapyear(YYYY2))
                            DD1 = 29 + DD1;
                            else
                            DD1 = 28 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x1;
                            }
                        }
                }

		}
		else if( (MM1 == 4) || (MM1 == 6) || (MM1 == 9) || (MM1 == 11) )
		{
        i=0;
		DD1 = 30 + DD1;
  x2:	                 if(DD1 < 1)
                        	{
                        		MM1 = (MM2 - 2 - i);

                        		if(MM1 <= 0)
                        		{
                        			MM1 = 12+MM1;
                        			YYYY1 = YYYY2-1;
                        		}
                        		else
                        		{
                        			YYYY1 = YYYY2;
                        		}

                        		if( (MM1 == 1) || (MM1 == 3) || (MM1 == 5) || (MM1 == 7) || (MM1 == 8) || (MM1 == 10) || (MM1 == 12) )
                        		{
                        			DD1 = 31 + DD1;
                                     if(DD1 < 1)
                                     {
                                        i++;
                                        goto x2;
                                     }
                        		}
                        		else if( (MM1 == 4) || (MM1 == 6) || (MM1 == 9) || (MM1 == 11) )
                        		{
                        			DD1 = 30 + DD1;
                                     if(DD1 < 1)
                                     {
                                        i++;
                                        goto x2;
                                     }
                                }
                               else if(MM1 == 2)
                                {
                                    if (isleapyear(YYYY2))
                                    DD1 = 29 + DD1;
                                    else
                                    DD1 = 28 + DD1;
                                    if(DD1 < 1)
                                    {
                                        i++;
                                        goto x2;
                                    }
                                }
                            }
                        // printf("\n2.%d,%d,%d",DD1,MM1,YYYY1);
                        // _getkey();
		}
		else if (isleapyear(YYYY2))
			{
            i=0;
				DD1 = 29 + DD1;

 x3:              if(DD1 < 1)
                    {
                        MM1 = (MM2 - 2 -i);

                        if(MM1 <= 0)
                        {
                            MM1 = 12+MM1;
                            YYYY1 = YYYY2-1;
                        }
                        else
                        {
                            YYYY1 = YYYY2;
                        }

                        if( (MM1 == 1) || (MM1 == 3) || (MM1 == 5) || (MM1 == 7) || (MM1 == 8) || (MM1 == 10) || (MM1 == 12) )
                        {
                            DD1 = 31 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x3;
                            }
                        }
                        else if( (MM1 == 4) || (MM1 == 6) || (MM1 == 9) || (MM1 == 11) )
                        {
                            DD1 = 30 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x3;
                            }
                        }
                        else if(MM1 == 2)
                        {
                          DD1 = 29 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x3;
                            }
                        }
                    }

			}

			else
			{
				DD1 = 28 + DD1;
                i=0;
 x4:              if(DD1 < 1)
                    {
                        MM1 = (MM2 - 2 -i);

                        if(MM1 <= 0)
                        {
                           MM1 = 12+MM1;
                            YYYY1 = YYYY2-1;
                        }
                        else
                        {
                            YYYY1 = YYYY2;
                        }

                        if( (MM1 == 1) || (MM1 == 3) || (MM1 == 5) || (MM1 == 7) || (MM1 == 8) || (MM1 == 10) || (MM1 == 12) )
                        {
                            DD1 = 31 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x4;
                            }
                        }
                        else if( (MM1 == 4) || (MM1 == 6) || (MM1 == 9) || (MM1 == 11) )
                        {
                            DD1 = 30 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x4;
                            }
                        }
                        else if(MM1 == 2)
                        {
                            DD1 = 28 + DD1;
                            if(DD1 < 1)
                            {
                                i++;
                                goto x4;
                            }
                        }

			}
		}
	}
	else
	{
		MM1 = MM2;
		YYYY1 = YYYY2;
	}
	LcdClear();
	memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
	sprintf((_SBYTE*)g_DispBuffer, (const char*)"Start:%.2d/%.2d/%d",DD1,MM1,YYYY1);
	HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);

	memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
	sprintf((_SBYTE*)g_DispBuffer, (const char*)"End  :%.2d/%.2d/%d",DD2,MM2,YYYY2);
	HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
	HAL_DebugPrint(3, (_SBYTE*) "Press a key..");
     _getkey();
     return 1;
}
int GetDates(void)
{
	while(1)
	{
		while(1)
		{
			LcdClear();
			fflush(stdin);
			HAL_DebugPrint(1, (_SBYTE*)"Enter LS Start Date");
			HAL_DebugPrint(2, (_SBYTE*)"(DD MM YYYY)");
			HAL_DebugPrint(3, (_SBYTE*)"Enter Start Date");
			HAL_DebugPrint(4, (_SBYTE*)"(and Press ENT)");
			DD1 = APP_GetUserAlphaNumeric ();
			HAL_DebugPrint(7, (_SBYTE*)"");
			HAL_DebugPrint(3, (_SBYTE*)"Enter Start Month");
			HAL_DebugPrint(4, (_SBYTE*)"(and Press ENT)");
			MM1 = APP_GetUserAlphaNumeric ();
			HAL_DebugPrint(7, (_SBYTE*)"");
			HAL_DebugPrint(3, (_SBYTE*)"Enter Start year");
			HAL_DebugPrint(4, (_SBYTE*)"(and Press ENT)");
			YYYY1 = APP_GetUserAlphaNumeric ();
			HAL_DebugPrint(7, (_SBYTE*)"");

			if(isdatevalid(MM1,DD1,YYYY1))
			{
				break;
			}
			else
			{
				HAL_DebugPrint(4, (_SBYTE*)"Invalid date");
                UTL_Wait(500);
                return 0;
			}
		}
		while(1)
		{
			LcdClear();
			fflush(stdin);
			HAL_DebugPrint(1, (_SBYTE*)"Enter LS End Date");
			HAL_DebugPrint(2, (_SBYTE*)"(DD MM YYYY):");
			HAL_DebugPrint(3, (_SBYTE*)"Enter Start Date");
			HAL_DebugPrint(4, (_SBYTE*)"(and Press ENT)");
			DD2= APP_GetUserAlphaNumeric ();
			HAL_DebugPrint(7, (_SBYTE*)"");
			HAL_DebugPrint(3, (_SBYTE*)"Enter Start Month");
			HAL_DebugPrint(4, (_SBYTE*)"(and Press ENT)");
			MM2= APP_GetUserAlphaNumeric ();
			HAL_DebugPrint(7, (_SBYTE*)"");
			HAL_DebugPrint(3, (_SBYTE*)"Enter Start year");
			HAL_DebugPrint(4, (_SBYTE*)"(and Press ENT)");
			YYYY2= APP_GetUserAlphaNumeric ();
			HAL_DebugPrint(7, (_SBYTE*)"");

			if(isdatevalid(MM2, DD2, YYYY2))
			{
				break;
			}
			else
			{
				HAL_DebugPrint(4, (_SBYTE*)"Invalid date");
                UTL_Wait(500);
                return 0;
			}
		}
		LcdClear();
		memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		HAL_DebugPrint(1, (_SBYTE*)"LS Start Date");
		sprintf((_SBYTE*)g_DispBuffer, (const char*)":%02d %02d %d", DD1, MM1, YYYY1);
		HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
		HAL_DebugPrint(3, (_SBYTE*)"LS End Date");
		memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		sprintf((_SBYTE*)g_DispBuffer, (const char*)":%02d %02d %d", DD2, MM2, YYYY2);
		HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);

		if(YYYY2 > YYYY1)
		{
			break;
		}
		else if( (YYYY2 ==YYYY1) && (MM2 > MM1) )
		{
			break;
		}
		else if( (YYYY2 == YYYY1) && (MM2 == MM1) && (DD2 >= DD1) )
		{
			break;
		}
		else
		{
			HAL_DebugPrint(1, (_SBYTE*)"Load Survey End Date");
			HAL_DebugPrint(2, (_SBYTE*)"Must be greater than");
			HAL_DebugPrint(3, (_SBYTE*)"Load Survey Start Date");
			HAL_DebugPrint(4, (_SBYTE*)"Press any key..");
			_getkey();
		}
	}
     return 1;
}
//=========================================================================
//The root function called from Main
int selData(void)
{
	LcdClear();
	int l_Choice = 0,Sub_choice,i, auth;
    unsigned char ccmd[20];
	int Result=0;
	char l_ChoiceVal = 0;
	char l_Choice2 = 0;
	_UBYTE chLS;

	LoadSurveyFlag = 0;
	PGbar=0;

	while(1)
	{
kaka:	fflush(stdin);

		l_Choice = MainScr();		//selScr();
		if(l_Choice == USER_CHOISE_NONE)
		{
			exit(0);
			break;
		}
		else if(l_Choice == 7)
		{
			RS232_PortClose(SERAIL_RJ_PORT_NO);
			UTL_Wait(100);
			RS232_PortOpen(SERAIL_RJ_PORT_NO, BAUD_RATE);
			if(Read_Serial_No()) 
            continue;
          	nameplate();         
		}
		else if(l_Choice == 8)
		{
			MetersDownloaded();
		}
		else if(l_Choice == USER_CHOISE_VIEW_MODE)
		{
			RS232_PortClose(SERAIL_RJ_PORT_NO);
			UTL_Wait(100);
			RS232_PortOpen(SERAIL_RJ_PORT_NO, BAUD_RATE);
			LcdClear();
			//+++++++++++++++++++++++++++++++++++++	
			if(Read_Serial_No())
			continue;
			Authentication();
			//++++++++++++++++++++++++++++++++++++++	
			View_Mode();
			continue;
		}
		else if(l_Choice == USER_CHOISE_DOWNLOAD_MODE)
		{
			RS232_PortClose(SERAIL_RJ_PORT_NO);
			UTL_Wait(100);
			RS232_PortOpen(SERAIL_RJ_PORT_NO, BAUD_RATE);
			LcdClear();
SUBCHOICE:  Sub_choice = selScr();

			if(Sub_choice == USER_CHOISE_NONE)
			{
				continue;
			}
			else if(Sub_choice == 4 )
            {
				LcdClear();
				HAL_DebugPrint(1,(_SBYTE*)"EnterPassword:");
				_getkey();
    			for (i=0;i<5;i++)
                {
    				ccmd[i]=toupper(_getkey());
					#ifdef LINUX_PC
							fprintf(stderr, "*");
					#else
							goto_xy (1, 16+i);
							LcdPrintf("*");
					#endif
                }
                    ccmd[i]=NULL;
                if(strcmp((_SBYTE*)ccmd,"CLRDB")!=0)
                {
                    LcdClear();
                    HAL_DebugPrint(1, (_SBYTE*)"WRONG PASSWORD!");
                    HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
                    _getkey();
                    goto SUBCHOICE;
                 }
                 else
                 {
                    LcdClear();
                    HAL_DebugPrint(1, (_SBYTE*)"Are you sure");
                    HAL_DebugPrint(2, (_SBYTE*)"to delete data");
                    HAL_DebugPrint(3, (_SBYTE*)"1. Yes");
                    HAL_DebugPrint(4, (_SBYTE*)"0. No");
                    while((l_ChoiceVal > 0x31 || l_ChoiceVal < 0x30))
                    l_ChoiceVal = _getkey();

                    if(l_ChoiceVal == '1')
                    {
                    	i = system("rm -rf /DLMS");
                    	LcdClear();
                    	HAL_DebugPrint(1, (_SBYTE*)"Successful delete");
                    	HAL_DebugPrint(2, (_SBYTE*)"Press a key to Exit");
                    }
                    else
                    {
                    	LcdClear();
                    	HAL_DebugPrint(1, (_SBYTE*)"Press a key to Exit");
                    }

                    _getkey();
                    //UTL_Wait(1800);
                    ///goto SUBCHOICE;
                    exit(0);
                 }
            }
			else if(Sub_choice == 3 )
            {
				LcdClear();
				HAL_DebugPrint(1,(_SBYTE*)"EnterPassword:");
				_getkey();
    			for (i=0;i<4;i++)
                {
                    ccmd[i]=toupper(_getkey());
					#ifdef LINUX_PC
							fprintf(stderr, "*");
					#else
							goto_xy (1, 16+i);
							LcdPrintf("*");
					#endif
                }
                    ccmd[i]=NULL;
                if(strcmp((_SBYTE*)ccmd,"AUTH")!=0)
                {
                    LcdClear();
                    HAL_DebugPrint(1, (_SBYTE*)"WRONG PASSWORD!");
                    HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY...");
                    _getkey();
                    goto SUBCHOICE;
                 }
                 else
                 {
					LcdClear();
					auth= remove(APP_FILE_DLMS_AUTH);

					if(auth==0)
					{
						HAL_DebugPrint(1, (_SBYTE*)" ::Erased:: ");
						HAL_DebugPrint(2, (_SBYTE*)"Press a Key");
					}
					else
					{
						HAL_DebugPrint(1, (_SBYTE*)"File not found");
						HAL_DebugPrint(2, (_SBYTE*)"Press a Key");
					}
					_getkey();
					goto SUBCHOICE;
                 }
            }
			else if(Sub_choice == 1 )
			{
            	if(Read_Serial_No())
    			continue;
    			if(Authentication() == 0)
    				continue;
				VolumeOfData = 0;
          		Selective_Data();
          	}
			else if(Sub_choice == 2 )
			{
                if(Read_Serial_No())
    			continue;
    			Authentication();
				rrr_c=0;
				rrr_c1=0;
				rrr_s=0;
				sss_c=0;
				sss_c1=0;
				sss_s=0;
				Result = 0;
                // activate();
				//==========================================LOAD SURVEY DAYS===================================================================
				while(1)
				{
		                LcdClear();
						fflush(stdin);
						HAL_DebugPrint(1, (_SBYTE*)"ENTER LOAD SURVEY");
						HAL_DebugPrint(2, (_SBYTE*)"1.START & END DATE");
						HAL_DebugPrint(3, (_SBYTE*)"2.NUMBER OF DAYS");
						HAL_DebugPrint(4, (_SBYTE*)"0.EXIT     ENTER:");
						//scanf("%d",&chLS);
						chLS = _getkey();
						if(chLS >= 48) chLS -= 48;
						if(chLS == 1 || chLS == 2)
							break;
                        else if(chLS == 0)
                         goto kaka;
						else
						{
                            LcdClear();
                            HAL_DebugPrint(1, (_SBYTE*)"WRONG CHOICE");
                            HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
							fflush(stdin);
							_getkey();
						}
					}
					if(chLS == 1)
                    {
                        LcdClear();
                        if(!GetDates())
                        goto kaka;

                    }
					else if(chLS == 2)
                    {
                        LcdClear();
                        if(!DefaultDate())
                        goto kaka;
                    }
				//=============================================================================================================
				fp = fopen(RAW_FILE_NAME,"w");
				if(fp == NULL)
				{
					fprintf(stderr, "file opening error %s", RAW_FILE_NAME);
				}
				switch(type)
				{
					case METER_TYPE_A:
						fprintf(fp,"\nTYPEA");
						break;
					case METER_TYPE_B:
						fprintf(fp,"\nTYPEB");
						break;
					case METER_TYPE_C:
						fprintf(fp,"\nTYPEC");
						break;
				}
				fclose(fp);
				PGbar = 0;
				g_UserAssociation = MR_ASSOCIATION;
				LcdClear();
				VolumeOfData = 0;
				HAL_DebugPrint(1, (_SBYTE*)"DOWNLOADING START....");
				//Disconnect();
				Result = SetNormalResponseMode ();

				if(Result == 1)
				{
					Result =0;
					Result = AssociationRequest();

					if(Result == 1)
					{
						LcdClear();
						Result = 0;
						HAL_DebugPrint(1, (_SBYTE*)"INSTANT DATA");PGbar = 20;
						Result = ReadInstantData();

						if(Result == 0)
						{
							continue;
						}

						Result = 0;
						//if(type == 2)
						{
							HAL_DebugPrint(1, (_SBYTE*)"BILLING DATA");PGbar = 20;
							Result = ReadBillingDataB();
						}
						//else
						if(Result == 0)
						{
							continue;
						}
						Result = 0;
						HAL_DebugPrint(1, (_SBYTE*)"BILLING DATA");PGbar = 20;
						Result = ReadBillingDataC();
                        if(Result == 0)
						{
							continue;
						}

						Result = 0;
						HAL_DebugPrint(1, (_SBYTE*)"EVENT DATA");PGbar = 20;
						Result = ReadEventData();

						if(Result == 0)
						{
							continue;
						}

						Result = 0;
						HAL_DebugPrint(1, (_SBYTE*)"LOAD SURVEY");PGbar = 20;
						Result = ReadLoadSurvey(3);

						if(Result == 0)
						{
							continue;
						}
                        Result = 0;
						Result = ReadPrepaymentData();

						if(Result == 0)
						{
							continue;
						}

						if(Result == 1)
						{
							Disconnect();
							CopytoMainFile();
						}
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"METER READ SUCCESS");
						HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
						_getkey();

					}
					else
					{
						Disconnect();
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
						HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
						HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
						HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
						l_Choice = _getkey();
						if(l_Choice == '0')
						{
							//DisablePort();
							return(1);
						}
					}
				}
				else
				{
					Disconnect();
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
					HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
					HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
					HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
					HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
					l_Choice = _getkey();
					if(l_Choice == '0')
					{
						//DisablePort();
						return(1);
					}
				}
			}//deactivate();
		}
		else if(l_Choice == USER_CHOISE_UPLOAD_MODE)
		{
			RS232_PortClose(SERAIL_RJ_PORT_NO);
			UTL_Wait(100);
			RS232_PortOpen(SERAIL_RJ_PORT_NO, 19200);//Requred baud rate as per CPRI specification  115200
			rrr_c=0;
			rrr_c1=0;
			rrr_s=0;
			sss_c=0;
			sss_c1=0;
			sss_s=0;
			VolumeOfData = 0;
            LcdClear();
			selData_Upload();
			exit(0); ///mundel because after one communication complete uploading seconds time not work so exist app and again enter if required
		}
		else if(l_Choice == USER_CHOISE_PROGRAM_MODE)//USER_CHOISE_PROGRAM_MODE)
		{
			l_Choice2 =displayProgPm();
			if(l_Choice2 == 0)
			goto kaka;

		}
		else if(l_Choice == USER_CHOISE_AUTHENTICATION)//USER_CHOISE_AUTHENTICATION)
		{
			RS232_PortClose(SERAIL_RJ_PORT_NO);
			UTL_Wait(100);
			RS232_PortOpen(SERAIL_RJ_PORT_NO, 19200);
			LcdClear();
			CMRI_AUTH();
			exit(0); ///mundel because after one communication complete uploading seconds time not work so exist app and again enter if required
		}
		else if(l_Choice == USER_CHOISE_TEST_MODE)
		{
			RS232_PortClose(SERAIL_RJ_PORT_NO);
			UTL_Wait(100);
			RS232_PortOpen(SERAIL_RJ_PORT_NO, BAUD_RATE);;
			LcdClear();
			if(Read_Serial_No())
			continue; //while(1)
			Authentication();
			Test_Mode();
			continue;//while(1)
		}
	}//while(1) loop end here

	return 0;
}
void CopytoMainFile(void)
{
	_ULONG l_Bytes;
	static unsigned char fg_FileBuffer[FILE_BUFFER_LEN]; 	//used for copying file at last
//	if(fp != NULL)
//	fclose(fp);
	fp =  fopen(RAW_FILE_NAME, "rb");
	if(fp == NULL) fprintf(stderr, "\nfile opening error %s", RAW_FILE_NAME);
	fp1 = fopen((const char*)FileName, "ab");
	if(fp1 == NULL)
	{
		fprintf(stderr, "\nfile opening error %s", FileName);
		fp1 = fopen((const char*)FileName, "w");
		if(fp1 == NULL)
		fprintf(stderr, "\nfile opening error %s", FileName);
	}

	fseek(fp1, 0L, SEEK_END);	//Appneding to the file
	//ErrPrt(1,8,"PLEASE WAIT..     ");
	do
	{
		l_Bytes = fread(fg_FileBuffer, 1, FILE_BUFFER_LEN, fp);
		fwrite(fg_FileBuffer, l_Bytes, 1, fp1);

	}while(l_Bytes);

	fclose(fp);
	fclose(fp1);
	remove(RAW_FILE_NAME);
}


int Read(unsigned char *RxDt)
{
	int i=0;
	unsigned long time1 = 0;

	while(1)
	{
		while( (rx_flag == 0) && (time1 < 3000) )    //655350
		{
			UTL_Wait(1);
			time1++;
		}

		if(rx_flag == 1)
		{
			UTL_Wait(1);
			time1++;
		}

		if(time1 >= 3000)
		{
			fprintf(stderr, "\ntime out\n");
			rx_flag = 0;
			return 0;
		}
		else
		{
			if(head != tail)
			{
				RxDt[i++] = ReadByte();
				time1 = 0;
			}
        }
		if( (i > 7) && (i >= ((( RxDt[1] & 0x07)*0x100) +( RxDt[2] + 2))) )
        {
            FrameRcvFlag =1;
			break;
        }
	}
	rx_flag=0;
	return 1;
}

//=============================================================

int SendPkt(int NoofBytes)
{
	//Write(fg_SendBuffer,0,NoofBytes);
	RS232_PortSendString(SERAIL_RJ_PORT_NO, fg_SendBuffer, NoofBytes);
	return 0;
}


//=========================================================
int Authentication(void)
{
	unsigned char srno1[10], srno2[10], chr[30], i=0;
	int z = 0, temp, LLS_length, SAP;   ////16,  123456,1111111111111111,8888888888888888,0000,00,00,00,,,
	long x = 0, pos = 0, L_Serialno;		 ////SAP Sr.no      LLS                      HLS                            Mask  ,   MD   ,   LS Peroid   ,   Billing date   ,    Tod timming     ,    Physical address    ,   for future use  ,   for future use
	L_Serialno = Serialno;
	RTC_Flag = 0;
	MD_Int_Flag = 0;
	BillingDateFlag = 0;
	Profile_cap_Flag  = 0;
	Tod_Flag = 0;
	MD_Reser_Flag = 0;
	Phy_add_Flag = 0;
	x = 7;
	memset(srno1, 0, sizeof(srno1));
	memset(srno2, 0, sizeof(srno2));

	fprintf(stderr, "\n111pp");

	while((L_Serialno > 0) || (x > 1))
	{
		srno1[x] = 0x30 + L_Serialno%10;
		L_Serialno/=10;
		x--;
	}
	fprintf(stderr, "\n211pp");

	if(x == 1)
	{
		srno1[0] = 0x20;
		srno1[1] = 0x20;
	}
	else if(x == 0)
	{
		srno1[0] = 0x20;
	}
	if(fg_MtrSlnoInString)
	{
		for(i=0;i<8;i++)
		{
			srno1[i] = MtrSlNoNew[i];
		}
		srno1[8] ='\0';
	}
	stream_r = fopen(APP_FILE_DLMS_AUTH, "r+");

	if(NULL == stream_r)
	{
		fprintf(stderr, "\nFile not present\n");
		SAP = 16;
		stream_r=fopen(APP_FILE_DLMS_AUTH, "w");
		if(NULL == stream_r)
		{
			fprintf(stderr, "\nFile openning error\n");
		}
		goto LLSKeyManual;
	}

	fprintf(stderr, "\n311pp");
	do
	{
		LLS_length = 0;
        fseek(stream_r,0,SEEK_END);
    	x = ftell(stream_r);
        if(x > 5)
        {
    		fseek(stream_r, pos, SEEK_SET);
    		temp = 0;
    		while(( temp  != ',') && ( pos < x ))
			{
    			temp = fgetc(stream_r);
				pos++;
			}
			fgets ((_SBYTE*)srno2 , 9 , stream_r);
			fgetc(stream_r);//for read  ','
			pos += 9;
			temp = 0;
			do
			{
				temp = fgetc(stream_r);
				pos++;
				LLS_length++;
			}while(( temp  != ',') && ( pos < x ));
        }
		pos += 25; //16 byte HLS key + ,(semi colon) + MeterType + 7 flags
	}
    while ((strcmp ((const char*)srno1, (const char*)srno2) != 0) && ( pos < x ));
	fprintf(stderr, "\n511pp");
	if(strcmp ((const char*)srno1, (const char*)srno2) == 0)
	{
		memset(fg_LLSSecret, '\0', 17);
		fseek(stream_r,-LLS_length,SEEK_CUR);
		x = -1;
		do
		{
			x++;
			chr[x] = fgetc(stream_r);
		}while( chr[x]  != ',');
		strncpy((_SBYTE*)fg_LLSSecret,(const char*)chr,x);
		for(x = 0; x < 16; x++ )
		{
			fg_HLSSecret[x] = fgetc(stream_r);
		}
		fg_HLSSecret[16] ='\0';
		fgetc(stream_r);//for read  ','
		chr[0] = fgetc(stream_r);
		if( 'A' == chr[0] )type=1;else if( 'B' == chr[0] )type=2; else if( 'C' == chr[0] )type=3;

		if('1'==fgetc(stream_r))RTC_Flag = 1;
		if('1'==fgetc(stream_r))MD_Int_Flag = 1;
		if('1'==fgetc(stream_r))BillingDateFlag = 1;
		if('1'==fgetc(stream_r))Profile_cap_Flag  = 1;
		if('1'==fgetc(stream_r))Tod_Flag = 1;
		if('1'==fgetc(stream_r))MD_Reser_Flag = 1;
		if('1'==fgetc(stream_r))Phy_add_Flag = 1;
	}
	else
	{
		pos = 5; ///just random data put for finding last SAP value not 0 or 1 due to fatch last 0x0A
		temp =0;
        fseek(stream_r,0,SEEK_END);
    	x = ftell(stream_r);
        if(x>5)
        {
		while( temp  != 0x0A)
		{
			fseek(stream_r,-pos,SEEK_END);
			temp = fgetc(stream_r);
			pos++;
		}
		x = 0;
		temp = 0;
		while( temp  != ',')
		{
			temp = fgetc(stream_r);
			chr[x] = temp;
			x++;
		}
        }
		chr[x] = '\0';
		SAP = atoi((const char*)chr);
		SAP++;
LLSKeyManual:while(1)
		{
            z=LLSscr();
			fflush(stdin);
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ENTER LLS SECRET:");

            if((z==2)||(z==1)) //hpl case
            {
                memset(fg_LLSSecret, '1', 16);
                break;
            }
            else     //others
            {
LLSKeyManual1:  fclose(stream_r);
                UTL_Wait(50);
                LcdClear();
                stream_r=fopen(APP_FILE_DLMS_AUTH, "w");
                HAL_DebugPrint(1, (_SBYTE*)"ENTER LLS SECRET:");
                //gets((_SBYTE*)fg_LLSSecret);
                APP_GetLLS_Key_String(fg_LLSSecret);
                fprintf(stderr, "fg_LLSSecret = %s", fg_LLSSecret);
                break;
            }
		}

		while(1)
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"Select Meter Category");
			HAL_DebugPrint(2, (_SBYTE*)"1.TYPE A");
			HAL_DebugPrint(3, (_SBYTE*)"2.TYPE B");
			HAL_DebugPrint(4, (_SBYTE*)"3.TYPE C");
			HAL_DebugPrint(5, (_SBYTE*)"4.Net Meter");
			HAL_DebugPrint(6, (_SBYTE*)"0.Back  Enter Choice");
			while((type > 0x34 || type < 0x30))
				type = _getkey();
				//type = _getkey();
			fprintf(stderr, "type = %c", type);
			if(type >= 48)
			type -= 48;
			if(type == 4)
			{
				type=2;
			}

			if( (type >= 1) && (type <= 3) )
			{
				break;
			}
			else if(type == 0)
			{
				return 0;
			}
		}
		fseek(stream_r,0,SEEK_END);
		fprintf(stream_r,"\n");
		fprintf(stream_r,"%d,",SAP);
		fputs((const char*)srno1,stream_r);
		fprintf(stream_r,",");
		fputs((const char*)fg_LLSSecret,stream_r);
		fprintf(stream_r,",");
		fputs("0000000000000000",stream_r);
		if(type ==1 )fprintf(stream_r,",A");else if(type ==2 )fprintf(stream_r,",B");else if(type ==3 )fprintf(stream_r,",C");
		fputs("0000000",stream_r);
		//mask   ,	MD	, Profile period	,	billing date	, 	Tod Timing 	,	Physical address	,    for future use,        for future use
	}
	fclose(stream_r);
	return(1);
}
//=============================================================================
int  Read_Serial_No(void)
{
	unsigned char Result =0,l_loop=0,l_loop1=0,i=0,j=0;
	char l_Choice;

	g_UserAssociation = PC_ASSOCIATION;
	rrr_c=0;
	rrr_c1=0;
	rrr_s=0;
	sss_c=0;
	sss_c1=0;
	sss_s=0;

	while(1)
	{
		Result = 0;
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
		Result = SetNormalResponseMode ();

		if(Result == 1)
		{
			Result =0;
			Result = AssociationRequest();
			if(Result == 1)
			{
				Result = 0;
				fg_SendBuffer[0] = 0x7E;//HDLC Frame Opening Flag
				fg_SendBuffer[1] = 0xA0;//HDLC Frame TYPE 3
				fg_SendBuffer[3] = 0x03;//destination address
				fg_SendBuffer[4] = g_UserAssociation;//source address
				fg_SendBuffer[5] =0x32;
				fg_SendBuffer[6] = 0x00;//Fcs
				fg_SendBuffer[7] = 0x00;//Fcs
				fg_SendBuffer[8] = 0xE6;//E6 E6 00 LLC Bytes
				fg_SendBuffer[9] = 0xE6;
				fg_SendBuffer[10] = 0x00;
				fg_SendBuffer[11] = 0xC0;
				fg_SendBuffer[12] = 0x01;
				fg_SendBuffer[13] = 0x81;
				fg_SendBuffer[14] = 0x00;

				fg_SendBuffer[15] = 0x01;
				fg_SendBuffer[16] = 0x00;
				fg_SendBuffer[17] = 0x00 ;
				fg_SendBuffer[18] = 0x60 ;
				fg_SendBuffer[19] = 0x01 ;
				fg_SendBuffer[20] = 0x00 ;
				fg_SendBuffer[21] = 0xff ;
				fg_SendBuffer[22] = 0x02 ;
				fg_SendBuffer[23] = 0x00 ;

				fg_SendBuffer[2] = 0x19;//no. of bytes between flag
				fg_SendBuffer[26] = 0x7e;//Frame closing Flag
				fcs(fg_SendBuffer+1, 5, 1);
				fcs(fg_SendBuffer+1, 23, 1);
				SendPkt(27);

				Read(fg_RxBuffer);
				memset(MtrSlNo, 0, sizeof(MtrSlNo));
				memset(MtrSlNoNew, 0, sizeof(MtrSlNoNew));

				if(( fg_RxBuffer[15]==0x06 )||( fg_RxBuffer[15]==0x05 ))
				{
					fg_MtrSlnoInString=0;
					Serialno = (long)((long)fg_RxBuffer[16]*65536*256 + (long)fg_RxBuffer[17]*65536 + (long)fg_RxBuffer[18]*256 + (long)fg_RxBuffer[19]);
				}
				else if(( fg_RxBuffer[15]==0x09 )||( fg_RxBuffer[15]==0x0A )||( fg_RxBuffer[15]==0x08 ))
				{
					fg_MtrSlnoInString=1;
					for(l_loop=0;l_loop<fg_RxBuffer[16];l_loop++)
					{
						if((fg_RxBuffer[17+l_loop] == 0x20)||((fg_RxBuffer[17+l_loop] >=0x30)&&(fg_RxBuffer[17+l_loop] <= 0x39)))
						{
							if(fg_RxBuffer[17+l_loop] != 0x20)
							{
								MtrSlNo[l_loop1] = fg_RxBuffer[17+l_loop];
								l_loop1++;
							}
						}
					}
					for(i=0;i<8;i++)
					{
						if(i < (8 - l_loop1))
						MtrSlNoNew[i]= 0x20;
						else
						{
							MtrSlNoNew[i]=MtrSlNo[j];
							j++;
						}
						//printf("\nMtrSlNoNew[%c] ,MtrSlNo[%c]",MtrSlNoNew[i],MtrSlNo[i]);_getkey();
					}
				}

				Disconnect();
				break;
			}
			else
			{
				LcdClear();
				Disconnect();
				HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
				HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
				HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
				HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
				//Disconnect();
				l_Choice = _getkey();
				if(l_Choice == '0')
				return(1);
			}
		}
		else
		{
			Disconnect();
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE");
			HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
			HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
			HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
			l_Choice = _getkey();
			if(l_Choice == '0')
			return(1);
		}
	}
	return(0);
}


//==========================================================================

void Selective_Data (void)
{
    int chLS,Result,choice1;
    int Instdataflag = 0;

zz:	while(1)
    {
        fflush(stdin);
        choice1 = selScrSelected();
        if(choice1 == 0 && Instdataflag == 0) break;
        if(choice1 == 0 && Instdataflag == 1)
        {
            CopytoMainFile();
            break;
        }
        //======================================//load survey entry  ====================================================================
        if(choice1 == 3)
        {
            while(1)
            {
                LcdClear();
                fflush(stdin);
                HAL_DebugPrint(1, (_SBYTE*)"ENTER LOAD SURVEY");
                HAL_DebugPrint(2, (_SBYTE*)"1.START & END DATE");
                HAL_DebugPrint(3, (_SBYTE*)"2.NUMBER OF DAYS");
                HAL_DebugPrint(4, (_SBYTE*)"0.EXIT     ENTER:");
                //scanf("%d",&chLS);
                chLS = _getkey();
                if(chLS >= 48) chLS -= 48;
                if(chLS == 1 || chLS == 2) break;
                else if(chLS == 0)
                goto zz;
                else
                {
                	LcdClear();
                	HAL_DebugPrint(1, (_SBYTE*)"WRONG CHOICE");
                	HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
                    fflush(stdin);
                    _getkey();
                }
            }
            if(chLS == 1)
            {
            	LcdClear();
                if(!GetDates())
                goto zz;
            }
            else if(chLS == 2)
            {
				LcdClear();
				HAL_DebugPrint(1, (_SBYTE*)"0.EXIT");
				HAL_DebugPrint(2, (_SBYTE*)"ENTER NUMBER OF DAYS");
				if(!DefaultDate())
				goto zz;
            }
        }

        PGbar = 0;
        g_UserAssociation = MR_ASSOCIATION;
        Result = 0;
        rrr_c=0;rrr_c1=0;rrr_s=0;sss_c=0;sss_c1=0;sss_s=0;
        Result = SetNormalResponseMode();
        if(Result == 1)
        {
			Result =0;
			Result = AssociationRequest();
			if(Result == 1)
			{
				if(Instdataflag == 0)
				{
					Instdataflag = 1;
					fp = fopen(RAW_FILE_NAME,"w");
					if(fp == NULL)
					{
						fprintf(stderr, "file opening error %s", RAW_FILE_NAME);
					}
					if(type == 3) fprintf(fp,"\nTYPEC");
					else if(type == 2) fprintf(fp,"\nTYPEB");
					else if(type == 1) fprintf(fp,"\nTYPEA");
					fclose(fp);
					Result = 0;
					Result = ReadInstantData();
					if(Result == 0)
						break;
				}

				if(choice1 == 1)//billing data  ========================================================================================================
				{
					Result = 0;
					//if(type == 2)
					//{
						Result = ReadBillingDataB();
						if(Result == 0)
						break;
					//}
					//else
					Result = ReadBillingDataC();
					if(Result == 0)
					break;
					else
					{
						VolumeOfData=0;
						Disconnect();
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"BILLING DATA READ");
						HAL_DebugPrint(2, (_SBYTE*)"SUCCESSFULLY");
						HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
						CopytoMainFile();
						Instdataflag = 0;
						_getkey();
					}
				}
				else if(choice1 == 2)//event data   ========================================================================================================
				{ // activate();
					Result = 0;
					Result = ReadEventData();
					if(Result == 0)
					break;
					else
					{
						VolumeOfData=0;
						Disconnect();
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"EVENT DATA READ");
						HAL_DebugPrint(2, (_SBYTE*)"SUCCESSFULLY");
						HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
						CopytoMainFile();
						Instdataflag = 0;
						_getkey();
					}
				}
				else if(choice1 == 3)// Load survey ======================================================================================================
				{
					Result = 0;
					Result = ReadLoadSurvey(3);
					if(Result == 0)
					break;
					else
					{
						VolumeOfData=0;
						Disconnect();
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"LOAD SURVEY READ");
						HAL_DebugPrint(2, (_SBYTE*)"SUCCESSFULLY");
						HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
						CopytoMainFile();
						Instdataflag = 0;
						_getkey();
					}
				}
			}
			else
			{
				HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
				HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
				Disconnect();
				_getkey();
			}
        }
        else
        {
        	Disconnect();
        	HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
        	HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE");
        	HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();
        }
    }
}


void FrameType(_UBYTE *cp, _UBYTE &rrr_c, _UBYTE &rrr_c1, _UBYTE &rrr_s, _UBYTE &sss_c, _UBYTE &sss_c1, _UBYTE &sss_s)
{
	if(cp[1] & 0x08 )   //==0xA8    //segment bit == 1
	{
		rrr_s++;
		if(rrr_s>7)
		rrr_s=0;
	}
	else if(!(cp[1]  & 0x08))  //==0xA0  //segment bit == 0
	{
		sss_s++;
		if(sss_s>7)
		sss_s=0;

		rrr_s++;
		if(rrr_s>7)
		rrr_s=0;
	}

}
//==================================================================
int NoOfMeter(void)
{
   int l_Count = 0;
   DIR *dir;
   struct dirent *ent;
   unsigned char l_Tempfilename[200][13];//[30][13]

   	if((dir = opendir("/DLMS")) != NULL)
   	{
   		while((ent = readdir(dir)) != NULL)
   		{
   			strcpy((_SBYTE*)l_Tempfilename[l_Count], (const char*)ent->d_name);
			if(l_Tempfilename[l_Count][0] != '.')
			{
				l_Count++;
			}
   		}
   		closedir(dir);
   	}
   	else
   	{
   		perror("Could not open directory");
   		return 0;
   	}

    return (l_Count);
}

unsigned char  CTRATIO (void)
{
	unsigned int l_CtRatio = 0;
	ViewRequest(1,1,0,0,4,2,255,2);
	Read(fg_RxBuffer);

	if(fg_RxBuffer[15] != 0x0B)// 0B
	{
		if((fg_RxBuffer[15] == 0x12))
		{
			l_CtRatio = (long)fg_RxBuffer[16]*256 + (long)fg_RxBuffer[17];
		}
		else if((fg_RxBuffer[15] != 0x06)||(fg_RxBuffer[15] != 0x05))
		{
			l_CtRatio = (long)((long)fg_RxBuffer[16]*65536*256 + (long)fg_RxBuffer[17]*65536 + (long)fg_RxBuffer[18]*256 + (long)fg_RxBuffer[19]);
		}
	}
	if( l_CtRatio == 1)
	{
		fg_CtSignflag = 1;
	}

	return 0;
}
