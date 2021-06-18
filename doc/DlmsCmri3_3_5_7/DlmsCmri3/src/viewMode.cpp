/*
 * viewMode.cpp
 *
 *  Created on: 21-Sep-2015
 *      Author: mundel
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/rtc.h>
#include <dirent.h>
#include <math.h>

#include "display.h"
#include "Config.h"
#include "global.h"
#include "typedef.h"
#include "FUNCTH.h"
#include "rs232.h"
#include "utility.h"
#include "display.h"
#include "AES128.h"
#include "comm.h"
#include "pdsequivalent.h"
#include "Upload.h"
#include "auth.h"
#include "Program.h"
#include "viewMode.h"
#include "hhuLcd.h"


static char fg_Sign;
static char fg_VoltFlag;
//============================================================================================

int View_Mode(void)
{
	if(type == 1)//TYPE A METER
	{
		View_ModeTypeA();
	}
	else if(type == 2)//TYPE B METER
	{
		View_ModeTypeB();
	}
	else //TYPE C METER
	{
		View_ModeTypeA();
	}

	return(1);
}
//============================================================================================
int ViewRequest(unsigned char classID,unsigned char Obis_a,unsigned char Obis_b,\
				unsigned char Obis_c,unsigned char Obis_d,unsigned char Obis_e,\
				unsigned char Obis_f,unsigned char Attr)
{
	_UBYTE ret_lsh,l_RetVal = 0x00;
	ClearPacket();
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

	fg_SendBuffer[23] = 0x00;
	fg_SendBuffer[2] = 0x19;
	fg_SendBuffer[26] = 0x7e;
	fcs(fg_SendBuffer+1, 5, 1);
	fcs(fg_SendBuffer+1, 23, 1);
	SendPkt(27);

	l_RetVal = Read(fg_RxBuffer);
	if(!l_RetVal)
	{
		return(0);
	}
	if((fg_RxBuffer[14] == 0x00)||(fg_RxBuffer[14] == 0x01))
	{
		FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
		if(fg_RxBuffer[14] == 0x01)
		return 0;
		return 1;
	}
	return 0;
}

int View_ModeTypeA(void)
{
    unsigned int l_int=0;
	int l_PF;
    char l_Choice, PFBYTE,fg_PF_Single_byte = 0;
    //signed long l_SignedTemp;
	unsigned char l_result,meter_no_array[10]; //l_Point, temp,
	unsigned long l_temp; //AP_POWER
	long Sign_POWER;

PrevScreenA1:	LcdClear();

	HAL_DebugPrint(1, (_SBYTE*)" METER PARAMETERS   ");
	HAL_DebugPrint(2, (_SBYTE*)"1.METER SERIAL NO.");
	HAL_DebugPrint(3, (_SBYTE*)"2.RTC DATE & TIME ");
	HAL_DebugPrint(4, (_SBYTE*)"3.CURRENT         ");
	HAL_DebugPrint(5, (_SBYTE*)"4.VOLTAAGE        ");
	HAL_DebugPrint(6, (_SBYTE*)"5.PF              ");
	HAL_DebugPrint(7, (_SBYTE*)"6.BACK   0. NEXT");

	l_Choice = 0;
	while((l_Choice > 0x36) || (l_Choice < 0x30)) l_Choice = _getkey();
	if(l_Choice == '0') goto nextScreenA1;
	if(l_Choice == '6') return(1);
	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
	rrr_c=0;
	rrr_c1=0;
	rrr_s=0;
	sss_c=0;
	sss_c1=0;
	sss_s=0;
	Disconnect();
	l_result = SetNormalResponseMode ();
	if(l_result == 1)
	{
		l_result =0;
		l_result = AssociationRequest();

		if(l_result == 1)
		{
			HAL_DebugPrint(1, (_SBYTE*)"METER PARAMETERS\n");
			switch(l_Choice)
			{
				//=====================================================================================================================
				case '1': //serial no read
					LcdClear();
					l_result = ViewRequest(1,0,0,96,1,0,255,2);//sr no.
					if( fg_RxBuffer[15]==0x06 )
					{
						Serialno = (long)((long)fg_RxBuffer[16]*65536*256 + (long)fg_RxBuffer[17]*65536 + (long)fg_RxBuffer[18]*256 + (long)fg_RxBuffer[19]);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%06lu",(long)Serialno);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}
					else if(fg_RxBuffer[16]==0x0A )
					{
						meter_no_array[5] = fg_RxBuffer[26]-48;
						meter_no_array[4] = fg_RxBuffer[25]-48;
						meter_no_array[3] = fg_RxBuffer[24]-48;
						meter_no_array[2] = fg_RxBuffer[23]-48;
						meter_no_array[1] = fg_RxBuffer[22]-48;
						meter_no_array[0] = fg_RxBuffer[21]-48;
						Serialno=(meter_no_array[0]*pow(10,5))+(meter_no_array[1]*pow(10,4))+(meter_no_array[2]*pow(10,3))+(meter_no_array[3]*pow(10,2))+(meter_no_array[4]*pow(10,1))+(meter_no_array[5]*pow(10,0));
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%c%c%c%c%c%c%c%c%c%c",fg_RxBuffer[17],fg_RxBuffer[18],fg_RxBuffer[19],fg_RxBuffer[20],fg_RxBuffer[21],fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24],fg_RxBuffer[25],fg_RxBuffer[26]);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}
					else if(fg_RxBuffer[16]==0x09 )
					{
						meter_no_array[5] = fg_RxBuffer[25]-48;
						meter_no_array[4] = fg_RxBuffer[24]-48;
						meter_no_array[3] = fg_RxBuffer[23]-48;
						meter_no_array[2] = fg_RxBuffer[22]-48;
						meter_no_array[1] = fg_RxBuffer[21]-48;
						meter_no_array[0] = fg_RxBuffer[20]-48;
						Serialno=(meter_no_array[0]*pow(10,5))+(meter_no_array[1]*pow(10,4))+(meter_no_array[2]*pow(10,3))+(meter_no_array[3]*pow(10,2))+(meter_no_array[4]*pow(10,1))+(meter_no_array[5]*pow(10,0));
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%c%c%c%c%c%c%c%c%c",fg_RxBuffer[17],fg_RxBuffer[18],fg_RxBuffer[19],fg_RxBuffer[20],fg_RxBuffer[21],fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24],fg_RxBuffer[25]);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}
					else if(fg_RxBuffer[16]==0x08 )
					{
						meter_no_array[5] = fg_RxBuffer[25]-48;
						meter_no_array[4] = fg_RxBuffer[24]-48;
						meter_no_array[3] = fg_RxBuffer[23]-48;
						meter_no_array[2] = fg_RxBuffer[22]-48;
						meter_no_array[1] = fg_RxBuffer[21]-48;
						meter_no_array[0] = fg_RxBuffer[20]-48;
						if((fg_RxBuffer[19] < 0x30)||(fg_RxBuffer[19] > 0x39))
						meter_no_array[0] = 0;
						if((fg_RxBuffer[20] < 0x30)||(fg_RxBuffer[20] > 0x39))
						meter_no_array[1] = 0;
						if((fg_RxBuffer[21] < 0x30)||(fg_RxBuffer[21] > 0x39))
						meter_no_array[2] = 0;
						if((fg_RxBuffer[22] < 0x30)||(fg_RxBuffer[22] > 0x39))
						meter_no_array[3] = 0;
						if((fg_RxBuffer[23] < 0x30)||(fg_RxBuffer[23] > 0x39))
						meter_no_array[4] = 0;
						Serialno=(meter_no_array[0]*pow(10,5))+(meter_no_array[1]*pow(10,4))+(meter_no_array[2]*pow(10,3))+(meter_no_array[3]*pow(10,2))+(meter_no_array[4]*pow(10,1))+(meter_no_array[5]*pow(10,0));
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%c%c%c%c%c%c%c%c",fg_RxBuffer[17],fg_RxBuffer[18],fg_RxBuffer[19],fg_RxBuffer[20],fg_RxBuffer[21],fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24]);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}
					HAL_DebugPrint(2, (_SBYTE*)"OBIS CODE ");
					HAL_DebugPrint(3, (_SBYTE*)"(0.0.96.1.0.255)");
					break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '2':
					l_result = ViewRequest(8,0,0,1,0,0,255,2);//RTC
					if(l_result)
					{
						l_temp = fg_RxBuffer[17]*0x100 + fg_RxBuffer[18];
						HAL_DebugPrint(1, (_SBYTE*)"Date:(DD/MM/YYYY)");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "    :%02d/%02d/%d",fg_RxBuffer[20],fg_RxBuffer[19],l_temp);
						HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(3, (_SBYTE*)"Time:(HH:MM:SS)");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "    :%02d:%02d:%02d",fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24]);
						HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(5, (_SBYTE*)"RTC OBIS CODE");
						HAL_DebugPrint(6, (_SBYTE*)"(0.0.1.0.0.255)");
					}
					break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '3'://Current
					CTRATIO();
					l_result = ViewRequest(3,1,0,11,7,0,255,2);    	//for single phase
					if(l_result)
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,11,7,0,255,3);
						if(l_result)
						{
							Display_Value(1, (_SBYTE*)"Ph Current=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.11.7.0.255)");
						}
					}
					l_result = ViewRequest(3,1,0,91,7,0,255,2);    				//for single phase
					if(l_result)
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,91,7,0,255,3);
						if(l_result)
						{
							Display_Value(3, (_SBYTE*)"N Current I=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.91.7.0.255)");
						}
					}

					l_result = ViewRequest(3,1,0,31,7,0,255,2);
					if((fg_CtSignflag == 1) && (l_result))
					{
						l_temp = fg_RxBuffer[16];
						l_temp *= 0x100;
						l_temp += fg_RxBuffer[17];
						l_int = l_temp;
						l_int =l_int>>8;
						if(l_int & 0x80)
						{
							fg_Sign = 1;
							l_temp = ~l_temp ;
							l_temp = l_temp+1;
						}
						else
						fg_Sign = 0;

						l_result = ViewRequest(3,1,0,31,7,0,255,3);
						if(l_result)
						{
							Display_Signed_Value(1, (_SBYTE*)"Current IR=", l_temp);
							HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.31.7.0.255)");
						}
					}
					else if(l_result)
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,31,7,0,255,3);
						if(l_result)
						{
							Display_Value(1, (_SBYTE*)"Current IR=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.31.7.0.255)");
						}
					}

					l_result = ViewRequest(3,1,0,51,7,0,255,2);
					if(fg_CtSignflag == 1)
					{
						l_temp = fg_RxBuffer[16];
						l_temp *= 0x100;
						l_temp += fg_RxBuffer[17];
						l_int = l_temp;
						l_int =l_int>>8;
						if(l_int & 0x80)
						{
							fg_Sign = 1;
							l_temp = ~l_temp ;
							l_temp = l_temp+1;
						}
						else
						fg_Sign = 0;

						l_result = ViewRequest(3,1,0,51,7,0,255,3);
						if(l_result)
						{
							Display_Signed_Value(3, (_SBYTE*)"Current IY=", l_temp);
							HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.51.7.0.255)");
						}
					}
					else
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,51,7,0,255,3);
						if(l_result)
						{
							Display_Value(3, (_SBYTE*)"Current IY=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.51.7.0.255)");
						}
					}
					l_result = ViewRequest(3,1,0,71,7,0,255,2);
					if(fg_CtSignflag == 1)
					{
						fg_CtSignflag =0;
						l_temp = fg_RxBuffer[16];
						l_temp *= 0x100;
						l_temp += fg_RxBuffer[17];
						l_int = l_temp;
						l_int =l_int>>8;
						if(l_int & 0x80)
						{
							fg_Sign = 1;
							l_temp = ~l_temp ;
							l_temp = l_temp+1;
						}
						else
						fg_Sign = 0;

						l_result = ViewRequest(3,1,0,71,7,0,255,3);
						if(l_result)
						{
							Display_Signed_Value(5, (_SBYTE*)"Current IB=", l_temp);
							HAL_DebugPrint(6, (_SBYTE*)"OBIS(1.0.71.7.0.255)");
						}
					}
					else
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,71,7,0,255,3);
						if(l_result)
						{
							Display_Value(5, (_SBYTE*)"Current IB=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(6, (_SBYTE*)"OBIS(1.0.71.7.0.255)");
						}
					}
					break;
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '4':	//voltage
					fg_VoltFlag =1;
					l_result = ViewRequest(3,1,0,12,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,12,7,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"Voltage=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.12.7.0.255)");
					}

					fg_VoltFlag =1;
					l_result = ViewRequest(3,1,0,32,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,32,7,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"Voltage VRN=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.32.7.0.255)");
					}
					fg_VoltFlag =1;
					l_result = ViewRequest(3,1,0,52,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,52,7,0,255,3);
					if(l_result)
					{
						Display_Value(3, (_SBYTE*)"Voltage VYN=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.52.7.0.255)");
					}
					fg_VoltFlag =1;
					l_result = ViewRequest(3,1,0,72,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,72,7,0,255,3);
					if(l_result)
					{
						Display_Value(5, (_SBYTE*)"Voltage VBN=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(1.0.72.7.0.255)");
					}
					fg_VoltFlag =1;
					break;
				//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '5'://power factor
					 LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"PF    [OBIS]");
					l_temp=1;
					l_result = ViewRequest(3,1,0,33,7,0,255,2);
					if(l_result)
					{
						if((fg_RxBuffer[15] == 0x0F))
						{
							PFBYTE = fg_RxBuffer[16];
							fg_PF_Single_byte = 1;
						}
						else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
						{
							l_PF = fg_RxBuffer[16];
							l_PF *= 0x100;
							l_PF += fg_RxBuffer[17];
						}
						else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
						{
							l_PF = fg_RxBuffer[16];
							l_PF *= 0x100;
							l_PF += fg_RxBuffer[17];
							l_PF *= 0x100;
							l_PF += fg_RxBuffer[18];
							l_PF *= 0x100;
							l_PF += fg_RxBuffer[19];
						}
						else
						{
							PFBYTE = fg_RxBuffer[16];
							fg_PF_Single_byte = 1;
						}
					}
					l_result = ViewRequest(3,1,0,33,7,0,255,3);
					if(fg_RxBuffer[18] == 0xFE) l_temp = 100;
					else if(fg_RxBuffer[18] == 0xFD) l_temp = 1000;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							sprintf((_SBYTE*)g_DispBuffer, "R=%g[1,0,33,7,0,255",(float)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{

							sprintf((_SBYTE*)g_DispBuffer, "R=%g[1,0,33,7,0,255",(double)l_PF/l_temp);

						}
						HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
						//HAL_DebugPrint(2, (_SBYTE*)"OBIS(1,0,33,7,0,255)");
					}
					l_temp=1;
					l_result = ViewRequest(3,1,0,53,7,0,255,2);
					if((fg_RxBuffer[15] == 0x0F))
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
					}
					else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[18];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[19];
					}
					else
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					l_result = ViewRequest(3,1,0,53,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) l_temp =100;
					else if(fg_RxBuffer[18]==0xFD) l_temp =1000;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							//sprintf((_SBYTE*)g_DispBuffer, "Y phase PF = %g",(double)PFBYTE/l_temp);
							sprintf((_SBYTE*)g_DispBuffer, "Y=%g[1,0,53,7,0,255",(double)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{
							sprintf((_SBYTE*)g_DispBuffer, "Y=%g[1,0,53,7,0,255",(double)l_PF/l_temp);
						}
						HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
						//HAL_DebugPrint(4, (_SBYTE*)"OBIS(1,0,53,7,0,255)");
					}
					l_temp=1;
					l_result = ViewRequest(3,1,0,73,7,0,255,2);
					if((fg_RxBuffer[15] == 0x0F))
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
					}
					else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[18];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[19];

					}
					else
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					l_result = ViewRequest(3,1,0,73,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) l_temp =100;
					else if(fg_RxBuffer[18]==0xFD) l_temp =1000;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							//sprintf((_SBYTE*)g_DispBuffer, "B phase PF = %g",(double)PFBYTE/l_temp);
							sprintf((_SBYTE*)g_DispBuffer, "B=%g[1,0,73,7,0,255",(double)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{
							sprintf((_SBYTE*)g_DispBuffer, "B=%g[1,0,73,7,0,255",(double)l_PF/l_temp);
						}
						HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
						//HAL_DebugPrint(6, (_SBYTE*)"OBIS(1,0,73,7,0,255)");
						//HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
						//_getkey();

					}
					l_temp=1;
					l_result = ViewRequest(3,1,0,13,7,0,255,2);
					if((fg_RxBuffer[15] == 0x0F))
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
					}
					else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[18];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[19];
					}
					else
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					l_result = ViewRequest(3,1,0,13,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) l_temp =100;
					else if(fg_RxBuffer[18]==0xFD) l_temp =1000;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							sprintf((_SBYTE*)g_DispBuffer, "System  PF = %g",(double)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{
							sprintf((_SBYTE*)g_DispBuffer, "System PF = %g",(double)l_PF/l_temp);
						}
						HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(1,0,13,7,0,255)");
					}
					break;
                //------------------------------------------------------------
				default:
				   // goto nextScreen1;
					break;
			}
			//==================================================================================
			if(l_result == 1)
			{
				Disconnect();
			}
			HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
			HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
			Disconnect();
			l_Choice = _getkey();
			if(l_Choice == '0')
			return(1);
		}
	}
	else
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
		HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
		HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
		Disconnect();
		l_Choice = _getkey();
		if(l_Choice == '0')
		return(1);
	}

	goto PrevScreenA1;

nextScreenA1: LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)" METER PARAMETERS ");
	HAL_DebugPrint(2, (_SBYTE*)"1.FREQUENCY       ");
	HAL_DebugPrint(3, (_SBYTE*)"2.POWER           ");
	HAL_DebugPrint(4, (_SBYTE*)"3.ENERGY          ");
	HAL_DebugPrint(5, (_SBYTE*)"4.NO OF POWER FAILURE");
	HAL_DebugPrint(6, (_SBYTE*)"5.CUMUL. TAMPER COUNT");
	HAL_DebugPrint(7, (_SBYTE*)"6.BACK   0.NEXT");
	l_Choice = 0;
	while((l_Choice > 0x36) || (l_Choice < 0x30))  l_Choice = _getkey();
	if(l_Choice == '0')goto nextScreenA2;
	if(l_Choice == '6')goto PrevScreenA1;
	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
	rrr_c = rrr_c1 = rrr_s = sss_c = sss_c1 = sss_s = 0;
	Disconnect();
	l_result = SetNormalResponseMode ();
	if(l_result == 1)
	{
		l_result =0;
		l_result = AssociationRequest();

		if(l_result == 1)
		{
			HAL_DebugPrint(1, (_SBYTE*)"METER PARAMETERS");
			switch(l_Choice)
			{
				//=====================================================================================================================
				case '1':
					l_result = ViewRequest(3,1,0,14,7,0,255,2);//frequency
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,14,7,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"Frequency=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.14.7.0.255)");
					}
					break;
				//----------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '2':
					LcdClear();
					l_result = ViewRequest(3,1,0,9,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,9,7,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"APP. =", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.9.7.0.255)");
					}

					l_result = ViewRequest(3,1,0,1,7,0,255,2);
					if(fg_RxBuffer[15] == 0x05)
					{
						Sign_POWER = fg_RxBuffer[16];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[17];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[18];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[19];
					}
					l_result = ViewRequest(3,1,0,1,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) Sign_POWER /=100;
					else if(fg_RxBuffer[18]==0x02) Sign_POWER *=100;
					else if(fg_RxBuffer[18]==0x01) Sign_POWER *=10;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_RxBuffer[18]==0)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%.3f KW",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==1)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%.2f KW",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==2)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%.1f KW",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==3)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%g KW",(double)Sign_POWER/1000);
						}
						HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1,0,1,7,0,255)");
					}

					l_result = ViewRequest(3,1,0,3,7,0,255,2);
					if(fg_RxBuffer[15] == 0x05)
					{
						Sign_POWER = fg_RxBuffer[16];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[17];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[18];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[19];
					}
					l_result = ViewRequest(3,1,0,3,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) Sign_POWER /=100;
					else if(fg_RxBuffer[18]==0x02) Sign_POWER *=100;
					else if(fg_RxBuffer[18]==0x01) Sign_POWER *=10;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_RxBuffer[18]==0)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%.3f KVAR",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==1)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%.2f KVAR",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==2)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%.1f KVAR",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==3)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%g KVAR",(double)Sign_POWER/1000);
						}
						HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(1,0,3,7,0,255)");
					}

					break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '3':
					l_result = ViewRequest(3,1,0,1,8,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,1,8,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"CE=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.1.8.0.255)");
					}

					l_result = ViewRequest(3,1,0,5,8,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,5,8,0,255,3);
					if(l_result)
					{
						Display_Value(3, (_SBYTE*)"CE-Lg=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.5.8.0.255)");
					}

					l_result = ViewRequest(3,1,0,8,8,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,8,8,0,255,3);
					if(l_result)
					{
						Display_Value(5, (_SBYTE*)"CE-Ld=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(1.0.8.8.0.255)");
					}

					l_result = ViewRequest(3,1,0,9,8,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,9,8,0,255,3);
					if(l_result)
					{
						HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
						_getkey();
						LcdClear();
						Display_Value(1, (_SBYTE*)"CE=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.9.8.0.255)");
					}
				break;
				//----------------------------------------------------------------------------------------------------------
				case '4':
					l_result = ViewRequest(1,0,0,96,7,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"NO. OF POWER FAILURE");
						Display_Value1(2, (_SBYTE*)"=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.96.7.0.255)");
					}
					else
					{
						HAL_DebugPrint(1, (_SBYTE*)"NOT AVAILABLE");
					}
					break;
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '5':	//
					l_result = ViewRequest(1,0,0,94,91,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE TAMPER");
						Display_Value1(2, (_SBYTE*)"COUNT=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.94.91.0,255)");
					}
					break;
				//-----------------------------------------------------------------
				case '6'://
					l_result = ViewRequest(3,0,0,94,91,8,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,0,0,94,91,8,255,3);
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE POWER");
						HAL_DebugPrint(2, (_SBYTE*)"FAILURE DURATION");
						Display_Value1(3, (_SBYTE*)"=", l_temp);
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(0.0.94.91.8.255)");
					}
					break;
				//-----------------------------------------------------------------------------
				default:
					//goto nextScreen2;
				break;
			}	//==================================================================================
			if(l_result == 1)
			{
				Disconnect();
			}
			HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
			HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
			Disconnect();
			l_Choice = _getkey();
			if(l_Choice == '0')
			return(1);
		}
	}
	else
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
		HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
		HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
		Disconnect();
		l_Choice = _getkey();
		if(l_Choice == '0')
		return(1);
	}
	goto nextScreenA1;

nextScreenA2: LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)" METER PARAMETERS ");
	HAL_DebugPrint(2, (_SBYTE*)"1.CUMU.BILLING COUNT");
	HAL_DebugPrint(3, (_SBYTE*)"2.CUMU.PROGRAM COUNT");
	HAL_DebugPrint(4, (_SBYTE*)"3.BILLING DATE");
	HAL_DebugPrint(5, (_SBYTE*)"4.MAXIMUM DEMAND");
	HAL_DebugPrint(6, (_SBYTE*)"6. BACK");
	l_Choice = 0;
	while((( l_Choice > 0x34)||(l_Choice < 0x31 ))&&( l_Choice != 0x36 ))  l_Choice = _getkey();
	if(l_Choice == '6') goto nextScreenA1;
	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
	rrr_c=0;
	rrr_c1=0;
	rrr_s=0;
	sss_c=0;
	sss_c1=0;
	sss_s=0;
	l_result = SetNormalResponseMode ();
	if(l_result == 1)
	{
		l_result =0;
		l_result = AssociationRequest();

		if(l_result == 1)
		{
			HAL_DebugPrint(1, (_SBYTE*)"METER PARAMETERS\n");
			switch(l_Choice)
			{
				//==============================================================
				case '1':
					l_result = ViewRequest(1,0,0,0,1,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE BILLING");
						Display_Value1(2, (_SBYTE*)"COUNT=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.0.1.0.255)");
					}
					break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '2'://
					l_result = ViewRequest(1,0,0,96,2,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE PROGRAMING");
						Display_Value1(2, (_SBYTE*)"COUNT=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.96.2.0.255)");
					}
					break;
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '3':	//BILLING DATE
					l_result = ViewRequest(3,0,0,0,1,2,255,2);
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						l_temp = fg_RxBuffer[17]*0x100 + fg_RxBuffer[18];
						HAL_DebugPrint(1, (_SBYTE*)"BILLING ");
						sprintf((_SBYTE*)g_DispBuffer, "Date:(DD/MM/YYYY)");
						HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "    :%02d/%02d/%02d",fg_RxBuffer[20],fg_RxBuffer[19],l_temp);
						HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "Time:(HH:MM:SS)");
						HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "    :%02d:%02d:%02d",fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24]);
						HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(0.0.0.1.2.255)");
					}
					else
					{
						HAL_DebugPrint(1, (_SBYTE*)"NOT AVAILABLE");
					}
					break;
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '4':	//MD (kW)
					l_result = ViewRequest(4,1,0,1,6,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(4,1,0,1,6,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"MAX.DEMAND=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.1.6.0.255)");
					}

					l_result = ViewRequest(4,1,0,9,6,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(4,1,0,9,6,0,255,3);
					if(l_result)
					{
						Display_Value(3, (_SBYTE*)"MAX.DEMAND=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.9.6.0.255)");
					}
				break;
				//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				default:
					//goto nextScreen2;
				break;
			}	//=============================================================================================================================
			if(l_result == 1)
			{
				Disconnect();
			}
			HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
			HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
			Disconnect();
			l_Choice = _getkey();
			if(l_Choice == '0')
			return(1);
		}
	}
	else
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
		HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
		HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
		Disconnect();
		l_Choice = _getkey();
		if(l_Choice == '0')
		return(1);
	}
	goto nextScreenA2;
	//return(1);
}
int View_ModeTypeB(void)
{
	unsigned int l_int=0;
	int l_PF;
    char l_Choice, PFBYTE,fg_PF_Single_byte = 0;
    //signed long l_SignedTemp;
	unsigned char l_result,meter_no_array[10];;
	unsigned long l_temp;
    double AP_POWER;
	long Sign_POWER;
PrevScreenB1:	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)" METER PARAMETERS");
	HAL_DebugPrint(2, (_SBYTE*)"1.METER SERIAL NO.");
	HAL_DebugPrint(3, (_SBYTE*)"2.RTC DATE & TIME");
	HAL_DebugPrint(4, (_SBYTE*)"3.CURRENT");
	HAL_DebugPrint(5, (_SBYTE*)"4.VOLTAAGE");
	HAL_DebugPrint(6, (_SBYTE*)"5.PF");
	HAL_DebugPrint(7, (_SBYTE*)"6.BACK   0.NEXT");
	l_Choice = 0;
	while((l_Choice > 0x36) || (l_Choice < 0x30)) l_Choice = _getkey();
	if(l_Choice == '0') goto nextScreenB1;
	if(l_Choice == '6') return(1);
	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
	LcdClear();
	rrr_c=0;
	rrr_c1=0;
	rrr_s=0;
	sss_c=0;
	sss_c1=0;
	sss_s=0;
	Disconnect();
	l_result = SetNormalResponseMode ();
	if(l_result == 1)
	{
		l_result =0;
		l_result = AssociationRequest();

		if(l_result == 1)
		{
			switch(l_Choice)
			{	//=====================================================================================================================
				case '1':
					LcdClear();
					l_result = ViewRequest(1,0,0,96,1,0,255,2);//sr no.
					if( fg_RxBuffer[15]==0x06 )
					{
						Serialno = (long)((long)fg_RxBuffer[16]*65536*256 + (long)fg_RxBuffer[17]*65536 + (long)fg_RxBuffer[18]*256 + (long)fg_RxBuffer[19]);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%06lu",(long)Serialno);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}

					else if(fg_RxBuffer[16]==0x0A )
					{
						meter_no_array[5] = fg_RxBuffer[26]-48;
						meter_no_array[4] = fg_RxBuffer[25]-48;
						meter_no_array[3] = fg_RxBuffer[24]-48;
						meter_no_array[2] = fg_RxBuffer[23]-48;
						meter_no_array[1] = fg_RxBuffer[22]-48;
						meter_no_array[0] = fg_RxBuffer[21]-48;
						Serialno=(meter_no_array[0]*pow(10,5))+(meter_no_array[1]*pow(10,4))+(meter_no_array[2]*pow(10,3))+(meter_no_array[3]*pow(10,2))+(meter_no_array[4]*pow(10,1))+(meter_no_array[5]*pow(10,0));
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%c%c%c%c%c%c%c%c%c%c",fg_RxBuffer[17],fg_RxBuffer[18],fg_RxBuffer[19],fg_RxBuffer[20],fg_RxBuffer[21],fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24],fg_RxBuffer[25],fg_RxBuffer[26]);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}
					else if(fg_RxBuffer[16]==0x09 )
					{
						meter_no_array[5] = fg_RxBuffer[25]-48;
						meter_no_array[4] = fg_RxBuffer[24]-48;
						meter_no_array[3] = fg_RxBuffer[23]-48;
						meter_no_array[2] = fg_RxBuffer[22]-48;
						meter_no_array[1] = fg_RxBuffer[21]-48;
						meter_no_array[0] = fg_RxBuffer[20]-48;
						Serialno=(meter_no_array[0]*pow(10,5))+(meter_no_array[1]*pow(10,4))+(meter_no_array[2]*pow(10,3))+(meter_no_array[3]*pow(10,2))+(meter_no_array[4]*pow(10,1))+(meter_no_array[5]*pow(10,0));
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%c%c%c%c%c%c%c%c%c",fg_RxBuffer[17],fg_RxBuffer[18],fg_RxBuffer[19],fg_RxBuffer[20],fg_RxBuffer[21],fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24],fg_RxBuffer[25]);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}
					else if(fg_RxBuffer[16]==0x08 )
					{
						meter_no_array[5] = fg_RxBuffer[25]-48;
						meter_no_array[4] = fg_RxBuffer[24]-48;
						meter_no_array[3] = fg_RxBuffer[23]-48;
						meter_no_array[2] = fg_RxBuffer[22]-48;
						meter_no_array[1] = fg_RxBuffer[21]-48;
						meter_no_array[0] = fg_RxBuffer[20]-48;
						if((fg_RxBuffer[19] < 0x30)||(fg_RxBuffer[19] > 0x39))
						meter_no_array[0] = 0;
						if((fg_RxBuffer[20] < 0x30)||(fg_RxBuffer[20] > 0x39))
						meter_no_array[1] = 0;
						if((fg_RxBuffer[21] < 0x30)||(fg_RxBuffer[21] > 0x39))
						meter_no_array[2] = 0;
						if((fg_RxBuffer[22] < 0x30)||(fg_RxBuffer[22] > 0x39))
						meter_no_array[3] = 0;
						if((fg_RxBuffer[23] < 0x30)||(fg_RxBuffer[23] > 0x39))
						meter_no_array[4] = 0;
						Serialno=(meter_no_array[0]*pow(10,5))+(meter_no_array[1]*pow(10,4))+(meter_no_array[2]*pow(10,3))+(meter_no_array[3]*pow(10,2))+(meter_no_array[4]*pow(10,1))+(meter_no_array[5]*pow(10,0));
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "Sr.No.=%c%c%c%c%c%c%c%c",fg_RxBuffer[17],fg_RxBuffer[18],fg_RxBuffer[19],fg_RxBuffer[20],fg_RxBuffer[21],fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24]);
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
					}
					HAL_DebugPrint(2, (_SBYTE*)"OBIS(0.0.96.1.0.255)");
					break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '2':
					l_result = ViewRequest(8,0,0,1,0,0,255,2);//RTC
					if(l_result)
					{
						l_temp = fg_RxBuffer[17]*0x100 + fg_RxBuffer[18];
						HAL_DebugPrint(1, (_SBYTE*)"Date:(DD/MM/YYYY)");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "    :%02d/%02d/%d",fg_RxBuffer[20],fg_RxBuffer[19],l_temp);
						HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(3, (_SBYTE*)"Time:(HH:MM:SS)");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*) g_DispBuffer, "    :%02d:%02d:%02d",fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24]);
						HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(5, (_SBYTE*)"RTC OBIS CODE");
						HAL_DebugPrint(6, (_SBYTE*)"(0.0.1.0.0.255)");
					}
				break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '3'://Current
					CTRATIO();
					l_result = ViewRequest(3,1,0,31,7,0,255,2);
					if((fg_CtSignflag == 1))
					{
						l_temp = fg_RxBuffer[16];
						l_temp *= 0x100;
						l_temp += fg_RxBuffer[17];
						l_int = l_temp;
						l_int =l_int>>8;
						if(l_int & 0x80)
						{
							fg_Sign = 1;
							l_temp = ~l_temp ;
							l_temp = l_temp+1;
						}
						else
						fg_Sign = 0;
						l_result = ViewRequest(3,1,0,31,7,0,255,3);
						if(l_result)
						{
							Display_Signed_Value(1, (_SBYTE*)"Current IR=", l_temp);
							HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.31.7.0.255)");
						}
					}
					else
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,31,7,0,255,3);
						if(l_result)
						{
							Display_Value(1, (_SBYTE*)"Current IR=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.31.7.0.255)");
						}
					}

					l_result = ViewRequest(3,1,0,51,7,0,255,2);
					if((fg_CtSignflag == 1))
					{
						l_temp = fg_RxBuffer[16];
						l_temp *= 0x100;
						l_temp += fg_RxBuffer[17];
						l_int = l_temp;
						l_int =l_int>>8;
						if(l_int & 0x80)
						{
							fg_Sign = 1;
							l_temp = ~l_temp ;
							l_temp = l_temp+1;
						}
						else
						fg_Sign = 0;

						l_result = ViewRequest(3,1,0,51,7,0,255,3);
						if(l_result)
						{
							Display_Signed_Value(3, (_SBYTE*)"Current IY=", l_temp);
							HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.51.7.0.255)");
						}
					}
					else
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,51,7,0,255,3);
						if(l_result)
						{
							Display_Value(3, (_SBYTE*)"Current IY=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.51.7.0.255)");
						}
					}

					l_result = ViewRequest(3,1,0,71,7,0,255,2);
					if((fg_CtSignflag == 1))
					{
						fg_CtSignflag =0;
						l_temp = fg_RxBuffer[16];
						l_temp *= 0x100;
						l_temp += fg_RxBuffer[17];
						l_int = l_temp;
						l_int =l_int>>8;
						if(l_int & 0x80)
						{
							fg_Sign = 1;
							l_temp = ~l_temp ;
							l_temp = l_temp+1;
						}
						else
						fg_Sign = 0;

						l_result = ViewRequest(3,1,0,71,7,0,255,3);
						if(l_result)
						{
							Display_Signed_Value(5, (_SBYTE*)"Current IB=", l_temp);
							HAL_DebugPrint(5, (_SBYTE*)"OBIS(1.0.71.7.0.255)");
						}
					}
					else
					{
						l_temp = Calculate_Value();
						l_result = ViewRequest(3,1,0,71,7,0,255,3);
						if(l_result)
						{
							Display_Value(5, (_SBYTE*)"Current IB=", l_temp, (_SBYTE*)"");
							HAL_DebugPrint(6, (_SBYTE*)"OBIS(1.0.71.7.0.255)");
						}
					}
				break;
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '4':	//voltage
					fg_VoltFlag =1;
					l_result = ViewRequest(3,1,0,32,7,0,255,2);
					l_temp = Calculate_Value();

					l_result = ViewRequest(3,1,0,32,7,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"Voltage VRN=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.32.7.0.255)");
					}
					fg_VoltFlag =1;
					l_result = ViewRequest(3,1,0,52,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,52,7,0,255,3);
					if(l_result)
					{
						Display_Value(3, (_SBYTE*)"Voltage VYN=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.52.7.0.255)");
					}
					fg_VoltFlag =1;
					l_result = ViewRequest(3,1,0,72,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,72,7,0,255,3);
					if(l_result)
					{
						Display_Value(5, (_SBYTE*)"Voltage VBN=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(1.0.72.7.0.255)");
					}
					fg_VoltFlag =1;
				break;
				//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '5'://power factor
					LcdClear();
					l_temp=1;
					l_result = ViewRequest(3,1,0,33,7,0,255,2);
					if((fg_RxBuffer[15] == 0x0F))
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
					}
					else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[18];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[19];
					}
					else
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					l_result = ViewRequest(3,1,0,33,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) l_temp =100;
					else if(fg_RxBuffer[18]==0xFD) l_temp =1000;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							sprintf((_SBYTE*)g_DispBuffer, "R phase PF = %g",(float)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{
							sprintf((_SBYTE*)g_DispBuffer, "R phase PF = %g",(double)l_PF/l_temp);
						}
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1,0,33,7,0,255)");
					}
					l_temp=1;
					l_result = ViewRequest(3,1,0,53,7,0,255,2);
					if((fg_RxBuffer[15] == 0x0F))
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
					}
					else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[18];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[19];
					}
					else
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					l_result = ViewRequest(3,1,0,53,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) l_temp =100;
					else if(fg_RxBuffer[18]==0xFD) l_temp =1000;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							sprintf((_SBYTE*)g_DispBuffer, "Y phase PF = %g",(double)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{
							sprintf((_SBYTE*)g_DispBuffer, "Y phase PF = %g",(double)l_PF/l_temp);
						}
						HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1,0,53,7,0,255)");
					}
					l_temp=1;
					l_result = ViewRequest(3,1,0,73,7,0,255,2);
					if((fg_RxBuffer[15] == 0x0F))
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
					}
					else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[18];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[19];

					}
					else
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					l_result = ViewRequest(3,1,0,73,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) l_temp =100;
					else if(fg_RxBuffer[18]==0xFD) l_temp =1000;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							sprintf((_SBYTE*)g_DispBuffer, "B phase PF = %g",(double)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{
							sprintf((_SBYTE*)g_DispBuffer, "B phase PF = %g",(double)l_PF/l_temp);
						}
						HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(1,0,73,7,0,255)");
						HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
						_getkey();
					}
					l_temp=1;
					l_result = ViewRequest(3,1,0,13,7,0,255,2);
					if((fg_RxBuffer[15] == 0x0F))
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					else if((fg_RxBuffer[15] == 0x10)||(fg_RxBuffer[15] == 0x12) )
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
					}
					else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
					{
						l_PF = fg_RxBuffer[16];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[17];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[18];
						l_PF *= 0x100;
						l_PF += fg_RxBuffer[19];
					}
					else
					{
						PFBYTE = fg_RxBuffer[16];
						fg_PF_Single_byte = 1;
					}
					l_result = ViewRequest(3,1,0,13,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) l_temp =100;
					else if(fg_RxBuffer[18]==0xFD) l_temp =1000;
					if(l_result)
					{
						LcdClear();
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_PF_Single_byte)
						{
							sprintf((_SBYTE*)g_DispBuffer, "System  PF = %g",(double)PFBYTE/l_temp);
							fg_PF_Single_byte=0;
						}
						else
						{
							sprintf((_SBYTE*)g_DispBuffer, "System PF = %g",(double)l_PF/l_temp);
						}
						HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1,0,13,7,0,255)");
					}
					break;
				//------------------------------------------------------------
				default:
					// goto nextScreen1;
					break;
			}
			//==================================================================================
			if(l_result == 1)
			{
				Disconnect();
			}
			HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
			HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
			Disconnect();
			l_Choice = _getkey();
			if(l_Choice == '0')
			return(1);
		}
	}
	else
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
		HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
		HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
		Disconnect();
		l_Choice = _getkey();
		if(l_Choice == '0')
		return(1);
	}
	goto PrevScreenB1;

nextScreenB1: LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)" METER PARAMETERS");
	HAL_DebugPrint(2, (_SBYTE*)"1.FREQUENCY");
	HAL_DebugPrint(3, (_SBYTE*)"2.POWER");
	HAL_DebugPrint(4, (_SBYTE*)"3.ENERGY");
	HAL_DebugPrint(5, (_SBYTE*)"4.NUMBER OF POWER FAILURE");
	HAL_DebugPrint(6, (_SBYTE*)"5.CUMULATIVE TAMPER COUNT");
	HAL_DebugPrint(7, (_SBYTE*)"6.BACK   0.NEXT");
	l_Choice = 0;
	while(( l_Choice > 0x36 )||(l_Choice < 0x30 ))  l_Choice = _getkey();
	if(l_Choice == '0')goto nextScreenB2;
	if(l_Choice == '6')goto PrevScreenB1;
	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
	LcdClear();
	rrr_c=0;
	rrr_c1=0;
	rrr_s=0;
	sss_c=0;
	sss_c1=0;
	sss_s=0;
	Disconnect();
	l_result = SetNormalResponseMode ();
	if(l_result == 1)
	{
		l_result =0;
		l_result = AssociationRequest();

		if(l_result == 1)
		{
			switch(l_Choice)
			{
				//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '1':
					l_result = ViewRequest(3,1,0,14,7,0,255,2);//frequency
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,14,7,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"Frequency=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.14.7.0.255)");
					}
					break;
				//----------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '2'://Power
					LcdClear();
					l_result = ViewRequest(3,1,0,9,7,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,1,0,9,7,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"APP. =", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.9.7.0.255)");
					}

					l_result = ViewRequest(3,1,0,1,7,0,255,2);
					if(fg_RxBuffer[15] == 0x05)
					{
						Sign_POWER = fg_RxBuffer[16];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[17];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[18];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[19];
					}
					l_result = ViewRequest(3,1,0,1,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) Sign_POWER /=100;
					else if(fg_RxBuffer[18]==0x02) Sign_POWER *=100;
					else if(fg_RxBuffer[18]==0x01) Sign_POWER *=10;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_RxBuffer[18] == 0)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%.3f KW",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18] == 1)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%.2f KW",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18] == 2)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%.1f KW",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18] == 3)
						{
							sprintf((_SBYTE*)g_DispBuffer, "ACT. =%g KW",(double)Sign_POWER/1000);
						}
						HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1,0,1,7,0,255)");
					}

					l_result = ViewRequest(3,1,0,3,7,0,255,2);
					if(fg_RxBuffer[15] == 0x05)
					{
						Sign_POWER = fg_RxBuffer[16];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[17];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[18];
						Sign_POWER *= 0x100;
						Sign_POWER += fg_RxBuffer[19];
					}
					l_result = ViewRequest(3,1,0,3,7,0,255,3);
					if(fg_RxBuffer[18]==0xFE) Sign_POWER /=100;
					else if(fg_RxBuffer[18]==0x02) Sign_POWER *=100;
					else if(fg_RxBuffer[18]==0x01) Sign_POWER *=10;
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						if(fg_RxBuffer[18]==0)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%.3f KVAR",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==1)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%.2f KVAR",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==2)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%.1f KVAR",(double)Sign_POWER/1000);
						}
						else if(fg_RxBuffer[18]==3)
						{
							sprintf((_SBYTE*)g_DispBuffer, "RACT.=%g KVAR",(double)Sign_POWER/1000);
						}
						HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(1,0,3,7,0,255)");
					}
					break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '3'://Energy
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE ENERGIES");
					l_result = ViewRequest(3,1,0,1,8,0,255,2);
					AP_POWER = fg_RxBuffer[16];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[17];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[18];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[19];
					l_result = ViewRequest(3,1,0,1,8,0,255,3);
					if(l_result)
					{
						Display_Value(2, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
						HAL_DebugPrint(3, (_SBYTE*)"(Imp)");
					}

					l_temp=0;
					l_result = ViewRequest(3,1,0,2,8,0,255,2);
					AP_POWER = fg_RxBuffer[16];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[17];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[18];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[19];
					l_result = ViewRequest(3,1,0,2,8,0,255,3);
					if(l_result)
					{
						Display_Value(4, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
						HAL_DebugPrint(5, (_SBYTE*)"(Exp)");
						HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
						_getkey();
					}

					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE ENERGIES");
					l_temp=0;
					l_result = ViewRequest(3,1,0,9,8,0,255,2);
					AP_POWER = fg_RxBuffer[16];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[17];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[18];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[19];
					l_result = ViewRequest(3,1,0,9,8,0,255,3);
					if(l_result)
					{
						Display_Value(2, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
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
					if(l_result)
					{
						Display_Value(4, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
						HAL_DebugPrint(5, (_SBYTE*)"(Exp)");
						HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
						_getkey();
					}
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE ENERGIES");
					l_result = ViewRequest(3,1,0,5,8,0,255,2);
					AP_POWER = fg_RxBuffer[16];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[17];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[18];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[19];
					l_result = ViewRequest(3,1,0,5,8,0,255,3);
					if(l_result)
					{
						Display_Value(2, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
						HAL_DebugPrint(3, (_SBYTE*)"Q1");
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
					if(l_result)
					{
						Display_Value(4, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
						HAL_DebugPrint(5, (_SBYTE*)"Q2");
						HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
						_getkey();
					}
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE ENERGIES");
					l_result = ViewRequest(3,1,0,7,8,0,255,2);
					AP_POWER = fg_RxBuffer[16];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[17];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[18];
					AP_POWER *= 0x100;
					AP_POWER +=  fg_RxBuffer[19];
					l_result = ViewRequest(3,1,0,7,8,0,255,3);
					if(l_result)
					{
						Display_Value(2, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
						HAL_DebugPrint(3, (_SBYTE*)"Q3");
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
					if(l_result)
					{
						Display_Value(4, (_SBYTE*)"", AP_POWER, (_SBYTE*)"");
						HAL_DebugPrint(5, (_SBYTE*)"Q4");
					}
					break;

				case '4':
					l_result = ViewRequest(1,0,0,96,7,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"NO. OF POWER FAILURE");
						Display_Value1(2, (_SBYTE*)"=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.96.7.0.255)");
					}
					else
					{
						HAL_DebugPrint(1, (_SBYTE*)"NOT AVAILABLE");
					}
				break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '5':	//
					l_result = ViewRequest(1,0,0,94,91,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE TAMPER");
						Display_Value1(2, (_SBYTE*)"COUNT=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.94.91.0,255)");
					}
				break;
				//-----------------------------------------------------------------
				case '6'://
					l_result = ViewRequest(3,0,0,94,91,8,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(3,0,0,94,91,8,255,3);
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE POWER");
						HAL_DebugPrint(2, (_SBYTE*)"FAILURE DURATION");
						Display_Value1(3, (_SBYTE*)"=", l_temp);
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(0.0.94.91.8.255)");
					}
					break;
				//-----------------------------------------------------------------
				default:
					//goto nextScreen2;
				break;
			}
			//==================================================================================
			if(l_result == 1)
			{
				Disconnect();
			}
			HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
			HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
			Disconnect();
			l_Choice = _getkey();
			if(l_Choice == '0')
			return(1);
		}
	}
	else
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
		HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
		HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
		Disconnect();
		l_Choice = _getkey();
		if(l_Choice == '0')
		return(1);
	}
	goto nextScreenB1;

nextScreenB2: LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)" METER PARAMETERS");
	HAL_DebugPrint(2, (_SBYTE*)"1.CUMUL.BILING COUNT");
	HAL_DebugPrint(3, (_SBYTE*)"2.CUMUL.PROGRAM.COUNT");
	HAL_DebugPrint(4, (_SBYTE*)"3.BILLING DATE");
	HAL_DebugPrint(5, (_SBYTE*)"4.MAXIMUM DEMAND");
	HAL_DebugPrint(6, (_SBYTE*)"6.BACK");
	l_Choice = 0;
	while(( l_Choice > 0x35 )||(l_Choice < 0x31 ))  l_Choice = _getkey();
	if(l_Choice == '6') goto nextScreenB1;
	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*)"METER READ...");
	LcdClear();
	rrr_c=0;
	rrr_c1=0;
	rrr_s=0;
	sss_c=0;
	sss_c1=0;
	sss_s=0;
	l_result = SetNormalResponseMode ();
	if(l_result == 1)
	{
		l_result =0;
		l_result = AssociationRequest();

		if(l_result == 1)
		{
			switch(l_Choice)
			{
				//--------------------------------------------------------------------------
				case '1':
					LcdClear();
					l_result = ViewRequest(1,0,0,0,1,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE BILLING");
						Display_Value1(2, (_SBYTE*)"COUNT=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.0.1.0.255)");
					}
					else
					{
						HAL_DebugPrint(3, (_SBYTE*)"NOT AVAILABLE");
					}
					break;
				//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '2'://
					l_result = ViewRequest(1,0,0,96,2,0,255,2);
					l_temp = Calculate_Value();
					if(l_result)
					{
						HAL_DebugPrint(1, (_SBYTE*)"CUMULATIVE PROGRAMING");
						Display_Value1(2, (_SBYTE*)"COUNT=", l_temp);
						HAL_DebugPrint(3, (_SBYTE*)"OBIS(0.0.96.2.0.255)");
					}
					break;
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '3':	//BILLING DATE
					l_result = ViewRequest(3,0,0,0,1,2,255,2);
					if(l_result)
					{
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						l_temp = fg_RxBuffer[17]*0x100 + fg_RxBuffer[18];
						HAL_DebugPrint(1, (_SBYTE*)"BILLING ");
						sprintf((_SBYTE*)g_DispBuffer, "Date:(DD/MM/YYYY)");
						HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "    :%02d/%02d/%02d",fg_RxBuffer[20],fg_RxBuffer[19],l_temp);
						HAL_DebugPrint(3, (_SBYTE*) g_DispBuffer);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "Time:(HH:MM:SS)");
						HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "    :%02d:%02d:%02d",fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24]);
						HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);
						HAL_DebugPrint(6, (_SBYTE*)"OBIS(0.0.0.1.2.255)");
					}
					else
					{
						HAL_DebugPrint(1, (_SBYTE*)"NOT AVAILABLE");
					}
					break;
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				case '4':	//md
					l_result = ViewRequest(4,1,0,1,6,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(4,1,0,1,6,0,255,3);
					if(l_result)
					{
						Display_Value(1, (_SBYTE*)"MAX.DEMAND=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(2, (_SBYTE*)"OBIS(1.0.1.6.0.255)");
					}
					else
					{
						HAL_DebugPrint(1, (_SBYTE*)"NOT AVAILABLE");
						Disconnect();
						break;
					}

					l_result = ViewRequest(4,1,0,9,6,0,255,2);
					l_temp = Calculate_Value();
					l_result = ViewRequest(4,1,0,9,6,0,255,3);
					if(l_result)
					{
						Display_Value(3, (_SBYTE*)"MAX.DEMAND=", l_temp, (_SBYTE*)"");
						HAL_DebugPrint(4, (_SBYTE*)"OBIS(1.0.9.6.0.255)");
					}
					else
					{
						HAL_DebugPrint(1, (_SBYTE*)"NOT AVAILABLE");
						Disconnect();
						break;
					}
					break;
				//------------------------------------------------------------
				default:
					//goto nextScreen2;
				break;
			}	//=============================================================================================================================
			if(l_result == 1)
			{
				Disconnect();
			}
			HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY TO");
			HAL_DebugPrint(3, (_SBYTE*)"CONTINUE");
			HAL_DebugPrint(4, (_SBYTE*)"PRESS 0 TO RETURN");
			Disconnect();
			l_Choice = _getkey();
			if(l_Choice == '0')
			return(1);
		}
	}
	else
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE.");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY TO");
		HAL_DebugPrint(4, (_SBYTE*)"CONTINUE");
		HAL_DebugPrint(5, (_SBYTE*)"PRESS 0 TO RETURN");
		Disconnect();
		l_Choice = _getkey();
		if(l_Choice == '0')
		return(1);
	}
	goto nextScreenB2;
}

//==============================================================
unsigned long Calculate_Value (void)
{
	unsigned long l_Temp;

	if(fg_RxBuffer[15] == 0x12)
	{

		l_Temp = fg_RxBuffer[16];
		l_Temp *= 0x100;
		l_Temp += fg_RxBuffer[17];
	}
	else if((fg_RxBuffer[15] == 0x06)||(fg_RxBuffer[15] == 0x05))//kaka
	{
		l_Temp = fg_RxBuffer[16];
		l_Temp *= 0x100;
		l_Temp += fg_RxBuffer[17];
		l_Temp *= 0x100;
		l_Temp += fg_RxBuffer[18];
		l_Temp *= 0x100;
		l_Temp += fg_RxBuffer[19];
	}
	return(l_Temp);
}
void Display_Value (_UBYTE a_LineNo, _SBYTE* a_PreMessage, double l_Value, _SBYTE* a_PostMessage)
{
	_SBYTE l_Temp1[50];
	_SBYTE l_Temp2[50];

	memset(l_Temp1, 0, sizeof(l_Temp1));
	memset(l_Temp2, 0, sizeof(l_Temp2));
	//---------Display value  and take place point----------------
	if( ( fg_RxBuffer[20] == 0x1B ) || ( fg_RxBuffer[20] == 0x1C ) || ( fg_RxBuffer[20] == 0x1D ) || \
		( fg_RxBuffer[20] == 0x1E ) || ( fg_RxBuffer[20] == 0x1F ) ||  (fg_RxBuffer[20] == 0x20 ))
	{
		l_Value = l_Value/1000;  //for change in to Kilo   divide 1000
	}

	if(fg_RxBuffer[18]==0xFF)
	{
		sprintf(l_Temp1, "%.1f", (double)l_Value/10);
	}
	else if(fg_RxBuffer[18]==0xFE)
	{
		sprintf(l_Temp1, "%.2f", (double)l_Value/100);
	}
	else if(fg_RxBuffer[18]==0xFD)
	{
		g_displayPoint = 6;
		sprintf(l_Temp1, "%.3f", (double)l_Value/1000);
	}
	else if(fg_RxBuffer[18]==0xFC)
	{
		g_displayPoint = 4;
		sprintf(l_Temp1, "%.4f", (double)l_Value/10000);
	}
	else if(fg_RxBuffer[18]==0xFB)
	{
		g_displayPoint = 5;
		sprintf(l_Temp1, "%.5f", (double)l_Value/100000);
	}
	else if(fg_RxBuffer[18]==0xFA)
	{
		g_displayPoint = 6;
		sprintf(l_Temp1, "%.6f", (double)l_Value/1000000);
	}

	else if((fg_RxBuffer[18] == 0x00) && (fg_VoltFlag != 1))
	{
		g_displayPoint = 3;
		sprintf(l_Temp1, "%.3f", (double)l_Value);
	}
    else if(fg_RxBuffer[18]==0x01)
	{
        g_displayPoint = 2;
		l_Value *= 10;
		sprintf(l_Temp1, "%.2f", (double)l_Value);
	}
	else if(fg_RxBuffer[18]==0x02)
	{
        g_displayPoint = 1;
		l_Value *= 100;
		sprintf(l_Temp1, "%.1f", (double)l_Value);
	}
	else if(fg_RxBuffer[18]==0x03)
	{
        g_displayPoint = 4;
		l_Value *= 1000;
		sprintf(l_Temp1, "%g", (double)l_Value);
	}
	else
	{
        if( ( fg_RxBuffer[20] == 0x1B ) || ( fg_RxBuffer[20] == 0x1C ) || ( fg_RxBuffer[20] == 0x1D ) || \
            ( fg_RxBuffer[20] == 0x1E ) || ( fg_RxBuffer[20] == 0x1F ) ||  (fg_RxBuffer[20] == 0x20 ))
        {
            g_displayPoint = 3;
            sprintf(l_Temp1, "%g", (double)l_Value);
        }
        else
        {
            g_displayPoint = 1;
            sprintf(l_Temp1, "%g", (double)l_Value);
        }
	}
	//--------------Display unit------------------------
    if(fg_RxBuffer[20] == 0x05) sprintf(l_Temp2," HOURS");
	else if(fg_RxBuffer[20] == 0x06) sprintf(l_Temp2," MINUTES");
	else if(fg_RxBuffer[20] == 0x07) sprintf(l_Temp2," SECONDS");
	else if(fg_RxBuffer[20] == 0x1B) sprintf(l_Temp2," KW");
	else if(fg_RxBuffer[20] == 0x1C) sprintf(l_Temp2," KVA");
	else if(fg_RxBuffer[20] == 0x1D) sprintf(l_Temp2," Kvar");
	else if(fg_RxBuffer[20] == 0x1E) sprintf(l_Temp2," KWh");
	else if(fg_RxBuffer[20] == 0x1F) sprintf(l_Temp2," KVAh");
	else if(fg_RxBuffer[20] == 0x20) sprintf(l_Temp2," Kvarh");
	else if(fg_RxBuffer[20] == 0x21) sprintf(l_Temp2," A");
	else if(fg_RxBuffer[20] == 0x23) sprintf(l_Temp2," V");
	else if(fg_RxBuffer[20] == 0x2C) sprintf(l_Temp2," Hz");
    fg_VoltFlag = 0;

    memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
    sprintf((_SBYTE*)g_DispBuffer, "%s%s%s%s",a_PreMessage, l_Temp1, l_Temp2, a_PostMessage);
    HAL_DebugPrint(a_LineNo, (_SBYTE*) g_DispBuffer);
}
//======================================================================================
void Display_Value1 (_UBYTE a_LineNo, _SBYTE* a_Message, double l_Value1)
{
	_SBYTE l_Temp1[50];
	_SBYTE l_Temp2[50];

	memset(l_Temp1, 0, sizeof(l_Temp1));
	memset(l_Temp2, 0, sizeof(l_Temp2));
	///-----------Display value  and take place point----------------------------
	if( ( fg_RxBuffer[20] == 0x1B ) || ( fg_RxBuffer[20] == 0x1C ) || ( fg_RxBuffer[20] == 0x1D ) || \
	( fg_RxBuffer[20] == 0x1E ) || ( fg_RxBuffer[20] == 0x1F ) ||  (fg_RxBuffer[20] == 0x20 ))
	{
		l_Value1 = l_Value1/1000;  //for change in to Kilo   divide 1000
	}
	if(fg_RxBuffer[18]==0xFF)
	{
		sprintf(l_Temp1, "%.1f",(double)l_Value1/10);
	}
	else if(fg_RxBuffer[18]==0xFE)
	{
		g_displayPoint=2;
		sprintf(l_Temp1, "%.2f",(double)l_Value1/100);
	}
	else if(fg_RxBuffer[18]==0xFD)
	{
		g_displayPoint=3;
		sprintf(l_Temp1, "%.3f",(double)l_Value1/1000);
	}
	else if(fg_RxBuffer[18]==0xFC)
	{
		g_displayPoint=4;
		sprintf(l_Temp1, "%.4f",(double)l_Value1/10000);
	}
	else if(fg_RxBuffer[18]==0xFB)
	{
		g_displayPoint=5;
		sprintf(l_Temp1, "%.5f",(double)l_Value1/100000);
	}
	else if(fg_RxBuffer[18]==0xFA)
	{
		g_displayPoint=6;
		sprintf(l_Temp1, "%.6f",(double)l_Value1/1000000);
	}
	else if(fg_RxBuffer[18]==0x03)
	{
		l_Value1 *= 1000;
		sprintf(l_Temp1, "%g",(double)l_Value1);
	}
	else if(fg_RxBuffer[18]==0x02)
	{
		l_Value1 *= 100;
		sprintf(l_Temp1, "%.1f",(double)l_Value1);
	}
	else if(fg_RxBuffer[18]==0x01)
	{
		l_Value1 *= 10;
		sprintf(l_Temp1, "%.2f",(double)l_Value1);
	}
	else
	{
        if( ( fg_RxBuffer[20] == 0x1B ) || ( fg_RxBuffer[20] == 0x1C ) || ( fg_RxBuffer[20] == 0x1D ) || \
            ( fg_RxBuffer[20] == 0x1E ) || ( fg_RxBuffer[20] == 0x1F ) ||  (fg_RxBuffer[20] == 0x20 ))
        {
            sprintf(l_Temp1, "%g",(double)l_Value1);
        }
        else
        {
            sprintf(l_Temp1, "%g",(double)l_Value1);
        }
	}
	//---------------Display unit --------------------------------
	if(fg_RxBuffer[20] == 0x05) sprintf(l_Temp2," HOURS");
	else if(fg_RxBuffer[20] == 0x06) sprintf(l_Temp2," MINUTES");
	else if(fg_RxBuffer[20] == 0x07) sprintf(l_Temp2," SECONDS");
	else if(fg_RxBuffer[20] == 0x1B) sprintf(l_Temp2," KW");
	else if(fg_RxBuffer[20] == 0x1C) sprintf(l_Temp2," KVA");
	else if(fg_RxBuffer[20] == 0x1D) sprintf(l_Temp2," Kvar");
	else if(fg_RxBuffer[20] == 0x1E) sprintf(l_Temp2," KWh");
	else if(fg_RxBuffer[20] == 0x1F) sprintf(l_Temp2," KVAh");
	else if(fg_RxBuffer[20] == 0x20) sprintf(l_Temp2," Kvarh");
	else if(fg_RxBuffer[20] == 0x21) sprintf(l_Temp2," A");
	else if(fg_RxBuffer[20] == 0x23) sprintf(l_Temp2," V");
	else if(fg_RxBuffer[20] == 0x2C) sprintf(l_Temp2," Hz");

	memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
	sprintf((_SBYTE*)g_DispBuffer, "%s%s%s",a_Message, l_Temp1, l_Temp2);
	HAL_DebugPrint(a_LineNo, (_SBYTE*) g_DispBuffer);
}
//======================================================================================
void Display_Signed_Value (_UBYTE a_LineNo, _SBYTE* a_Message, unsigned long l_Value)
{
    int l_int = 0;
    _SBYTE l_Temp1[50];
    _SBYTE l_Temp2[50];

    memset(l_Temp1, 0, sizeof(l_Temp1));
    memset(l_Temp2, 0, sizeof(l_Temp2));

	if(fg_RxBuffer[18]==0xFF)
	{
        l_int = (unsigned int)l_Value;
        if(fg_Sign)
            sprintf(l_Temp1, "%-.1f",(float)l_int/10);
        else
            sprintf(l_Temp1, "%.1f",(float)l_int/10);
	}
	else if(fg_RxBuffer[18] == 0xFE)
	{
        l_int = (unsigned int)l_Value;

        if(fg_Sign)
        {
            sprintf(l_Temp1, "-.2f",(float)l_int/100);
        }
        else
        {
            sprintf(l_Temp1, "%.2f",(float)l_int/100);
        }
	}
	else if(fg_RxBuffer[18]==0xFD)
	{
		l_int = (unsigned int)l_Value;
        if(fg_Sign)
            sprintf(l_Temp1, "%-.3f",(float)l_int/1000);
        else
            sprintf(l_Temp1, "%.3f",(float)l_int/1000);
	}
	else if(fg_RxBuffer[18]==0xFC)
	{
		l_int = (unsigned int)l_Value;
        if(fg_Sign)
            sprintf(l_Temp1, "%-.4f",(float)l_int/10000);
        else
            sprintf(l_Temp1, "%.4f",(float)l_int/10000);
	}

	else if(fg_RxBuffer[18]==0x00)
	{
		g_displayPoint=3;
		l_int = (unsigned int)l_Value;
        if(fg_Sign)
            sprintf(l_Temp1, "%-.3f",(float)l_int/1000);
        else
            sprintf(l_Temp1, "%.3f",(float)l_int/1000);

	}
    else if(fg_RxBuffer[18]==0x01)
	{
        g_displayPoint=2;
		l_Value *= 10;
		l_int = (unsigned int)l_Value;
        if(fg_Sign)
            sprintf(l_Temp1, "%-.2f",(float)l_int/1000);
        else
            sprintf(l_Temp1, "%.2f",(float)l_int/1000);
	}
	else if(fg_RxBuffer[18]==0x02)
	{
        g_displayPoint=1;
		l_Value *= 100;
		l_int = (unsigned int)l_Value;
        if(fg_Sign)
            sprintf(l_Temp1, "%-.1f",(float)l_int/1000);
        else
            sprintf(l_Temp1, "%.1f",(float)l_int/1000);
	}
	else if(fg_RxBuffer[18]==0x03)
	{
        g_displayPoint=4;
		l_Value *= 1000;
		l_int = (unsigned int)l_Value;
        if(fg_Sign)
            sprintf(l_Temp1, "%-g",(float)l_int/1000);
        else
            sprintf(l_Temp1, "%g",(float)l_int/1000);
	}
	else
	{
        if( ( fg_RxBuffer[20] == 0x1B ) || ( fg_RxBuffer[20] == 0x1C ) || ( fg_RxBuffer[20] == 0x1D ) || \
            ( fg_RxBuffer[20] == 0x1E ) || ( fg_RxBuffer[20] == 0x1F ) ||  (fg_RxBuffer[20] == 0x20 ))
        {
            g_displayPoint=3;
            l_int = (unsigned int)l_Value;
            if(fg_Sign)
                sprintf(l_Temp1, "%-g",(float)l_int/1000);
            else
                sprintf(l_Temp1, "%g",(float)l_int/1000);
        }
        else
        {
            g_displayPoint=1;
            l_int = (unsigned int)l_Value;
            if(fg_Sign)
            sprintf(l_Temp1, "%-g",(float)l_int);
            else
            sprintf(l_Temp1, "%g",(float)l_int);
        }
	}
	//---------------Display unit ------------------------
    if(fg_RxBuffer[20] == 0x05) sprintf(l_Temp2," HOURS");
	else if(fg_RxBuffer[20] == 0x06) sprintf(l_Temp2," MINUTES");
	else if(fg_RxBuffer[20] == 0x07) sprintf(l_Temp2," SECONDS");
	else if(fg_RxBuffer[20] == 0x1B) sprintf(l_Temp2," KW");
	else if(fg_RxBuffer[20] == 0x1C) sprintf(l_Temp2," KVA");
	else if(fg_RxBuffer[20] == 0x1D) sprintf(l_Temp2," Kvar");
	else if(fg_RxBuffer[20] == 0x1E) sprintf(l_Temp2," KWh");
	else if(fg_RxBuffer[20] == 0x1F) sprintf(l_Temp2," KVAh");
	else if(fg_RxBuffer[20] == 0x20) sprintf(l_Temp2," Kvarh");
	else if(fg_RxBuffer[20] == 0x21) sprintf(l_Temp2," A");
	else if(fg_RxBuffer[20] == 0x23) sprintf(l_Temp2," V");
	else if(fg_RxBuffer[20] == 0x2C) sprintf(l_Temp2," Hz");

   memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
   sprintf((_SBYTE*)g_DispBuffer, "%s%s%s",a_Message, l_Temp1, l_Temp2);
   HAL_DebugPrint(a_LineNo, (_SBYTE*) g_DispBuffer);
}
//======================================================================================



