#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <linux/rtc.h>

#include "Config.h"
#include "global.h"
#include "FUNCTH.h"
#include "AES128.h"
#include "auth.h"
#include "comm.h"
#include "Program.h"
#include "display.h"
#include "hhuLcd.h"
#include "utility.h"

#if 1

static unsigned char MDIntg_Prd,Billing_Date,Profi_Cap_Prd,Tariff1,Tariff2,Tariff3,Tariff4,Tariff5,Tariff6,Tariff7,Tariff8;
static unsigned char TodHr1,TodHr2,TodHr3,TodHr4,TodHr5,TodHr6,TodHr7,TodHr8,TodMin1,TodMin2,TodMin3,TodMin4,TodMin5,TodMin6,TodMin7,TodMin8;
static unsigned char MDScriptId1,MDScriptId2,MDScriptId3,MDScriptId4,MDScriptId5,MDScriptId6,MDScriptId7,MDScriptId8;
static unsigned int Physical_Address,TodActYr,TodActMonth,TodActDate,TodActHr,TodActMin;

Aes128 Key;
FILE *fg_pFile;
FILE *fg_pFile1;
FILE *stream_r1;

void FrameType(_UBYTE *cp, _UBYTE &rrr_c, _UBYTE &rrr_c1, _UBYTE &rrr_s, _UBYTE &sss_c, _UBYTE &sss_c1, _UBYTE &sss_s);
int FlagCheck(int _Choice)
{
	switch(_Choice)
	{
		case 1:
			if(RTC_Flag)
			return(1);
		break;
		case 2:
			if(MD_Int_Flag)
			return(1);
		break;
		case 3:
			if(BillingDateFlag)
			return(1);
		break;
		case 4:
			if(Profile_cap_Flag)
			return(1);
		break;
		case 5:
			if(Tod_Flag)
			return(1);
		break;
		case 6:
			if(MD_Reser_Flag)
			return(1);
		break;
		case 7:
			if(Phy_add_Flag)
			return(1);
		break;
	}
	return(0);
}
int AuthenticateVerification(void)
{
	int cc1=0,i,j;
	unsigned char ret_lsh=0;
	fg_SendBuffer[0] = 0x7E;//HDLC Frame Opening Flag 
	fg_SendBuffer[1] = 0xA0;//HDLC Frame TYPE 3
	fg_SendBuffer[3] = 0x03;//destination address
	fg_SendBuffer[4] = g_UserAssociation;//source address
	
	ret_lsh = (rrr_s << 5);
	fg_SendBuffer[5] = (ret_lsh | 0x10);
	ret_lsh = sss_s << 1;
	fg_SendBuffer[5] = ret_lsh | fg_SendBuffer[5];

	fg_SendBuffer[6] = 0x00;//fcs
	fg_SendBuffer[7] = 0x00;//fcs
	fg_SendBuffer[8] = 0xE6;//E6 E6 00 LLC Bytes
	fg_SendBuffer[9] = 0xE6;
	fg_SendBuffer[10] = 0x00;
	fg_SendBuffer[11] = 0xC3;
	fg_SendBuffer[12] = 0x01;
	fg_SendBuffer[13] = 0xC1;
	fg_SendBuffer[14] = 0x00;

	fg_SendBuffer[15] = 0x0F;
	fg_SendBuffer[16] = 0x00;
	fg_SendBuffer[17] = 0x00 ;
	fg_SendBuffer[18] = 0x28 ;
	fg_SendBuffer[19] = 0x00 ;
	fg_SendBuffer[20] = 0x00 ;
	fg_SendBuffer[21] = 0xff ;

	fg_SendBuffer[22] = 0x01 ;
	fg_SendBuffer[23] = 0x01 ;
	fg_SendBuffer[24] = 0x09 ;
	fg_SendBuffer[25] = 0x10 ;
	
	cc1=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			fg_SendBuffer[26+cc1] = state[j][i] ;
			cc1++;
		}
	}
	fg_SendBuffer[2] = 0x2B;//no. of bytes between flag
	fg_SendBuffer[44] = 0x7e;//Frame closing Flag
	fcs(fg_SendBuffer+1, 5, 1);
	fcs(fg_SendBuffer+1, 41, 1);
	SendPkt(45);
	Read(fg_RxBuffer);

	if(fg_RxBuffer[14] == 0x00)//later change mundel
	{
		FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
		return 1;
	}
	else
	{
		return 0;
	}
	
}
int MeterProgramming(void)
{
	int Result,ParaMeter;

	while(1)
	{	
		ParaMeter=9;
		rrr_c=0;
		rrr_c1=0;
		rrr_s=0;
		sss_c=0;
		sss_c1=0;
		sss_s=0;
		g_UserAssociation = US_ASSOCIATION;
		LcdClear();

              
kuldeep:ParaMeter = ProgrammingScr();
		if(!ParaMeter)
		return(1);
		if(!FlagCheck(ParaMeter))
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*) "AUTHENTICATE REQURED");
			HAL_DebugPrint(2, (_SBYTE*) "PRESS ANY KEY TO");
			HAL_DebugPrint(3, (_SBYTE*) "CONTINUE");
			_getkey();
			goto kuldeep;
		}
        LcdClear();
        HAL_DebugPrint(1, (_SBYTE*) "PROGRAMMING START...");
		//Disconnect();
		Result =0;
		Result = SetNormalResponseMode ();
		if(Result == 1) 
		{
			Result =0;
			Result = AssociationRequest();
			if(Result == 1)
			{
				Key.Encryption128();
				Result =0;
				Result = AuthenticateVerification();
				if(Result == 1)
				{	
					Result =0;
					if(!ProgParaRead(ParaMeter))
					goto disconnect;
					switch(ParaMeter)
					{	//=========================================================================================================
						case METER_RTC_SETTING:
							Result = ProgParaSet(ParaMeter,8,0,0,1,0,0,255,2);
							if(Result)
							{
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*) "RTC ");
							}
						break;
						//---------------------------------------------------------------------------------------------------------------
						case DEMAND_INTEGRATION_PERIOD:
							Result = ProgParaSet(ParaMeter,1,1,0,0,8,0,255,2);
							if(Result)
							{
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*) "DEMAND INTGRATION");
								HAL_DebugPrint(2, (_SBYTE*) "PERIOD");
							}
						break;
						//-----------------------------------------------------------------------------------------------------------------
						case BILLING_DATE:
							// if(type == 2)// remove as per said by harvinder and sumeet    DLMS CMRI used in JdVVNL 1188 Tender (CAT B)

							// {
								//clrscr();
								//printf("CATAGARY B BILLING DATE \n");
								// break;
							// }
							Result = ProgParaSet(ParaMeter,22,0,0,15,0,0,255,4);
							if(Result)
							{
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*) "BILLING DATE");
							}
						break;
						//----------------------------------------------------------------------------------------------------------------
						case PROFILE_CAPTURE_PERIOD:
							Result = ProgParaSet(ParaMeter,1,1,0,0,8,4,255,2);
							if(Result)
							{
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*) "PROFILE CAPTURE");
								HAL_DebugPrint(2, (_SBYTE*) "PERIOD");
							}
						break;
						//------------------------------------------------------------------------------------------------------------------
						case TOD_TIMING:
                       		// if(type == 2)// remove as per said by harvinder and sumeet    DLMS CMRI used in JdVVNL 1188 Tender (CAT B)
							// {
								//clrscr();
								//printf("CATAGARY B BILLING DATE \n");
								// break;
							// }
							Result = ProgParaSet(ParaMeter,20,0,0,13,0,0,255,9);
							if(Result)
							{
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*) "TOD TIMING");
							}
						break;
						//-------------------------------------------------------------------------------------------------------------------
						case MD_RESET:                    
							Result = ProgParaSet(ParaMeter,9,0,0,10,0,1,255,1);
							if(Result)
							{
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*) "MD RESET SUCCESS");
							}
						break;
						//-------------------------------------------------------------------------------------------------------------------
						case PHYSICAL_ADDRESS_CHANGE:
							Result = ProgParaSet(ParaMeter,23,0,0,22,0,0,255,9);
							if(Result)
							{
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*) "PHYSICAL ADDRESS");
							}
						break;
					}	//==========================================================================================================
disconnect:			Disconnect();
					//ErrPrt(1,4,"END TIME          "); sys_Time(5);
					if(Result)
					{
						if(ParaMeter != MD_RESET)
						HAL_DebugPrint(4, (_SBYTE*) "SUCCESS SETTING");
						HAL_DebugPrint(5, (_SBYTE*) "PRESS ANY KEY..");
						_getkey();
						//return(1);
					}
					else
					{
						LcdClear();
                        if(type == 2)
                        {
                        	HAL_DebugPrint(4, (_SBYTE*) "NOT AVAILABLE FOR");
                        	HAL_DebugPrint(5, (_SBYTE*) "CATAGARY B");
                        }
                        else
                        {
                        	HAL_DebugPrint(4, (_SBYTE*) "PARA.NOT SETTING");
                        	HAL_DebugPrint(5, (_SBYTE*) "PRESS ANY KEY..");
                        }
						_getkey();
						//return(1);
					}
				}
				else
				{
					Disconnect();
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*) "AUTHENTICATE REQURED");
					HAL_DebugPrint(2, (_SBYTE*) "PRESS ANY KEY TO");
					HAL_DebugPrint(3, (_SBYTE*) "CONTINUE");
					_getkey();
					//return(1);
				}	
			}
			else
			{
				LcdClear();
				HAL_DebugPrint(1, (_SBYTE*) "ASSOCIATION FAIL");
				HAL_DebugPrint(2, (_SBYTE*) "PRESS ANY KEY TO");
				HAL_DebugPrint(3, (_SBYTE*) "CONTINUE");
				HAL_DebugPrint(4, (_SBYTE*) "PRESS 0 TO RETURN");
				ParaMeter = _getkey();
				if(ParaMeter == '0')
				{
					return(1);
				}
			}
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*) "COMMUNICATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*) "CHECK CABLE.");
			HAL_DebugPrint(3, (_SBYTE*) "PRESS ANY KEY TO");
			HAL_DebugPrint(4, (_SBYTE*) "CONTINUE");
			HAL_DebugPrint(5, (_SBYTE*) "PRESS 0 TO RETURN");
			ParaMeter = _getkey();
			if(ParaMeter == '0')
			{
				return(1);
			}
		}
	}//while loop

	return 0;
}

int ProgParaSet(int ParaMeter,unsigned char classID,unsigned char Obis_a,unsigned char Obis_b,unsigned char Obis_c,unsigned char Obis_d,unsigned char Obis_e,unsigned char Obis_f,unsigned char Attr)
{
	struct rtc_time l_Rtc;
	_UBYTE ret_lsh,l_RetVal = 0x00;
	int c = 0;
	unsigned int temp;
	ClearPacket();
	fg_SendBuffer[c++] = 0x7E;
	fg_SendBuffer[c++] = 0xA0;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x03;
	fg_SendBuffer[c++] = g_UserAssociation;
	temp = c++;
	ret_lsh = (rrr_s << 5);
	fg_SendBuffer[temp] = (ret_lsh | 0x10);
	ret_lsh = sss_s << 1;
	fg_SendBuffer[temp] = ret_lsh | fg_SendBuffer[temp];

	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = 0xE6;
	fg_SendBuffer[c++] = 0xE6;
	fg_SendBuffer[c++] = 0x00;
	if(ParaMeter == MD_RESET )
	fg_SendBuffer[c++] = 0xC3 ;
	else
	fg_SendBuffer[c++] = 0xC1;
	if(ParaMeter == TOD_TIMING)
	fg_SendBuffer[c++] = 0x02;
	else
	fg_SendBuffer[c++] = 0x01;
	fg_SendBuffer[c++] = 0x81;
	fg_SendBuffer[c++] = 0x00;
	fg_SendBuffer[c++] = classID;
	fg_SendBuffer[c++] = Obis_a;
	fg_SendBuffer[c++] = Obis_b;
	fg_SendBuffer[c++] = Obis_c;
	fg_SendBuffer[c++] = Obis_d;
	fg_SendBuffer[c++] = Obis_e;
	fg_SendBuffer[c++] = Obis_f;
	fg_SendBuffer[c++] = Attr;
	if(ParaMeter == MD_RESET )
	fg_SendBuffer[c++] = 0x01 ;
	else
	fg_SendBuffer[c++] = 0x00 ;
	
	switch(ParaMeter)
	{	//=======================================================================
		case METER_RTC_SETTING:
			UTL_RTCRead (&l_Rtc);		//Read RTC of HHU
			fg_SendBuffer[c++] = 0x09;
			fg_SendBuffer[c++] = 0x0C;
			fg_SendBuffer[c++] = ((l_Rtc.tm_year+1900) / 256);
			fg_SendBuffer[c++] = (_UBYTE) ((l_Rtc.tm_year+1900) % 256);
			fg_SendBuffer[c++] = (_UBYTE) l_Rtc.tm_mon+1;
			fg_SendBuffer[c++] = (_UBYTE) l_Rtc.tm_mday;
			fg_SendBuffer[c++] = 0xFF;//
			fg_SendBuffer[c++] = (_UBYTE) l_Rtc.tm_hour;
			fg_SendBuffer[c++] = (_UBYTE) l_Rtc.tm_min;
			fg_SendBuffer[c++] = (_UBYTE) l_Rtc.tm_sec;
			fg_SendBuffer[c++] = 0x00;//hundredth of second
			fg_SendBuffer[c++] = 0x80;//not specified
			fg_SendBuffer[c++] = 0x00;
			fg_SendBuffer[c++] = 0x00;//
		break;
		//--------------------------------------------------------------------------------
		case DEMAND_INTEGRATION_PERIOD:
			temp = MDIntg_Prd;
			temp *=60;
			fg_SendBuffer[c++] = 0x12 ;
			fg_SendBuffer[c++] = (unsigned char )(temp >> 8) ;
			fg_SendBuffer[c++] = (unsigned char )temp ;

		break;
		//--------------------------------------------------------------------------------
		case BILLING_DATE:
			fg_SendBuffer[c++] = 0x01 ;
			fg_SendBuffer[c++] = 0x01;
			fg_SendBuffer[c++] = 0x02 ;
			fg_SendBuffer[c++] = 0x02 ;
			
			fg_SendBuffer[c++] = 0x09 ;
			fg_SendBuffer[c++] = 0x04 ;
			fg_SendBuffer[c++] = 0 ;
			fg_SendBuffer[c++] = 0 ;
			fg_SendBuffer[c++] = 0 ;
			fg_SendBuffer[c++] = 0 ;
			fg_SendBuffer[c++] = 0x09 ;
			fg_SendBuffer[c++] = 0x05 ;
			fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = Billing_Date;
			fg_SendBuffer[c++] = 0xFF;
		break;
		//--------------------------------------------------------------------------------
		case PROFILE_CAPTURE_PERIOD:
			temp = Profi_Cap_Prd;
			temp *=60;
			fg_SendBuffer[c++] = 0x12 ;
			fg_SendBuffer[c++] = (unsigned char )(temp >> 8) ;
			fg_SendBuffer[c++] = (unsigned char )temp ;
		
		break;
		//--------------------------------------------------------------------------------
		case TOD_TIMING:
			
			fg_SendBuffer[c++] = 0x00;//It show not last block
			fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x01;//It show 1st Block
			fg_SendBuffer[c++] = 0x81;//No of byte in next single byte
			fg_SendBuffer[c++] = 0xB0;//data in all block
			fg_SendBuffer[c++] = 0x01;//array
			fg_SendBuffer[c++] = 0x01;// one element
			fg_SendBuffer[c++] = 0x02;
			fg_SendBuffer[c++] = 0x02;
			fg_SendBuffer[c++] = 0x11;
			fg_SendBuffer[c++] = 0x01;
			fg_SendBuffer[c++] = 0x01;
			fg_SendBuffer[c++] = 0x10;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr1;fg_SendBuffer[c++] = TodMin1;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff1;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr2;fg_SendBuffer[c++] = TodMin2;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff2;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr3;fg_SendBuffer[c++] = TodMin3;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff3;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr4;fg_SendBuffer[c++] = TodMin4;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff4;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			
			fg_SendBuffer[1] |= 0x08;
			fg_SendBuffer[2] = c + 1;
			fg_SendBuffer[c+2] = 0x7E;
			fcs(fg_SendBuffer+1, 5, 1);
			fcs(fg_SendBuffer+1,c-1, 1);
			SendPkt(c+3);
			l_RetVal = Read(fg_RxBuffer);
			if(!l_RetVal)
			{
				return(0);
			}
			//if(fg_RxBuffer[2] == 0xA0)
			{
				FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
			}
			ClearPacket();c = 0;
			fg_SendBuffer[c++] = 0x7E;
			fg_SendBuffer[c++] = 0xA0;
			fg_SendBuffer[c++] = 0x51;
			fg_SendBuffer[c++] = 0x03;
			fg_SendBuffer[c++] = g_UserAssociation;
			temp = c++;
			ret_lsh = (rrr_s << 5);
			fg_SendBuffer[temp] = (ret_lsh | 0x10);
			ret_lsh = sss_s << 1;
			fg_SendBuffer[temp] = ret_lsh | fg_SendBuffer[temp];
			
			fg_SendBuffer[c++] = 0x44;
			fg_SendBuffer[c++] = 0x48;
			fg_SendBuffer[c++] = TodHr5;fg_SendBuffer[c++] = TodMin5;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff5;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr6;fg_SendBuffer[c++] = TodMin6;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff6;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr7;fg_SendBuffer[c++] = TodMin7;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff7;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr8;fg_SendBuffer[c++] = TodMin8;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = Tariff8;
			
			fg_SendBuffer[2] = c + 1;
			fg_SendBuffer[c+2] = 0x7E;
			fcs(fg_SendBuffer+1, 5, 1);
			fcs(fg_SendBuffer+1,c-1, 1);
			SendPkt(c+3);
			l_RetVal = Read(fg_RxBuffer);
			if(!l_RetVal)
			{
				return(0);
			}
			//if(fg_RxBuffer[14] == 0x00)
			{
				FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
			}
			ClearPacket();c = 0;
			fg_SendBuffer[c++] =  0x7E;
			fg_SendBuffer[c++] =  0xA8;
			fg_SendBuffer[c++] =  0x7A;
			fg_SendBuffer[c++] =  0x03;
			fg_SendBuffer[c++] = g_UserAssociation;
			temp = c++;
			ret_lsh = (rrr_s << 5);
			fg_SendBuffer[temp] = (ret_lsh | 0x10);
			ret_lsh = sss_s << 1;
			fg_SendBuffer[temp] = ret_lsh | fg_SendBuffer[temp];
			
			fg_SendBuffer[c++] =  0x5E;
			fg_SendBuffer[c++] =  0x95;
			fg_SendBuffer[c++] =  0xE6;
			fg_SendBuffer[c++] =  0xE6;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0xC1;fg_SendBuffer[c++] =  0x03;
			fg_SendBuffer[c++] =  0x81;
			fg_SendBuffer[c++] =  0x01;//It show last block;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0x02;//It show 2 nd block;
			fg_SendBuffer[c++] =  0x81;//no of byte in single digit
			fg_SendBuffer[c++] =  0x88;//no of bytes
			fg_SendBuffer[c++] =  0x02;fg_SendBuffer[c++] =  0x03;fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x04;
			fg_SendBuffer[c++] =  TodHr1;fg_SendBuffer[c++] =  TodMin1;fg_SendBuffer[c++] =  0xFF;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x06;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x0A;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x64;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x12;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  MDScriptId1;
			fg_SendBuffer[c++] =  0x02;fg_SendBuffer[c++] =  0x03;fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x04;
			fg_SendBuffer[c++] =  TodHr2;fg_SendBuffer[c++] =  TodMin2;fg_SendBuffer[c++] =  0xFF;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x06;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x0A;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x64;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x12;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  MDScriptId2;
			fg_SendBuffer[c++] =  0x02;fg_SendBuffer[c++] =  0x03;fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x04;
			fg_SendBuffer[c++] =  TodHr3;fg_SendBuffer[c++] =  TodMin3;fg_SendBuffer[c++] =  0xFF;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x06;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x0A;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x64;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x12;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  MDScriptId3;
			fg_SendBuffer[c++] =  0x02;fg_SendBuffer[c++] =  0x03;fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x04;
			fg_SendBuffer[c++] =  TodHr4;fg_SendBuffer[c++] =  TodMin4;fg_SendBuffer[c++] =  0xFF;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x06;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x0A;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x64;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x12;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  MDScriptId4;
			fg_SendBuffer[c++] =  0x02;fg_SendBuffer[c++] =  0x03;fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x04;
			fg_SendBuffer[c++] =  TodHr5;fg_SendBuffer[c++] =  TodMin6;fg_SendBuffer[c++] =  0xFF;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x06;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x0A;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  0x64;fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x12;fg_SendBuffer[c++] =  0x00;fg_SendBuffer[c++] =  MDScriptId5;
			fg_SendBuffer[c++] =  0x02;fg_SendBuffer[c++] =  0x03;fg_SendBuffer[c++] =  0x09;fg_SendBuffer[c++] =  0x04;
			fg_SendBuffer[c++] =  TodHr6;

			fg_SendBuffer[2] = c + 1;
			fg_SendBuffer[c+2] = 0x7E;
			fcs(fg_SendBuffer+1, 5, 1);
			fcs(fg_SendBuffer+1,c-1, 1);
			SendPkt(c+3);
			l_RetVal = Read(fg_RxBuffer);
			if(!l_RetVal)
			{
				return(0);
			}
			//if(fg_RxBuffer[2] == 0xA0)
			{
				FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
			}
			ClearPacket();c = 0;
			fg_SendBuffer[c++] = 0x7E;
			fg_SendBuffer[c++] = 0xA0;
			fg_SendBuffer[c++] = 0x3D;
			fg_SendBuffer[c++] = 0x03;
			fg_SendBuffer[c++] = g_UserAssociation;
			temp = c++;
			ret_lsh = (rrr_s << 5);
			fg_SendBuffer[temp] = (ret_lsh | 0x10);
			ret_lsh = sss_s << 1;
			fg_SendBuffer[temp] = ret_lsh | fg_SendBuffer[temp];
			
			fg_SendBuffer[c++] = 0x44;
			fg_SendBuffer[c++] = 0x48;
									  fg_SendBuffer[c++] = TodMin6;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = MDScriptId6;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr7;fg_SendBuffer[c++] = TodMin7;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = MDScriptId7;
			fg_SendBuffer[c++] = 0x02;fg_SendBuffer[c++] = 0x03;fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x04;
			fg_SendBuffer[c++] = TodHr8;fg_SendBuffer[c++] = TodMin8;fg_SendBuffer[c++] = 0xFF;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x09;fg_SendBuffer[c++] = 0x06;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x0A;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = 0x64;fg_SendBuffer[c++] = 0xFF;
			fg_SendBuffer[c++] = 0x12;fg_SendBuffer[c++] = 0x00;fg_SendBuffer[c++] = MDScriptId8;
			
			fg_SendBuffer[2] = c + 1;
			fg_SendBuffer[c+2] = 0x7E;
			fcs(fg_SendBuffer+1, 5, 1);
			fcs(fg_SendBuffer+1,c-1, 1);
			SendPkt(c+3);
			l_RetVal = Read(fg_RxBuffer);
			if(!l_RetVal)
			{
				return(0);
			}
			//if(fg_RxBuffer[14] == 0x00)
			{
				FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
			}
			ClearPacket();c = 0;
			fg_SendBuffer[c++] = 0x7E;
			fg_SendBuffer[c++] = 0xA0;
			fg_SendBuffer[c++] = 0x27;
			fg_SendBuffer[c++] = 0x03;
			fg_SendBuffer[c++] = g_UserAssociation;
			temp = c++;
			ret_lsh = (rrr_s << 5);
			fg_SendBuffer[temp] = (ret_lsh | 0x10);
			ret_lsh = sss_s << 1;
			fg_SendBuffer[temp] = ret_lsh | fg_SendBuffer[temp];
			
			fg_SendBuffer[c++] = 0x44;
			fg_SendBuffer[c++] = 0x48;
			fg_SendBuffer[c++] =  0xE6;
			fg_SendBuffer[c++] =  0xE6;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0xC1;fg_SendBuffer[c++] =  0x01;
			fg_SendBuffer[c++] =  0x81;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0x14; //obis code
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0x0D;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0x00;
			fg_SendBuffer[c++] =  0xFF;
			fg_SendBuffer[c++] =  0x0A;
			fg_SendBuffer[c++] =  0x00;
			
		//   TOd activation date   
			fg_SendBuffer[c++] = 0x09;
			fg_SendBuffer[c++] = 0x0C;
			fg_SendBuffer[c++] = (TodActYr / 256);
			fg_SendBuffer[c++] = (_UBYTE) (TodActYr % 256);
			fg_SendBuffer[c++] = (_UBYTE) TodActMonth;
			fg_SendBuffer[c++] = (_UBYTE) TodActDate;
			fg_SendBuffer[c++] = 0xFF;//
			fg_SendBuffer[c++] = (_UBYTE) TodActHr;
			fg_SendBuffer[c++] = (_UBYTE) TodActMin;
			fg_SendBuffer[c++] = 0x00;//second
			fg_SendBuffer[c++] = 0x00;//hundredth of second
			fg_SendBuffer[c++] = 0x80;//not specified
			fg_SendBuffer[c++] = 0x00;
			fg_SendBuffer[c++] = 0x00;//

		break;
		//--------------------------------------------------------------------------------
		case MD_RESET:
			fg_SendBuffer[c++] = 0x12 ;
			fg_SendBuffer[c++] = 0x00 ;
			fg_SendBuffer[c++] = 0x00;
		
		break;
		//--------------------------------------------------------------------------------
		case PHYSICAL_ADDRESS_CHANGE:
			fg_SendBuffer[c++] = 0x12 ;
			fg_SendBuffer[c++] = (unsigned char )(Physical_Address >> 8) ;
			fg_SendBuffer[c++] = (unsigned char )Physical_Address ;
		break;
		//--------------------------------------------------------------------------------
		default:
		
		break;
	}	//===================================================================
	fg_SendBuffer[2] = c + 1;
	fg_SendBuffer[c+2] = 0x7E;

	fcs(fg_SendBuffer+1, 5, 1);
	fcs(fg_SendBuffer+1,c-1, 1);

	SendPkt(c+3);
	
	l_RetVal = Read(fg_RxBuffer);
	if(!l_RetVal)
	{
		return(0);
	}
	// if(fg_RxBuffer[14] == 0x00)
	// {
		// FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
		// return 1;
	// }
	if((fg_RxBuffer[14] == 0x00)||(fg_RxBuffer[14] == 0x01))
	{
		FrameType(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
		if(fg_RxBuffer[14] == 0x01)
		return 0;
		return 1;
	}
	
	return(0);
}
int ProgParaRead(int ParaMeter)
{
	unsigned char srno1[10],srno2[10],chr[30],NoOftod;
	long x=0,L_Serialno;
	L_Serialno = Serialno;
	x = 7;
	while((L_Serialno>0)||(x > 2))
	{
		srno1[x] = 0x30 + L_Serialno%10;
		L_Serialno/=10;
		x--;
	}
	if(x == 1)
	{
		srno1[0] = 0x20;
		srno1[1] = 0x20;
	}
	else if(x == 0)
	{
		srno1[0] = 0x20;
	}
	srno1[8] ='\0';
	switch(ParaMeter)
	{	
		//=======================================================================
		case METER_RTC_SETTING:
		
		break;
		//--------------------------------------------------------------------------------
		case DEMAND_INTEGRATION_PERIOD: case PROFILE_CAPTURE_PERIOD:case BILLING_DATE:case PHYSICAL_ADDRESS_CHANGE:
			
			MDIntg_Prd = Profi_Cap_Prd = Billing_Date = Physical_Address = 0;
			stream_r1=fopen(FILE_PROG_TXT, "r+");
			fseek(stream_r1,0,SEEK_END); 
			x = ftell(stream_r1);
			if(x == -1)//
			{
				goto MDnext;
			}
			fseek(stream_r1,0,SEEK_SET);
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = '\0'; 
			MDIntg_Prd = atoi((const char*)chr);
			fgetc(stream_r1);//for read  ','
			
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = '\0'; 
			Billing_Date = atoi((const char*)chr);
			fgetc(stream_r1);//for read  ','
			
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = '\0'; 
			Profi_Cap_Prd = atoi((const char*)chr);
			fgetc(stream_r1);//for read  ','
			
			chr[0] = fgetc(stream_r1);//16 T0 16381
			chr[1] = fgetc(stream_r1);
			chr[2] = fgetc(stream_r1);
			chr[3] = fgetc(stream_r1);
			chr[4] = fgetc(stream_r1);
			chr[5] = '\0'; 
			Physical_Address = atoi((const char*)chr);
			
MDnext:		fclose(stream_r1);

		break;
		//--------------------------------------------------------------------------------
		case TOD_TIMING:
			TodHr1 = TodHr2 = TodHr3 = TodHr4 = TodHr5 = TodHr6 = TodHr7 = TodHr8 = 0xFF;
			TodMin1 = TodMin2 = TodMin3 = TodMin4 = TodMin5 = TodMin6 = TodMin7 = TodMin8 = 0xFF;
			MDScriptId1 = MDScriptId2 = MDScriptId3 = MDScriptId4 = MDScriptId5 = MDScriptId6 = MDScriptId7 = MDScriptId8 =0;
			Tariff1 = Tariff2 = Tariff3 = Tariff4 = Tariff5 = Tariff6 = Tariff7 = Tariff8 =0;
			TodActYr = TodActMonth = TodActDate = TodActHr = TodActMin = 0xFF;
			stream_r1=fopen(FILE_TOD_TXT,"r+");
			fseek(stream_r1,0,SEEK_END); 
			x = ftell(stream_r1);
			if(x == -1)//
			{
				LcdClear();
				HAL_DebugPrint(1, (_SBYTE*) "TOD TIMING NOT SET");
				//_getkey();
				fclose(stream_r1);
				return(0);
			}
			fseek(stream_r1,0,SEEK_SET); 
			chr[0] = fgetc(stream_r1);
			chr[1] = '\0'; 
			NoOftod = atoi((const char*)chr);
			fgetc(stream_r1);  // for read of ' ,'
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = '\0'; 
			TodActDate = atoi((const char*)chr);
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = '\0'; 
			TodActMonth = atoi((const char*)chr);
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = fgetc(stream_r1);
			chr[3] = fgetc(stream_r1);
			chr[4] = '\0'; 
			TodActYr = atoi((const char*)chr);
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = '\0'; 
			TodActHr = atoi((const char*)chr);
			chr[0] = fgetc(stream_r1);
			chr[1] = fgetc(stream_r1);
			chr[2] = '\0'; 
			TodActMin = atoi((const char*)chr);
			fseek(stream_r1,16,SEEK_SET); 
			switch(NoOftod)
			{	//***************************************************************************************
				case 2:
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr1 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId1 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr2 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId2 = atoi((const char*)chr);
				break;
				//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case 3:
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr1 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId1 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr2 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId2 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr3 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId3 = atoi((const char*)chr);
				break;
				//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case 4:
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr1 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId1 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr2 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId2 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr3 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId3 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr4 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId4 = atoi((const char*)chr);
				break;
				//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case 5:
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr1 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId1 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr2 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId2 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr3 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId3 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr4 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId4 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr5 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId5 = atoi((const char*)chr);
				break;
				//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case 6:
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr1 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId1 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr2 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId2 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr3 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId3 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr4 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId4 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr5 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId5 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr6 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin6 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff6 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId6 = atoi((const char*)chr);
				break;
				//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case 7:
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr1 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId1 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr2 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId2 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr3 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId3 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr4 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId4 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr5 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId5 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr6 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin6 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff6 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId6 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr7 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin7 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff7 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId7 = atoi((const char*)chr);
				break;
				//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				case 8:
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr1 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff1 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId1 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr2 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff2 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId2 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr3 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff3 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId3 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr4 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff4 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId4 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr5 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff5 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId5 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr6 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin6 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff6 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId6 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr7 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin7 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff7 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId7 = atoi((const char*)chr);
					fgetc(stream_r1);  // for read of 0x0A    means Enter
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodHr8 = atoi((const char*)chr);
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					TodMin8 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = '\0'; 
					Tariff8 = atoi((const char*)chr);
					fgetc(stream_r1);//for read  ','
					chr[0] = fgetc(stream_r1);
					chr[1] = fgetc(stream_r1);
					chr[2] = '\0'; 
					MDScriptId8 = atoi((const char*)chr);
				break;
			}	//***************************************************************************************
			fclose(stream_r1);
			break;
		//--------------------------------------------------------------------------------
		case MD_RESET:
		
		
		break;
		
		//--------------------------------------------------------------------------------
		default:
		
		break;
	}	//===================================================================

	return(1);
}

#endif
