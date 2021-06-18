#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <linux/rtc.h>
#include <string.h>
#include <dirent.h>
#include <sys/select.h>

#include "Config.h"
#include "global.h"
#include "FUNCTH.h"
#include "AES128.h"
#include "auth.h"
#include "comm.h"
#include "hhuLcd.h"
#include "display.h"
#include "utility.h"
#include "rs232.h"
#include "Program.h"

void FrameTypeFile(_UBYTE *cp, _UBYTE &rrr_c, _UBYTE &rrr_c1, _UBYTE &rrr_s, _UBYTE &sss_c, _UBYTE &sss_c1, _UBYTE &sss_s);


int selScr(void)
{
	char l_Choice = 0;
	unsigned int l_Temp = 0;
	while(1)
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*) "1.SELECTED DOWNLOAD");
		memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		l_Temp = NoOfMeter();
		sprintf((char*)g_DispBuffer, "2.ALL DATA DOWN(%d)", l_Temp);
		HAL_DebugPrint(2, (_SBYTE*) g_DispBuffer);
		HAL_DebugPrint(3, (_SBYTE*) "3.ERASE AUTHNTICATION");
		HAL_DebugPrint(4, (_SBYTE*) "4.ERASE DATA");
		HAL_DebugPrint(5, (_SBYTE*) "0.EXIT");
        while((l_Choice > 0x34 || l_Choice < 0x30))
        l_Choice = _getkey();
		return (l_Choice-0x30);
    }
	return 0;
 }

int MainScr(void)
{
	char l_Choice = 0;

	while(1)
	{
		LcdClear();
		memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		sprintf((char*)g_DispBuffer, "DAKSH CMRI(%d.%d.%d)", APP_VER_MAJOR, APP_VER_MINOR, APP_VER_REV);
		HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
		HAL_DebugPrint(2, (_SBYTE*)"1.VIEW MODE");
		HAL_DebugPrint(3, (_SBYTE*)"2.MRI COMMUNICATION");
		HAL_DebugPrint(4, (_SBYTE*)"3.PC COMMUNICATION");
		HAL_DebugPrint(5, (_SBYTE*)"4.PROGRAMMING MODE");
		HAL_DebugPrint(6, (_SBYTE*)"5.AUTHENTICATION");
		HAL_DebugPrint(7, (_SBYTE*)"6.Exit   0. NEXT");
		l_Choice = 0;
		while((l_Choice > 0x36 || l_Choice < 0x30) )
        l_Choice = _getkey();
		if(l_Choice == '6')
		{
			return 0;
		}
		else if(l_Choice == '0')
		{
			LcdClear();
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((char*)g_DispBuffer, "DAKSH CMRI(%d.%d.%d)", APP_VER_MAJOR, APP_VER_MINOR, APP_VER_REV);
			HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
			HAL_DebugPrint(2, (_SBYTE*)"1.TEST MODE");
			HAL_DebugPrint(3, (_SBYTE*)"2.NAMEPLATE DETAILS");
			HAL_DebugPrint(4, (_SBYTE*)"3.METERS DOWNLOADED");
			HAL_DebugPrint(5, (_SBYTE*)"6.Back");
			l_Choice = 0;
			while(((l_Choice > 0x33 || l_Choice < 0x31)) && ( l_Choice != '6'))
			   l_Choice = _getkey();
			if(l_Choice == '6')
			{
				continue;
			}

			l_Choice += 5;
		}

		return (l_Choice-0x30);
    }
	return 0;
}

int LLSscr(void)
{
	char l_Choice = 0;
	while(1)
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"1.HPL METER");
		HAL_DebugPrint(2, (_SBYTE*)"2.HEPL METER");
		HAL_DebugPrint(3, (_SBYTE*)"3.OTHER METER");
        while((l_Choice>0x33)||(l_Choice<0x31))
        l_Choice = _getkey();
        return (l_Choice-0x30);
	}
	return 0;
}

int selScrSelected(void)
{
	char l_Choice = 0;
	while(1)
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"1.BILLING DATA");
		HAL_DebugPrint(2, (_SBYTE*)"2.EVENT DATA");
		HAL_DebugPrint(3, (_SBYTE*)"3.LOAD SURVEY DATA");
		HAL_DebugPrint(4, (_SBYTE*)"0.EXIT ");
        while((l_Choice > 0x33 || l_Choice < 0x30) ) 
        l_Choice = _getkey();
		return (l_Choice-0x30);
    }
	return 0;
}

int ProgrammingScr(void)
{
	char Pro_Choice = 0;
	while(1)
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"PROGRAMMING PARAMTR");
		HAL_DebugPrint(2, (_SBYTE*)"1.RTC SETTING");
		HAL_DebugPrint(3, (_SBYTE*)"2.DEMAND INT. PERIOD");
		HAL_DebugPrint(4, (_SBYTE*)"3.BILLING DATE");
		HAL_DebugPrint(5, (_SBYTE*)"4.PROFILE CAP.PERIOD");
		HAL_DebugPrint(6, (_SBYTE*)"5.TOD TIMINGS");
		HAL_DebugPrint(7, (_SBYTE*)"6.MD RESET  0.EXIT");
		//cin >> Choice;
        while((Pro_Choice > 0x36 || Pro_Choice < 0x30) ) 
        Pro_Choice = _getkey();
		return (Pro_Choice-0x30);
    }
	return 0;
}
int ProgrammingScrRead(void)
{
	char Pro_Choice = 0;
	while(1)
	{
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"READ PARAMETER");
		HAL_DebugPrint(2, (_SBYTE*)"1.MD INT. PERIOD");
		HAL_DebugPrint(3, (_SBYTE*)"2.BILLING DATE");
		HAL_DebugPrint(4, (_SBYTE*)"3.TOD TIMINGS");
		HAL_DebugPrint(5, (_SBYTE*)"4.LS INT. Time");
		HAL_DebugPrint(6, (_SBYTE*)"5.RTC 0.BACK ");
		HAL_DebugPrint(7, (_SBYTE*)"ENTER CHOICE:");
		//cin >> Choice;
        while((Pro_Choice > 0x35 || Pro_Choice < 0x30) ) 
        Pro_Choice = _getkey();
		return (Pro_Choice-0x30);
    }
	return 0;
}


void nameplate(void)
{
	char l_LineNo = 0;
	unsigned char l_TempDispBuffer[43];
	unsigned char i; //,l_Choice=0, meter_no_array[8],
	int l_result=0,x=0,y=0,l_Catbuff=0,l_Ratingbuff=0,l_loop=0;
	char attrib = 1;
	char category = 0;
	char rating = 0;
	char f_Rating = 0;
	char fg_MtrCurrRating = 0;
	char fg_MtrCat = 0;//f_Const=0,
	RS232_PortClose(SERAIL_RJ_PORT_NO);
	UTL_Wait(100);
	RS232_PortOpen(SERAIL_RJ_PORT_NO, BAUD_RATE);
	LcdClear();
	//+++++++++++++++++++++++++++++++++++++	
	Read_Serial_No();
	Authentication();
	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*) "METER READ...");
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
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"   * NAME PALTE *");
			l_result = ViewRequest(1,0,0,96,1,1,255,2);//Manufacture name
			l_LineNo = 2;
			if((fg_RxBuffer[15] == 0x0A ) || (fg_RxBuffer[15] == 0x09 ))
			{
				HAL_DebugPrint(l_LineNo, (_SBYTE*)"MANUFACTURE NAME- ");l_LineNo++;
				memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
				for(i = 0; (i < fg_RxBuffer[16]) && (i < 42); i++)
				{
					sprintf((_SBYTE*)&l_TempDispBuffer[i], "%c",fg_RxBuffer[17+i]);
					fprintf(stderr, "%c",fg_RxBuffer[17+i]);
				}
				HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
				if(i > 21)
				{
					l_LineNo++;
				}
			}
			l_result = ViewRequest(1,0,0,96,1,0,255,2);//sr no.
			if(l_result)
			{
				if(( fg_RxBuffer[15] == 0x06) || ( fg_RxBuffer[15] == 0x05))
				{
					Serialno = (long)((long)fg_RxBuffer[16]*65536*256 + (long)fg_RxBuffer[17]*65536 + (long)fg_RxBuffer[18]*256 + (long)fg_RxBuffer[19]);
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "SERIAL NO.=%06lu",(long)Serialno);
					HAL_DebugPrint(l_LineNo, (_SBYTE*) g_DispBuffer); l_LineNo++;
				}
				else
				{
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"SERIAL NO.=");
					for(i = 0; i < fg_RxBuffer[16]; i++)
					{
						sprintf((_SBYTE*)&l_TempDispBuffer[11 + i], "%c",fg_RxBuffer[17+i]);
					}
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
					if(i > 10)
					{
						l_LineNo++;
					}
				}
				HAL_DebugPrint(l_LineNo, (_SBYTE*)"PRESS ANY KEY..");
				_getkey();
			}
		}
		else
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
			Disconnect();
			_getkey();
			return;
		}
	}
	else
	{

		Disconnect();
		LcdClear();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
		_getkey();
		return;
	}

	g_UserAssociation = MR_ASSOCIATION;
	l_result = 0;
	LcdClear();
	HAL_DebugPrint(1, (_SBYTE*) "METER READ...");
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
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"   * NAME PALTE *");
			l_LineNo = 2;
			l_result = ViewRequest(1,0,0,94,91,9,255,2);
			if(l_result)
			{
				if(fg_RxBuffer[15] == 0x11 )
				{
					if(fg_RxBuffer[16] == 0)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*)"METER TYPE=3P-3W");l_LineNo++;
					}
					else if(fg_RxBuffer[16] == 1)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*)"METER TYPE=3P-4W");l_LineNo++;
					}
					else if(fg_RxBuffer[16] == 2)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*)"METER TYPE=3P-4W HT");l_LineNo++;// PT/CT
						//l_LineNo++;
					}
					else if(fg_RxBuffer[16] == 3)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*)"MTR TYPE=3P 4W LT CT");l_LineNo++;
					}
					else if(fg_RxBuffer[16] == 4)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*)"MTR TYPE=3P 4W WC");l_LineNo++;
					}
					else if(fg_RxBuffer[16] == 5)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*)"1P 2W WC");l_LineNo++;
					}
				}
			}
			l_result = ViewRequest(1,1,0,0,2,0,255,2);
			if(l_result)
			{
				if((fg_RxBuffer[15]==0x0A )||(fg_RxBuffer[15]==0x09 ))
				{
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"MTR FW VER=");
					for(i = 0; i < fg_RxBuffer[16]; i++)
					{
						sprintf((_SBYTE*)&l_TempDispBuffer[11+i], "%c",fg_RxBuffer[17+i]);
					}
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
					if(i > 10)
					{
						l_LineNo++;
					}
				}
			}
			//********************************************************
			l_result = ViewRequest(1,0,0,96,1,4,255,2);
			if(l_result)
			{
				if(fg_RxBuffer[15] == 0x12 )
				{
					Serialno = (long)((long)fg_RxBuffer[16]*256 + (long)fg_RxBuffer[17] );
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"MANUF.YEAR =%lu",(long)Serialno);
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
				}
				else if(fg_RxBuffer[15] == 0x09 )
				{
					Serialno = (long)((long)fg_RxBuffer[17]*256 + (long)fg_RxBuffer[18] );
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"MANUF.YEAR =%lu",(long)Serialno);
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
				}
			}

			//********************************************************
			l_result = ViewRequest(1,0,0,94,91,11,255,2);//Mtr category
			if(l_result)
			{
				if((fg_RxBuffer[15] == 0x0A ) || (fg_RxBuffer[15] == 0x09 ))
				{
					fg_MtrCat=1;
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"MTR CATEGRY=");
					for(i = 0; i < fg_RxBuffer[16]; i++)
					{
						sprintf((_SBYTE*)&l_TempDispBuffer[12+i], "%c",fg_RxBuffer[17+i]);
					}
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
				}
			}
			//********************************************************
			l_result = ViewRequest(1,0,0,94,91,12,255,2);//Mtrcurrent rating
			if(l_result)
			{
				if((fg_RxBuffer[15] == 0x0A ) || (fg_RxBuffer[15] == 0x09 ))
				{
					fg_MtrCurrRating=1;
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"CURR RATING=");
					for(i = 0; i < fg_RxBuffer[16]; i++)
					{
						sprintf((_SBYTE*)&l_TempDispBuffer[12+i], "%c",fg_RxBuffer[17+i]);
					}
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
				}
			}
			//********************************************************
			l_result = ViewRequest(1,1,0,0,4,2,255,2);//CT ratio
			if(l_result)
			{
				if(fg_RxBuffer[15] == 0x12 )
				{
					Serialno = (long)((long)fg_RxBuffer[16]*256 + (long)fg_RxBuffer[17] );
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"CT Ratio   =%lu",(long)Serialno);
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
				}
			}

			l_result = ViewRequest(1,1,0,0,4,3,255,2);//pT ratio
			if(l_result)
			{
				if(fg_RxBuffer[15]==0x12 )
				{
					Serialno = (long)((long)fg_RxBuffer[16]*256 + (long)fg_RxBuffer[17] );
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"PT Ratio   =%lu",(long)Serialno);
					HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
				}
			}

			x = 0; category = rating = attrib = 0;

			//------- for category and rating, this profile of hpl ------------
			l_result = ViewRequest(7,1,0,99,128,00,255,3);
			if(l_result)
			{
				for(attrib = 0; attrib < fg_RxBuffer[16]; attrib++)
				{
					if((fg_RxBuffer[27+x] == 0x5B ) && (fg_RxBuffer[28+x] == 0x0B ) && \
					   (fg_RxBuffer[29+x] == 0xFF ) && (fg_RxBuffer[30+x] == 0x0F ))
					{
						category = attrib;
					}
					else if((fg_RxBuffer[27+x] == 0x5B ) && (fg_RxBuffer[28+x] == 0x0C ) && \
							(fg_RxBuffer[29+x] == 0xFF ) && (fg_RxBuffer[30+x] == 0x0F ))
					{
						rating = attrib;
						f_Rating=1;
					}
					x = x + 18;
				}
			}
			y = 0;x = 0; attrib = 0;
			l_result = ViewRequest(7,1,0,99,128,00,255,2);
			if(l_result)
			{
				for(attrib = 0; attrib < fg_RxBuffer[18]; attrib++)
				{
					if( (fg_RxBuffer[19 + y] ) == 0x09)
					{
						y = y + fg_RxBuffer[19+y+1] + 2;
					}
					else if(fg_RxBuffer[19 + y] == 0x06)
					{
						y= y + 4 + 1;
					}
					else if(fg_RxBuffer[19 + y]==0x12)
					{
						y = y + 2 +1;
					}
					if((attrib+1) == category)
					{
						l_Catbuff = y + 19;
					}
					else if((attrib+1) == rating)
					{
						l_Ratingbuff = y + 19;
					}
				}
				if( (fg_RxBuffer[l_Catbuff +2] == 0x43) && (fg_RxBuffer[l_Catbuff +3] == 0x31) )
				{
					if(!fg_MtrCat)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*) "Mtr.Catgry=C1"); l_LineNo++;
					}
				}
				else if( (fg_RxBuffer[l_Catbuff +2] == 0x43) && (fg_RxBuffer[l_Catbuff +3] == 0x32) )
				{
					if(!fg_MtrCat)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*) "Mtr.Catgry=C2"); l_LineNo++;
					}
				}
				else if( (fg_RxBuffer[l_Catbuff +2] == 0x43) && (fg_RxBuffer[l_Catbuff +3] == 0x33) )
				{
					if(!fg_MtrCat)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*) "Mtr.Catgry=Single Ph"); l_LineNo++;
					}
				}
				else if(fg_RxBuffer[l_Catbuff + 2] == 'A')
				{
					if(!fg_MtrCat)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*) "Mtr.Catgry=A"); l_LineNo++;
					}
				}
				else if(fg_RxBuffer[l_Catbuff +2] == 'B')
				{
					if(fg_MtrCat == 0)
					{
						HAL_DebugPrint(l_LineNo, (_SBYTE*) "Mtr.Catgry=B"); l_LineNo++;
					}
				}
				if(f_Rating==1)
				{
					if(fg_MtrCurrRating == 0)
					{
						memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
						sprintf((_SBYTE*)l_TempDispBuffer,"Mtr.Rating=");
						for(l_loop=0;l_loop< fg_RxBuffer[l_Ratingbuff +1] ;l_loop++)
						{
							sprintf((_SBYTE*)&l_TempDispBuffer[11+i], "%c",fg_RxBuffer[l_Ratingbuff +2+l_loop]);
						}
						HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
					}
					memset(l_TempDispBuffer, 0, sizeof(l_TempDispBuffer));
					sprintf((_SBYTE*)l_TempDispBuffer,"Mtr.Const.=");
					l_loop=0;
					for(l_loop=0;l_loop< fg_RxBuffer[l_Ratingbuff +1+l_loop] ;l_loop++)
					{
						if( (fg_RxBuffer[l_Ratingbuff +2+l_loop] == 0x2F) )
						{
							if( (fg_RxBuffer[l_Ratingbuff +2+l_loop+1] == 0x35) )
							{
							   sprintf((_SBYTE*)&l_TempDispBuffer[11], "16000 imp");
							   HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
							   break;
							}
							else if( (fg_RxBuffer[l_Ratingbuff +2+l_loop+1] == 0x31) )
							{
							   sprintf((_SBYTE*)&l_TempDispBuffer[11], "48000 imp");
							   HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempDispBuffer); l_LineNo++;
							   break;
							}
						}
					}
				}
			}
			HAL_DebugPrint(l_LineNo, (_SBYTE*)"PRESS ANY KEY..");
			_getkey();

		}
		else
		{
			HAL_DebugPrint(1, (_SBYTE*)"ASSOCIATION FAIL");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
			Disconnect();
			_getkey();
			return;
		}
	}
	else
	{
		Disconnect();
		HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
		HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE");
		HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
		_getkey();
		return;
	}

	Disconnect();
}

void  Memory_status(void)
{
	/*///Mundel
	struct dfree free;
	long avail;
	LcdClear();

	 getdfree(2, &free);         //getting disk 2 ( B drive ) free space
   if (free.df_sclus == 0xFFFF)
   {
		
	   printf("Error in getdfree() call\n");
	   delay(1500);
	   //exit(1);
   }
	 avail =  (long) free.df_avail
			 * (long) free.df_bsec
			 * (long) free.df_sclus;    //To get exact bytes available

	 avail /=1024;

	 LcdClear();
	 printf("Free Space:");
	 printf("%ld KBytes\n",avail);
	 	 */
	 
}
//==============================================================

char displayProgPm (void)
{
	int l_Pscr=0,l_Pscr2=0,l_Todloop=0,l_Todindex=0;
    unsigned char l_result;
	unsigned long l_temp;
	char l_LineNo = 0;
    int n,p,Validuptoday,Validuptomon,Validuptoyr;
    char chr[5];
    struct rtc_time d5;
    FILE *fp3,*fp4;
	char buffer[360];

	RS232_PortClose(SERAIL_RJ_PORT_NO);
	UTL_Wait(100);
	RS232_PortOpen(SERAIL_RJ_PORT_NO, BAUD_RATE);
Pscr: l_Pscr=0;
	LcdClear();
	fflush(stdin);
	//+++++++++++++++++++++++++++++++++++++

	l_result = Read_Serial_No();
	if(l_result == 1)
	{
		return 0;
	}

	l_result = Authentication();
	fprintf(stderr, "l_result = %d", l_result);
	if(l_result == 0)
	{
//		LcdClear();
		puts("COMM ERROR");
//		_getkey();
		return 0;
	}
	//++++++++++++++++++++++++++++++++++++++
	UTL_RTCRead (&d5);		//Read RTC of HHU
	LcdClear();
	l_Pscr=0;
	HAL_DebugPrint(1, (_SBYTE*)"  * PROGRAMMING *");
	HAL_DebugPrint(2, (_SBYTE*)"1.SET PARAMETER");
	HAL_DebugPrint(3, (_SBYTE*)"2.READ PARAMETER");
	HAL_DebugPrint(4, (_SBYTE*)"0.BACK");
	while((l_Pscr > 0x32 || l_Pscr < 0x30) )
	l_Pscr = _getkey();
	if(l_Pscr == 0x30)
	{
		return 0;
	}
	else if(l_Pscr == 0x31)
	{
		fp3=fopen(FILE_DATE_TXT,"r+");
		if(fp3 == NULL)
		{
			LcdClear();
			fprintf(stderr, "file open error %s",FILE_DATE_TXT);
			HAL_DebugPrint(1, (_SBYTE*)"HHU Not Authenticate");
			HAL_DebugPrint(2, (_SBYTE*)"PRESS ANY KEY..");
			Disconnect();
			_getkey();
			return 0;
		}

		memset(buffer, 0, sizeof(buffer));

		fseek(fp3,0,SEEK_SET);
		chr[0] = fgetc(fp3);
		chr[1] = fgetc(fp3);
		chr[2] = '\0';
		Validuptoday = atoi(chr);

		fseek(fp3,2,SEEK_SET);
		chr[0] = fgetc(fp3);
		chr[1] = fgetc(fp3);
		chr[2] = '\0';
		Validuptomon = atoi(chr);

		fseek(fp3,6,SEEK_SET);
		chr[0] = fgetc(fp3);
		chr[1] = fgetc(fp3);
		chr[2] = '\0';
		Validuptoyr = atoi(chr);
		Validuptoyr  += 2000;

		if((d5.tm_year == Validuptoyr))
		{
			if(((d5.tm_mon+1) == Validuptomon))
			{
				if((d5.tm_mday >= Validuptoday))
				{
					LcdClear();
					HAL_DebugPrint(1, (_SBYTE*)"AUTH VALIDITY");
					memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
					sprintf((_SBYTE*)g_DispBuffer, "DATE(%02d/%02d/%02d)", Validuptoday, Validuptomon, Validuptoyr);
					HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
					HAL_DebugPrint(3, (_SBYTE*)"EXPIRED");
					HAL_DebugPrint(4, (_SBYTE*)"press a key");
					_getkey();
					l_Pscr=0;
					fclose(fp3);
					return 0;
				}
			}
            else if((d5.tm_mday+1) > Validuptomon)
            {
				LcdClear();
				HAL_DebugPrint(1, (_SBYTE*)"AUTH VALIDITY");
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "DATE(%02d/%02d/%02d)", Validuptoday, Validuptomon, Validuptoyr);
				HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
				HAL_DebugPrint(3, (_SBYTE*)"EXPIRED");
				HAL_DebugPrint(4, (_SBYTE*)"press a key");
				_getkey();
				l_Pscr=0;
				fclose(fp3);
				return 0;
            }
		}
		else if((d5.tm_year > Validuptoyr))
		{
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*)"AUTH VALIDITY");
			memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
			sprintf((_SBYTE*)g_DispBuffer, "DATE(%02d/%02d/%02d)", Validuptoday, Validuptomon, Validuptoyr);
			HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
			HAL_DebugPrint(3, (_SBYTE*)"EXPIRED");
			HAL_DebugPrint(4, (_SBYTE*)"press a key");
			_getkey();
			l_Pscr=0;
			fclose(fp3);
			return 0;
		}

		MeterProgramming();
		fclose(fp3);
	}
	else if(l_Pscr == 0x32)
	{

SCR2:	///InitPort(INTR,PORT,PIC,BAUD_RATE); ///mundel
		LcdClear();
		g_UserAssociation = PC_ASSOCIATION;
		Read_Serial_No();
		//Authentication();
		l_Pscr2=0;
		l_Pscr2 = ProgrammingScrRead();

		if(l_Pscr2 == 0)
		{
			goto Pscr;
		}
		else if(l_Pscr2)//billing Date
		{
			g_UserAssociation = MR_ASSOCIATION;
			rrr_c=0;rrr_c1=0;rrr_s=0;sss_c=0;sss_c1=0;sss_s=0;
			Disconnect();
			l_result = SetNormalResponseMode ();
			if(l_result == 1)
			{
				l_result =0;
				l_result = AssociationRequest();
				if((l_result == 1) && (l_Pscr2 == 2 ))
				{
					l_result = ViewRequest(22,0,0,15,0,0,255,4);//billing date
					if(l_result)
					{
						LcdClear();
						l_temp = fg_RxBuffer[30];
						if(l_temp == 0)
						{
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "BILLING DATE: %d", l_temp);
							HAL_DebugPrint(1, (_SBYTE*)g_DispBuffer);
							HAL_DebugPrint(2, (_SBYTE*)"(DISABLED)");
						}
						else
						{
							memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
							sprintf((_SBYTE*)g_DispBuffer, "BILLING DATE: %d", l_temp);
							HAL_DebugPrint(1, (_SBYTE*)g_DispBuffer);

						}
						l_temp=0;
						l_temp = fg_RxBuffer[17]*0x100 + fg_RxBuffer[18];
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "BILLING TIME:%02d:%02d:%02d",fg_RxBuffer[21],fg_RxBuffer[22],fg_RxBuffer[23]);
						HAL_DebugPrint(3, (_SBYTE*)g_DispBuffer);
						HAL_DebugPrint(4, (_SBYTE*)"Press any key..");
						_getkey();
					}
					Disconnect();
					goto SCR2;
				}
				else if((l_result == 1) && (l_Pscr2 == 1 ))
				{
					l_result=0;
					l_result = ViewRequest(1,1,0,0,8,0,255,2);//MDIP
					if(l_result)
					{
						LcdClear();
						if( fg_RxBuffer[15] == 0x12)
						l_temp = fg_RxBuffer[16]*0x100 + fg_RxBuffer[17];
						else if( fg_RxBuffer[15] == 0x06)
						l_temp = fg_RxBuffer[16]*0x1000 +fg_RxBuffer[17]*0x1000+ fg_RxBuffer[18]*0x100+ fg_RxBuffer[19];
						HAL_DebugPrint(1, (_SBYTE*)"Demand Int. Period");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "= %ld Minute",(l_temp/60));
						HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
						HAL_DebugPrint(3, (_SBYTE*)"Press any key..");
						_getkey();
					}
					Disconnect();
					goto SCR2;
				}
				else if((l_result == 1) && (l_Pscr2 == 3 ))//TOD
				{
					l_result = GetRequestFile(0x14, 0x00, 0x00, 0x0D, 0x00, 0x00, 0xff, 0x05);//Active tod timing
					if(l_result == 0)
					{
						CommError();
						return 0;
					}
					else
					{
						l_result = 0;
					}
					fp4 = fopen(FILE_TOD_TXT,"r+");
					if(fp4 == NULL)
					{
						LcdClear();
						printf("file open error1 %s", FILE_TOD_TXT);
						_getkey();
						Disconnect();
						return 0;
					}
					/*
						write todtime data in a file and after read we have read all data again from file and then convert in again in main buffer for cutting
					*/
					LcdClear();
					fseek(fp4,0,SEEK_SET);
					memset(buffer,0,sizeof(buffer));
					memset(fg_RxBuffer,0,sizeof(fg_RxBuffer));
					fread (buffer,1,359,fp4);
					p=36;
					for(n=0;n<358;n++)
					{
						if((buffer[p+1] <= 0x39) &&(buffer[p+1] >= 0x30))
						{
							buffer[p+1] = buffer[p+1]-48;
						}
						else if((buffer[p+1] <= 0x46) &&(buffer[p+1] >= 0x41) )
						{
							buffer[p+1] = buffer[p+1]-0x37;
						}
						if((buffer[p+2] <= 0x39) &&(buffer[p+2] >= 0x30))
						{
							buffer[p+2] = buffer[p+2]-48;
						}
						else if((buffer[p+2] <= 0x46) &&(buffer[p+2] >= 0x41) )
						{
							buffer[p+2] = buffer[p+2]-0x37;
						}
						fg_RxBuffer[21 +n] = ((buffer[p+1])*0x10)+(buffer[p+2]);
						p= p+2;
					}

					l_result=0;
					LcdClear();
					l_Todloop=0,l_Todindex=0;
					HAL_DebugPrint(1, (_SBYTE*)"Active TOD Timings");
					HAL_DebugPrint(2, (_SBYTE*)"TIME           Tariff");
					l_LineNo = 3;
					l_result = 1;//ViewRequest(20,0,0,13,0,0,255,5);//Active
					if(l_result)
					{
						for(l_Todloop = 0; l_Todloop < (fg_RxBuffer[22]/2); l_Todloop++)
						{
							if((fg_RxBuffer[27]== 0)&&(l_Todloop == 0))
							{
								if(fg_RxBuffer[46+l_Todindex] == 0)
								{
									fg_RxBuffer[46+l_Todindex] = fg_RxBuffer[27];
									fg_RxBuffer[47+l_Todindex] = fg_RxBuffer[28];
								}
								if(fg_RxBuffer[41+l_Todindex] != 0)
								{
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d-%02d:%02d     %d",fg_RxBuffer[27+l_Todindex],fg_RxBuffer[28+l_Todindex],\
																	fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
									HAL_DebugPrint(l_LineNo, (_SBYTE*)g_DispBuffer); l_LineNo++;
//									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
//									sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d   %d",fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
//									HAL_DebugPrint(4, (_SBYTE*)g_DispBuffer);
								}
								l_Todindex =l_Todindex+19;
							}
							else if(fg_RxBuffer[27+l_Todindex]!= 0)
							{
								if(fg_RxBuffer[46+l_Todindex] == 0)
								{
									fg_RxBuffer[46+l_Todindex] = fg_RxBuffer[27];
									fg_RxBuffer[47+l_Todindex] = fg_RxBuffer[28];
								}
								if((fg_RxBuffer[41+l_Todindex] != 0)&&(fg_RxBuffer[41+l_Todindex] != 0xFF))
								{
									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
									sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d-%02d:%02d     %d",fg_RxBuffer[27+l_Todindex],fg_RxBuffer[28+l_Todindex],\
											fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
									HAL_DebugPrint(l_LineNo, (_SBYTE*)g_DispBuffer); l_LineNo++;
//									memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
//									sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d   %d",fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
//									HAL_DebugPrint(4, (_SBYTE*)g_DispBuffer);
								}
								l_Todindex =l_Todindex+19;
							}
							//l_LineNo++;
							if(!(l_LineNo % 7))
							{
								HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
								_getkey();
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"Active TOD Timings");
								HAL_DebugPrint(2, (_SBYTE*)"TIME           Tariff");
								l_LineNo = 3;
							}
						}
					}

					if(l_LineNo != 3)
					{
						HAL_DebugPrint(5, (_SBYTE*)"Press any key..");
						_getkey();
					}
					Disconnect();
					fclose(fp4);
					l_result=0;
					l_Todloop=0,l_Todindex=0;
					LcdClear();
					g_UserAssociation = PC_ASSOCIATION;
					Read_Serial_No();
					Authentication();
					LcdClear();
					g_UserAssociation = MR_ASSOCIATION;
					rrr_c=0;rrr_c1=0;rrr_s=0;sss_c=0;sss_c1=0;sss_s=0;
					Disconnect();
					l_result = SetNormalResponseMode ();
					if(l_result == 1)
					{
						l_result =0;
						l_result = AssociationRequest();
						if(l_result)
						{
							l_result = GetRequestFile(0x14, 0x00, 0x00, 0x0D, 0x00, 0x00, 0xff, 0x09);//Passive tod timing
							if(l_result == 0)
							{
								CommError();
								return 0;
							}
							else
							{
								l_result = 0;
							}
							fp4 = fopen(FILE_TOD_TXT,"r+");
							if(fp4 == NULL)
							{
								LcdClear();
								printf("file open error2 %s", FILE_TOD_TXT);
								_getkey();
								Disconnect();
								return 0;
							}
							else
							{
								// printf("file opened\n");
								// _getkey();
							}
							LcdClear();
							fseek(fp4,0,SEEK_SET);
							memset(buffer,0,sizeof(buffer));
							memset(fg_RxBuffer,0,sizeof(fg_RxBuffer));
							fread (buffer,1,359,fp4);
							p=36;
							for(n=0;n<358;n++)
							{
								if((buffer[p+1] <= 0x39) &&(buffer[p+1] >= 0x30))
								{
									buffer[p+1] = buffer[p+1]-48;
								}
								else if((buffer[p+1] <= 0x46) &&(buffer[p+1] >= 0x41) )
								{
									buffer[p+1] = buffer[p+1]-0x37;
								}
								if((buffer[p+2] <= 0x39) &&(buffer[p+2] >= 0x30))
								{
									buffer[p+2] = buffer[p+2]-48;
								}
								else if((buffer[p+2] <= 0x46) &&(buffer[p+2] >= 0x41) )
								{
									buffer[p+2] = buffer[p+2]-0x37;
								}
								fg_RxBuffer[21 +n] = ((buffer[p+1])*0x10)+(buffer[p+2]);

								p= p+2;
							}
							HAL_DebugPrint(1, (_SBYTE*)"Passive TOD Timings");
							HAL_DebugPrint(2, (_SBYTE*)"TIME           Tariff");
							l_result =1;// ViewRequest(20,0,0,13,0,0,255,9);//Passive
							l_LineNo = 3;
							if(l_result)
							{
								for(l_Todloop=0;l_Todloop< (fg_RxBuffer[22]/2);l_Todloop++)
								{
									if((fg_RxBuffer[27]== 0)&&(l_Todloop == 0))
									{
										if(fg_RxBuffer[46+l_Todindex] == 0)
										{
											fg_RxBuffer[46+l_Todindex] = fg_RxBuffer[27];
											fg_RxBuffer[47+l_Todindex] = fg_RxBuffer[28];
										}
										if(fg_RxBuffer[41+l_Todindex] != 0)
										{
											memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
											sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d-%02d:%02d     %d",fg_RxBuffer[27+l_Todindex],fg_RxBuffer[28+l_Todindex],\
													fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
											HAL_DebugPrint(l_LineNo, (_SBYTE*)g_DispBuffer); l_LineNo++;
//											memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
//											sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d   %d",fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
//											HAL_DebugPrint(4, (_SBYTE*)g_DispBuffer);
										}
										l_Todindex =l_Todindex+19;
									}
									else if(fg_RxBuffer[27+l_Todindex]!= 0)
									{
										if(fg_RxBuffer[46+l_Todindex] == 0)
										{
											fg_RxBuffer[46+l_Todindex] = fg_RxBuffer[27];
											fg_RxBuffer[47+l_Todindex] = fg_RxBuffer[28];
										}
										if(fg_RxBuffer[41+l_Todindex] != 0)
										{
											memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
											sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d-%02d:%02d     %d",fg_RxBuffer[27+l_Todindex],fg_RxBuffer[28+l_Todindex],\
													fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
											HAL_DebugPrint(l_LineNo, (_SBYTE*)g_DispBuffer); l_LineNo++;
//											memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
//											sprintf((_SBYTE*)g_DispBuffer, "%02d:%02d   %d",fg_RxBuffer[46+l_Todindex],fg_RxBuffer[47+l_Todindex],fg_RxBuffer[41+l_Todindex]);
//											HAL_DebugPrint(4, (_SBYTE*)g_DispBuffer);
										}
										l_Todindex =l_Todindex+19;
									}

									//l_LineNo++;
									if(!(l_LineNo % 7))
									{
										HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
										_getkey();
										LcdClear();
										HAL_DebugPrint(1, (_SBYTE*)"Passive TOD Timings");
										HAL_DebugPrint(2, (_SBYTE*)"TIME           Tariff");
										l_LineNo = 3;
									}
								}
							}
							if(l_LineNo != 3)
							{
								HAL_DebugPrint(5, (_SBYTE*)"Press any key..");
								_getkey();
							}
							LcdClear();
							g_UserAssociation = MR_ASSOCIATION;
							rrr_c=0;rrr_c1=0;rrr_s=0;sss_c=0;sss_c1=0;sss_s=0;
							Disconnect();
							l_result = SetNormalResponseMode ();
							if(l_result == 1)
							{
								l_result =0;
								l_result = AssociationRequest();
								if(l_result)
								{
									l_result = ViewRequest(20,0,0,13,0,0,255,10);//RTC of pasive time
									if(l_result)
									{
										l_temp = fg_RxBuffer[17]*0x100 + fg_RxBuffer[18];
										HAL_DebugPrint(1, (_SBYTE*)"Activation Date");
										memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
										sprintf((_SBYTE*)g_DispBuffer, "%02d/%02d/%d  %02d:%02d",fg_RxBuffer[20],fg_RxBuffer[19],l_temp, fg_RxBuffer[22],fg_RxBuffer[23]);
										HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
//										HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
//										Disconnect();
//										_getkey();
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
								LcdClear();
								HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
								HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE");
								HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
								_getkey();
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
						LcdClear();
						HAL_DebugPrint(1, (_SBYTE*)"COMMUNICATION FAIL");
						HAL_DebugPrint(2, (_SBYTE*)"CHECK CABLE");
						HAL_DebugPrint(3, (_SBYTE*)"PRESS ANY KEY..");
						_getkey();
					}
					HAL_DebugPrint(7, (_SBYTE*)"Press any key..");
					fclose(fp4);
					_getkey();
					Disconnect();
					goto SCR2;
				}
				else if((l_result == 1) && (l_Pscr2 == 4 ))//lsip
				{
					l_result = ViewRequest(1,1,0,0,8,4,255,2);//LSINT TIME
					if(l_result)
					{
						LcdClear();
						if( fg_RxBuffer[15] == 0x12)
						l_temp = fg_RxBuffer[16]*0x100 + fg_RxBuffer[17];
						else if( fg_RxBuffer[15] == 0x06)
						l_temp = fg_RxBuffer[16]*0x1000 +fg_RxBuffer[17]*0x1000+ fg_RxBuffer[18]*0x100+ fg_RxBuffer[19];
						HAL_DebugPrint(1, (_SBYTE*)"Profile Cap Period");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "= %ld Minute",(l_temp/60));
						HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
					}
					HAL_DebugPrint(3, (_SBYTE*)"Press any key..");
					_getkey();
					Disconnect();
					goto SCR2;
				}
				else if((l_result == 1) && (l_Pscr2 == 5 ))//RTC
				{
					l_result = ViewRequest(8,0,0,1,0,0,255,2);//RTC
					if(l_result)
					{
						LcdClear();
						l_temp = fg_RxBuffer[17]*0x100 + fg_RxBuffer[18];
						HAL_DebugPrint(1, (_SBYTE*)"Date:(DD/MM/YYYY)");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "    :%02d/%02d/%d",fg_RxBuffer[20],fg_RxBuffer[19],l_temp);
						HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
						HAL_DebugPrint(3, (_SBYTE*)"Time:(HH:MM:SS)");
						memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
						sprintf((_SBYTE*)g_DispBuffer, "    :%02d:%02d:%02d",fg_RxBuffer[22],fg_RxBuffer[23],fg_RxBuffer[24]);
						HAL_DebugPrint(4, (_SBYTE*)g_DispBuffer);
					}
					HAL_DebugPrint(5, (_SBYTE*)"Press any key..");
					_getkey();
					Disconnect();
					goto SCR2;
				}
			}
		}
	}
	Disconnect();
	return 0;
}
//========================getrequest for save file===========================================================
		
int GetRequestFile(_UBYTE classID,_UBYTE Obis_a,_UBYTE Obis_b,_UBYTE Obis_c,_UBYTE Obis_d,_UBYTE Obis_e,_UBYTE Obis_f,_UBYTE Attr)
{
	_ULONG datablockno = 0,ctratio=0;
	_UBYTE moreblock = 0, ret_lsh=0;
	int counter = 0,l_Bytes = 0;
	_UBYTE l_RetVal = 0x00;
	FILE *fp5;
	remove(FILE_TOD_TXT);
	fp5 = fopen(FILE_TOD_TXT,"w");
	if(fp5 == NULL)
    {
        LcdClear();
		printf("file open error3 %s", FILE_TOD_TXT);
		_getkey();
		fclose(fp5);
		return 0;
    }
    else 
    {
        // printf("file opened\n");
        // _getkey();
    }  
	ClearPacket();
   
	if(PGbar % 20 == 0)
	{
		//ClrLn(2);
		// ErrPrt(1,1,"DATA DOWNLOADING....\n");
        // ErrPrt(1,2,"VOLUME:");
        // ErrPrt(1,3,"DON'T PRESS ANY KEY\nDURING DOWNLOADING");
		PGbar = 0;
	}
	// ErrPrt(8,2,"");printf("%lu BYTES",(long)VolumeOfData);
   
    
	//printf("|"); 
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
	{fclose(fp5);
		return(0);
	}	


	l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);
	fprintf(fp5, "        \n");

	for(counter = 16; counter <= 21; counter++)//obis code write
	{
		fprintf(fp5, "%02X", fg_SendBuffer[counter]);
		VolumeOfData +=2;
	}

	fprintf(fp5," %02X ",fg_SendBuffer[22]);

	FrameTypeFile(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

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
                        fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }  
                else if(fg_RxBuffer[20] == 0x82 )
                {
                    for(counter=23; counter < l_Bytes; counter++)
                    {
                        fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                } 
                
                else
                {
                    for(counter=21; counter < l_Bytes; counter++)
                    {
                        fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }    
            }  
        }
        else
        {        
            for(counter=15; counter < l_Bytes; counter++)
            {
                fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
            }
        }
        fclose(fp5);
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
                    fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                }
            }  
            else if(fg_RxBuffer[20] == 0x82 )
            {
                for(counter=23; counter < l_Bytes; counter++)
                {
                    fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                }
            } 
            else
            {
                for(counter=21; counter < l_Bytes; counter++)
                {
                    fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                }
            }    
        }       
	}
	else
	{
		for(counter=15; counter < l_Bytes; counter++)
		{
			fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
		}
	}

	while (fg_RxBuffer[1] & 0x08) //== 0xA8
	{
		ClearPacket();

	if(PGbar % 20 == 0)
	{
		//ClrLn(2);
		// ErrPrt(1,1,"DATA DOWNLOADING....\n");
        // ErrPrt(1,2,"VOLUME:");
        // ErrPrt(1,3,"DON'T PRESS ANY KEY\nDURING DOWNLOADING");
		PGbar = 0;
	}
	// ErrPrt(8,2,"");printf("%lu BYTES",(long)VolumeOfData);
		//printf("|"); 
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
			fclose(fp5);
			return(0);
		}
		l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);
		FrameTypeFile(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

		if (!( fg_RxBuffer[1] & 0x08)) //0xA0
		{
			for(counter=8; counter < l_Bytes; counter++)
			{
				fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
			}
			
			break;
		}
		
		if (fg_RxBuffer[1] & 0x08) //== 0xA8
		{
			for(counter=8; counter < l_Bytes; counter++)
			{
				fprintf(fp5, "%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
			}
		}
	}//While(0xA8)
	
	while (moreblock)
	{
		moreblock = 0;
		ClearPacket();

    	if(PGbar % 20 == 0)
    	{
    		//ClrLn(2);
    		// ErrPrt(1,1,"DATA DOWNLOADING....\n");
            // ErrPrt(1,2,"VOLUME:");
            // ErrPrt(1,3,"DON'T PRESS ANY KEY\nDURING DOWNLOADING");
    		PGbar = 0;
    	}

    	memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		sprintf((_SBYTE*)g_DispBuffer, "%lu BYTES",(long)VolumeOfData);
		HAL_DebugPrint(2, (_SBYTE*)g_DispBuffer);
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
		fclose(fp5);
			return(0);
		}
		l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);
		FrameTypeFile(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

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
                        fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                }  
                else if(fg_RxBuffer[20] == 0x82 )
                {
                    for(counter=23; counter < l_Bytes; counter++)
                    {
                        fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                    }
                } 
                else
                {
                    for(counter=21; counter < l_Bytes; counter++)
                    {
                        fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
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
                            fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }  
                    else if(fg_RxBuffer[20] == 0x82 )
                    {
                        for(counter=23; counter < l_Bytes; counter++)
                        {
                            fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    } 
                    else
                    {
                        for(counter=21; counter < l_Bytes; counter++)
                        {
                            fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
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
                            fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    }  
                    else if(fg_RxBuffer[20] == 0x82 )
                    {
                        for(counter=23; counter < l_Bytes; counter++)
                        {
                            fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
                        }
                    } 
                    else
                    {
                        for(counter=21; counter < l_Bytes; counter++)
                        {
                            fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
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
        		//ClrLn(2);
        		// ErrPrt(1,1,"DATA DOWNLOADING....\n");
                // ErrPrt(1,2,"VOLUME:");
                // ErrPrt(1,3,"DON'T PRESS ANY KEY\nDURING DOWNLOADING");
        		PGbar = 0;
        	}
        	// ErrPrt(8,2,"");printf("%lu BYTES",(long)VolumeOfData);
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
			fclose(fp5);
				return(0);
			}
			l_Bytes = ((( fg_RxBuffer[1] & 0x07)*0x100) + fg_RxBuffer[2] - 1);
			FrameTypeFile(fg_RxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
			
			for(counter=8; counter < l_Bytes; counter++)
			{
				fprintf(fp5,"%02X", fg_RxBuffer[counter]);VolumeOfData +=2;
			}
		}//While(0xA8)
		
	}//while(moreblock)
	fclose(fp5);
	// memcpy(&buffer,&fg_RxBuffer,sizeof(buffer));
	return 1;
}

void FrameTypeFile(_UBYTE *cp, _UBYTE &rrr_c, _UBYTE &rrr_c1, _UBYTE &rrr_s, _UBYTE &sss_c, _UBYTE &sss_c1, _UBYTE &sss_s)
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

unsigned char  MetersDownloaded (void)
{
	/*
   char done,i=0,z=0,x=1;
   struct ffblk a; ///mundel

   LcdClear();
   done = findfirst("\\DLMS\\*.ddt",&a,0);
   //Memory_status();

   memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
   sprintf((char*)g_DispBuffer, "Total Downloaded(%d)",NoOfMeter());
   HAL_DebugPrint(1,(_SBYTE*) g_DispBuffer);

   while(!done)
   {    
    z++;
	printf("(%d) ",z);
	for(i=0;i<8;i++)
	 {		 
	  if(a.ff_name[i] == 46)
	  {
		break;
	  }
	  else if(a.ff_name[i] != 46)
	  {
		printf("%c",a.ff_name[i]);
	  }
	 }
	 x = z%8;
	 if(x==0)
	 {
	  puts("\n Enter for next");
	  _getkey();
	  LcdClear();
	  printf("Total Meters(%d)",NoOfMeter());  
	 }
	 printf("\n");	 
	 i=0;
     done = findnext(&a);	
   }    
   _getkey();
      	*/

   int l_Count = 0;
   int l_LineNo = 0;
   char *l_RetVal;
   int l_TempVal = 0;
   DIR *dir;
   struct dirent *ent;
   unsigned char l_Tempfilename[200][13];//[30][13]
   unsigned char l_TempBuffer[45];

	LcdClear();
	memset(l_TempBuffer, 0, sizeof(l_TempBuffer));
	l_TempVal = NoOfMeter();
	sprintf((char*)l_TempBuffer, "Total Meters(%d)", l_TempVal);
	HAL_DebugPrint(1, (_SBYTE*) l_TempBuffer);

	if((dir = opendir("/DLMS")) != NULL)
	{
		l_LineNo = 2;
		while((ent = readdir(dir)) != NULL)
		{
			strcpy((_SBYTE*)l_Tempfilename[0], (const char*)ent->d_name);
			if(l_Tempfilename[0][0] != '.')
			{
				l_Count++;
				//fprintf(stderr, "\n%s", ent->d_name);
				memset(l_TempBuffer, 0, sizeof(l_TempBuffer));
				sprintf((char*)l_TempBuffer, "%d)%s", l_Count, ent->d_name);
				l_RetVal = strstr((_SBYTE*)l_TempBuffer, ".ddt");
				if(l_RetVal)
				{
					*l_RetVal = 0;
				}
				HAL_DebugPrint(l_LineNo, (_SBYTE*) l_TempBuffer);
				l_LineNo++;
				if(!(l_LineNo % 7))
				{
					l_LineNo = 2;
					HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
					_getkey();
					LcdClear();
					memset(l_TempBuffer, 0, sizeof(l_TempBuffer));
					sprintf((char*)l_TempBuffer, "Total Meters(%d)", l_TempVal);
					HAL_DebugPrint(1, (_SBYTE*) l_TempBuffer);
				}
			}
		}
		closedir(dir);
	}
	else
	{
		perror("Could not open directory");
		return 0;
	}
	HAL_DebugPrint(7, (_SBYTE*)"PRESS ANY KEY..");
	_getkey();
}
//=============================================================

int ReadBillingDataB(void)
{
	char result = 0;

    result = GetRequest(0x07, 0x01, 0x00, 0x62, 0x01, 0x00, 0xff, 0x08);//, write, ref Nameplate

    if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x05, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x05, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	return 1;
}

int ReadHplSpecific(void)
{
    char result =0;

    result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x81, 0x00, 0xff, 0x03);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }

    result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x81, 0x00, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
        result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x80, 0x00, 0xff, 0x03);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }

    result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x80, 0x00, 0xff, 0x02);
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
int ReadBillingDataC(void)
{
	char result = 0;
    result = GetRequest(0x07, 0x01, 0x00, 0x62, 0x01, 0x00, 0xff, 0x08);//, write, ref Nameplate
    	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x06, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x06, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x07, 0x01, 0x00, 0x62, 0x01, 0x00, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x62, 0x01, 0x00, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	// TOD downloading
	result = GetRequest(0x14, 0x00, 0x00, 0x0D, 0x00, 0x00, 0xff, 0x05);//Active tod timing
	fprintf(stderr, "Debug point mundel1\n");
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}
	fprintf(stderr, "Debug point mundel2\n");
	ReadHplSpecific_Billing();//for abt meter puneet 080715
	return 1;
}
int ReadEventData(void)
{
	char result = 0;

	// if(type != 2)// commented by puneet sir
	// {
		// result = GetRequest(0x14, 0x00, 0x00, 0x0D, 0x00, 0x00, 0xff, 0x05);//Active tod timing

		// if(result == 0)
		// {
			// CommError();
			// return 0;
		// }
		// else
		// {
			// result = 0;
		// }
	// }
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x07, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x07, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x00, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x00, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x01, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x01, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x02, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x02, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}
//310115 for  DG Power Events
	result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x0E, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}
// 310115  for  Main Power Events   GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x0F, 0xff, 0x02, write, ref EvtData);//Main Power Events
	result = GetRequest(0x01, 0x01, 0x00, 0x80, 0x80, 0x0F, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}
//////////////////////////////////////////////
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x03, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x03, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x04, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x04, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x05, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x05, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x06, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x06, 0xff, 0x02);

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
//==============================================================

int ReadLoadSurvey(int DateFlag)
{
	char result = 0;//1.0.0.8.0.255
	result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x02, 0x00, 0xff, 0x03);
	if(result == 0)//1.0.99.2.0.255     0x01, 0x00, 0x63, 0x02, 0x00, 0xff  daily load profile
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x02, 0x00, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;


	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x04, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return (0);
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x04, 0xff, 0x02);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x01, 0x00, 0xff, 0x03);

	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
	{
		result = 0;
	}

	if(DateFlag == 3)
	{
		LoadSurveyFlag = 1;
	}

	result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x01, 0x00, 0xff, 0x02);

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
//===========================================================================
int ReadHplSpecific_Billing(void)//puneet 080715 change for  ABT meter
{
    char result =0;
    fprintf(stderr, "Debug point mundel3\n");
    result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x83, 0x00, 0xff, 0x03);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
    fprintf(stderr, "Debug point mundel4\n");
    result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x83, 0x00, 0xff, 0x02);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }
        result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x82, 0x00, 0xff, 0x03);
    if(result == 0)
    {
        CommError();
        return 0;
    }
    else
    {
        result = 0;
    }

    result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x82, 0x00, 0xff, 0x02);
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
