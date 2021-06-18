
#include <sys/time.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include "Config.h"
#include "pdsequivalent.h"
#include "typedef.h"
#include "utility.h"
#include "global.h"

//#include_next


#define isleapyear(year) ((!(year % 4) && (year % 100)) || (!(year % 400)))

static _UBYTE fg_RTCInitialized;
static _SINT fg_rtcHandle;

const _UINT fg_UTCDay[] = {0,31,59,90,120,151,181,212,243,273,304,334,365};
const _UINT fg_UTCLeapDay[] = {0,31,60,91,121,152,182,213,244,274,305,335,366};

const unsigned int fg_UTCMonth[] 	  = {31,28,31,30,31,30,31,31,30,31,30,31};
const unsigned int fg_UTCLeapMonth[] = {31,29,31,30,31,30,31,31,30,31,30,31};
//==========================================================================================
//a_Len is the length of a_pSource : 0x33445566778899 to "33445566778899"
void UTL_ConvertHexStrToASCHexStr (_UBYTE* a_pDest, _UBYTE* a_pSrc, _UBYTE a_Len)
{
	_UBYTE l_Count, l_Byte, l_Nibble;

	for (l_Count = 0x00; l_Count < a_Len; l_Count++)
	{
		l_Byte = *(a_pSrc + l_Count);
		l_Nibble = (l_Byte & 0xF0);		//Higher Nibble
		l_Nibble >>= 4;					//Right Shift 4 Bits

		if(l_Nibble <= 0x09)	//(l_Nibble >= 0x00) &&
		{
			*a_pDest++ = (l_Nibble + 0x30);
		}
		else if( (l_Nibble >= 0x0A) && (l_Nibble <= 0x0F) )
		{
			*a_pDest++ = l_Nibble + 'A' - 10;
		}

		//Processing for Lower Nibble
		l_Nibble = (l_Byte & 0x0F);		//Higher Nibble

		if(l_Nibble <= 0x09)	//(l_Nibble >= 0x00) &&
		{
			*a_pDest++ = (l_Nibble + 0x30);
		}
		else if( (l_Nibble >= 0x0A) && (l_Nibble <= 0x0F) )
		{
			*a_pDest++ = l_Nibble + 'A' - 10;
		}
	}

	return;
}

//==========================================================================================
//Conver Integer value to any base (8, 10, 16 etc)
void UTL_ConvertIntToASCDec(_UBYTE* a_pDest,_UINT a_Value)
{
	_UBYTE l_Index, l_Reminder, l_Len, *l_pPTR;
	_UINT l_Value;

	l_Index = 0x00;
	l_Len = 0x00;
	l_Value = a_Value;
	l_pPTR = a_pDest;

	do
	{
		l_Reminder = l_Value % 10;
		l_Value -= l_Reminder;
		l_Value /= 10;

		if(l_Reminder <= 0x09)
		{
			l_Reminder += 0x30;
		}
		else
		{
			l_Reminder += 55;
		}

		*a_pDest = l_Reminder;
		a_pDest++;
		l_Len++;
	} while (l_Value);

	a_pDest --;

	for (l_Index = 0x00; l_Index < (l_Len/2); l_Index++)
	{
		l_Reminder = *a_pDest;
		*a_pDest = *l_pPTR;
		*l_pPTR = l_Reminder;
		a_pDest --;
		l_pPTR ++;
	}

	return;
}

//==========================================================================================
//"12345678" to 0x12345678
void UTL_ConvertASCHexToHex (_UBYTE* a_pDest, _UBYTE* a_pSrc, _UBYTE a_Len)
{
	_UBYTE l_Count, l_ByteVal, l_Temp;

	if ((a_Len % 2) != 0) a_Len++;	//make the length even

	for(l_Count = 0x00; l_Count < a_Len; l_Count++)
	{
		l_ByteVal = *a_pSrc;

		if( (l_ByteVal >= '0') && (l_ByteVal <= '9') )
		{
			l_ByteVal -= 0x30;
		}
		else if( (l_ByteVal >= 'A') && (l_ByteVal <= 'F') )
		{
			l_ByteVal -= 55;
		}
		else if( (l_Temp >= 'a') && (l_Temp <= 'f') )
		{
			l_Temp -= 'a';
		}

		l_ByteVal <<= 4;		//MSB Nibble

		a_pSrc++;			//process for LSB Nibble

		l_Temp = *a_pSrc;

		if( (l_Temp >= 0x30) && (l_Temp <= 0x39) )
		{
			l_Temp -= 0x30;
		}
		else if( (l_Temp >= 'A') && (l_Temp <= 'F') )
		{
			l_Temp -= ('A' - 10);
		}
		else if( (l_Temp >= 'a') && (l_Temp <= 'f') )
		{
			l_Temp -= ('a' - 10);
		}

		l_ByteVal |= l_Temp ;		//ORing lower Nibble

		*a_pDest = l_ByteVal;
		a_pDest++;
		a_pSrc++;
	}

	return;
}

//==========================================================================================
//Reverses a string, from a_pStart to a_Len
void UTL_ReverseString (_UBYTE* a_pStart, _UBYTE a_Len)
{
	_UBYTE l_Index, l_Temp, *l_pEnd;

	l_pEnd = (a_pStart + a_Len -1);

	for (l_Index = 0x00; l_Index < (a_Len/2); l_Index++)
	{
		l_Temp = *a_pStart;
		*a_pStart = *l_pEnd;
		*l_pEnd = l_Temp;
		a_pStart ++;
		l_pEnd --;
	}

	return;
}
//==========================================================================================
//returns 1 on success - if both matches, otherwise returns 0
_UBYTE UTL_CompareString(_UBYTE* a_pStr1, _UBYTE* a_pStr2, _UBYTE a_Len)
{
	_UBYTE l_Count;

	for (l_Count = 0x00; l_Count < a_Len; l_Count++)
	{
		if(!(*(a_pStr1 + l_Count) == *(a_pStr2 + l_Count)))
		{
			return(0);
		}
	}

	return(1);
}

//==============================================================================
void UTL_Wait(_SINT a_miliSec)
{
	long l_uSec;

	l_uSec = a_miliSec * 1000;

	usleep(l_uSec);

	return;
}

//==============================================================================
static _UINT fg_PBMaxValue, fg_PBMinValue;
float fg_UnitCount;
static _UBYTE fg_LineNo;

#define MAX_ROW_DOTS	128


void UTL_PBIncreament (_UBYTE a_Count)
{
	return;
}

void UTL_PBInit(_UINT a_MaxVal, _UINT a_MinVal, _UBYTE a_LineNo)
{
	fg_PBMaxValue = a_MaxVal;
	fg_PBMinValue = a_MinVal;
	fg_LineNo = a_LineNo;

	fg_UnitCount = (a_MaxVal - a_MinVal) / MAX_ROW_DOTS;

	return;
}


//==============================================================================
//RTC Related Functions
//==============================================================================
void UTL_RTCInitialize (void)
{
	if (fg_RTCInitialized == 0x01) return;

	//RTC: Reading Date/Time
	//Open device RTC
	fg_rtcHandle = open("/dev/rtc", O_RDONLY);

	if (fg_rtcHandle == -1)
	{
		perror("RTC Reading problem");
		//_getkey();
	}

	//ioctl(fg_rtcHandle, RTC_RD_TIME, &fg_RTC);

	return;
}

//=======================================================================
void UTL_RTCRead(struct rtc_time* a_sRTC)
{
//	ioctl(fg_rtcHandle, RTC_RD_TIME, a_sRTC);
//	return;
//	system("chmod 777 /dev/rtc");
	fg_rtcHandle = open("/dev/rtc", O_RDONLY);

	if (fg_rtcHandle == -1)
	{
		perror("RTC Reading problem /dev/rtc.. ");
		//_getkey();
	}
	else
	{

		ioctl(fg_rtcHandle, RTC_RD_TIME, a_sRTC);
		close(fg_rtcHandle);
	}

	return;
}

//==============================================================================
//Returns HHU time in UTC Format
_ULONG UTL_GetHHUUTCTime(void)
{
	_UINT l_iDD, l_iMM, l_iYY, l_iLeapDay;
	_ULONG l_lTimeinSecond, l_lTempLongUTC;
	_UBYTE l_Hour,l_Minute;
	_UINT l_Count;

	UTL_RTCRead (&g_RTC);		//Read RTC of HHU

	l_Minute = g_RTC.tm_min;
	l_Hour = g_RTC.tm_hour;
	l_iYY = g_RTC.tm_year;
	if(l_iYY >= 100)
	l_iYY -= 100;  				//Reference year 2000 but in linux 2012 read as 112 so we write 100
	l_iLeapDay = 0;

	for(l_Count = 2000; l_Count < (l_iYY+2000); l_Count++)  //calculate leap day
	{
		//if( ((l_Count)%4) == 0 )
		if(isleapyear(l_Count))
		{
			l_iLeapDay++;
		}
	}

	l_iMM = g_RTC.tm_mon ; //-1  discard because january 0 in linux
	l_iDD = g_RTC.tm_mday -1; //dare
	l_lTempLongUTC      = l_iYY;
	l_lTimeinSecond  = (l_lTempLongUTC * 365 * 24 * 3600);

	if(isleapyear(l_Count))	//if((l_Count % 4) == 0x00)		//current year If Leap Year
	{
		l_lTempLongUTC = fg_UTCLeapDay[l_iMM];
	}
	else
	{
		l_lTempLongUTC = fg_UTCDay[l_iMM];
	}

	l_lTimeinSecond += (l_lTempLongUTC*24*3600);
	l_lTempLongUTC      = l_iDD;

	l_lTimeinSecond += (l_lTempLongUTC*24*3600);
	l_lTempLongUTC      =  l_Hour;

	l_lTimeinSecond += (l_lTempLongUTC*3600);
	l_lTempLongUTC      = l_Minute;

	l_lTimeinSecond += (l_lTempLongUTC*60);
	l_lTempLongUTC      = l_iLeapDay;

	l_lTimeinSecond += (l_lTempLongUTC*24*3600);
	l_lTimeinSecond -= 19800;	//Subtracted +5.30 Hrs - India Time is ahead of 5 hrs 30 mins
	l_lTimeinSecond += g_RTC.tm_sec;

	return(l_lTimeinSecond);
}
//===============================================================
void ConvertUTCtoRTC(_ULONG a_UTCinSeconds, RtcTypeDef *a_pRtcStuct)
{
	_ULONG l_UTCTime = 0;
	unsigned long YearSec,MonthSec,RemainSec;
	unsigned int day,month,year;
	unsigned char hour,minute;

	l_UTCTime = a_UTCinSeconds;
	//l_UTCTime += 19800; //Add 5:30 for Indian time

	year= 0;
	YearSec = 0;
	for(int l_Count = 2000; ; l_Count++)
	{
		year = l_Count;
		if(isleapyear(l_Count))
		{
			YearSec +=31622400;
			if(YearSec > l_UTCTime)
			{
				YearSec -=31622400;
				break;
			}
		}
		else
		{
			YearSec +=31536000;
			if(YearSec > l_UTCTime)
			{
				YearSec -=31536000;
				break;
			}
		}
	}
	RemainSec = l_UTCTime - YearSec;//(l_UTCTime % 31536000);			//(365*24*3600) 	//calculate remaining seconds after year calculate
	month = 0 ;
	MonthSec = 0;
	if(isleapyear((year)))
	{
		for(unsigned long l_Count1 = 0; ; l_Count1++)
		{
			month++;
			MonthSec +=(fg_UTCLeapMonth[l_Count1]*86400);

			if((MonthSec > RemainSec)||(month > 12))
			{
				MonthSec -=(fg_UTCLeapMonth[l_Count1]*86400);
				break;
			}
		}
	}
	else
	{
		for(unsigned long l_Count2 = 0; ; l_Count2++)
		{
			month++;
			MonthSec +=(fg_UTCMonth[l_Count2]*86400);

			if((MonthSec > RemainSec)||(month > 12))
			{
				MonthSec -=(fg_UTCMonth[l_Count2]*86400);
				break;
			}
		}
	}

	RemainSec = RemainSec - MonthSec; //Remaining seconds after month calculate
	day = (unsigned int)(RemainSec/86400);
	day += 1;
	RemainSec = (RemainSec % 86400);
	hour = (unsigned int) (RemainSec/3600);
	RemainSec = (unsigned int) (RemainSec%3600);
	minute = (RemainSec/60);
	RemainSec = (RemainSec%60);

	a_pRtcStuct->Day = day;
	a_pRtcStuct->Month = month;
	a_pRtcStuct->Year = (year - 2000);
	a_pRtcStuct->Hour = hour;
	a_pRtcStuct->Minute = minute;
	a_pRtcStuct->Second = RemainSec;
}
//=================================================================
