
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include "kt_misc_func.h"
#include "kt_comm.h"
#include "functh.h"
#include "kt_global.h"
#include "kt_libUART.h"

#define PPPINITFCS16 0xffff /*initial FCS value*/
#define PPPGOODFCS16 0xf0b8 /*Good final FCS value*/
//#define P 0x8408
#define isleapyear(year) ((!(year % 4) && (year % 100)) || (!(year % 400)))

unsigned int fcstab[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78,
};

//jena start
FILE *fp, *fp1;
unsigned char LoadSurveyFlag;
unsigned char FileName[20];
unsigned int g_intFlg;
unsigned long head, tail;
unsigned int YYYY1, YYYY2;\
unsigned char MM1, MM2, DD1, DD2;
unsigned int count;

//jena end

unsigned char SendBuffer[] ={0x7e,0xA0,0x07,0x03,0x41,0x93,0x5A,0x64,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xFF,0xAA,0xBB,0xFE,0x03,0x03,0x03,0x03,0xFF,0xAA,0xBB,0xFE,0x05,0x05,0x05,0x05,0xFF,0xAA,0xBB, 0xFE,0x0B,0x0B,0x0B,0x0B,0xFF,0xFE,0x06,0x06,0x06,0x06,0xFF,0xAA,0xBB,0xFE,0x07,0x07,0x07,0x07,0xFF,0xAA,0xBB,0xFE,0x0c,0x0c,0x0c,0x0c,0xff,0xAA,0xBB,0xFE,0x0F,0x0F,0x0F,0x0F,0xFF,0xFE,0x13,0x13,0x13,0x13,0xFF,0xFE,0x14,0x14,0x14,0x14,0xFF,0xFE,0x11,0x11,0x11,0x11,0xFF,0xFE,0x12,0x12,0x12,0x12,0xFF,0xFE,0x19,0x19,0x19,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF};
unsigned char buffer[] ={0x7e,0xA0,0x07,0x03,0x41,0x93,0x5A,0x64,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xFF,0xAA,0xBB,0xFE,0x03,0x03,0x03,0x03,0xFF,0xAA,0xBB,0xFE,0x05,0x05,0x05,0x05,0xFF,0xAA,0xBB, 0xFE,0x0B,0x0B,0x0B,0x0B,0xFF,0xFE,0x06,0x06,0x06,0x06,0xFF,0xAA,0xBB,0xFE,0x07,0x07,0x07,0x07,0xFF,0xAA,0xBB,0xFE,0x0c,0x0c,0x0c,0x0c,0xff,0xAA,0xBB,0xFE,0x0F,0x0F,0x0F,0x0F,0xFF,0xFE,0x13,0x13,0x13,0x13,0xFF,0xFE,0x14,0x14,0x14,0x14,0xFF,0xFE,0x11,0x11,0x11,0x11,0xFF,0xFE,0x12,0x12,0x12,0x12,0xFF,0xFE,0x19,0x19,0x19,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF};
unsigned char llssecret[16];

unsigned char rrr_c=0, rrr_c1=0, rrr_s=0, sss_c=0, sss_c1=0, sss_s=0;
//int fcs(unsigned char *cp, int length, int flag);
//jena void FrameType(unsigned char *cp, unsigned char* rrr_c, unsigned char* rrr_c1, unsigned char* rrr_s, unsigned char* ss_c, unsigned char* sss_c1, unsigned char* sss_s);


void ErrPrt(int x, int y, char *str1)
{
	//clrscr();
	//gotoxy(x,y);
	printf("%s", str1);
}

int  isdatevalid(int month, int day, int year)
{
  if(!(year >= 2000 && year <= 2020)) return 0;
  if (day <= 0) return 0 ;
  switch( month )
	{
	  case 1  :
	  case 3  :
	  case 5  :
	  case 7  :
	  case 8  :
	  case 10 :
	  case 12 : if (day > 31) return 0 ; else return 1 ;
	  case 4  :
	  case 6  :
	  case 9  :
	  case 11 : if (day > 30) return 0 ; else return 1 ;
	  case 2  : 
	    if ( day > 29 ) return 0 ;
      if ( day < 29 ) return 1 ;
      if (isleapyear(year)) return 1 ;   // leap year
    else return 0 ;
	}
  return 0 ;
}

int selScr(void)
{
	int Choice = 5;
	while(1)
	{
		//clrscr();
		ErrPrt(1,1,"1.SELECTED DATA");
		printf("\n2.ALL DATA\n0.EXIT\nENTER CHOICE: ");
		//cin >> Choice;
		scanf("%d",&Choice);
		if(Choice >= 0 && Choice <= 2)
			return Choice;
		else
		{
			//clrscr();
			ErrPrt(1,1,"\nWRONG CHOICE  \nPRESS ANY KEY..");
			fflush(stdin);
			//getch();
		}
	}
}

int selScrSelected(void)
{
	int Choice = 5;	
	while(1)
	{
		//clrscr();
		ErrPrt(1,1,"1.BILLING DATA");
		printf("\n2.EVENT DATA\n3.LOAD SURVEY DATA\n0.EXIT\nENTER CHOICE: ");
		//cin >> Choice;
		scanf("%d",&Choice);
		if(Choice >= 0 && Choice <= 3)
			return Choice;
		else
		{
			//clrscr();
			ErrPrt(1,1,"\nWRONG CHOICE  \nPRESS ANY KEY..");
			fflush(stdin);
			//getch();
		}
	}
}


void CommError(void)
{
	ErrPrt(1,1,"\nCOMMUNICATION ERROR.\nTRY AGAIN.\nPRESS ANY KEY..");
	fclose(fp);
	remove("\\example.ddt");
	//getch();
}

int ReadLoadSurvey(int DateFlag)
{
	int result = 0;
//	printf("\nLOAD SURVEY DATA\n");
//	fprintf(fp,"\nLOAD SURVEY DATA");
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x04, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x04, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x01, 0x00, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	if(DateFlag == 3)
		LoadSurveyFlag = 1;
	result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x01, 0x00, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
    return 1;

}

int ReadEventData(void)
{
	int result = 0;
//	printf("\nEVENT DATA\n");
//	fprintf(fp,"\nEVENT DATA");
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x07, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x07, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x00, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x00, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x01, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x01, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;		
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x02, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x02, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;		
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x03, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x03, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;		
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x04, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x04, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;		
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x05, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x05, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;		
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x06, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x00, 0x00, 0x63, 0x62, 0x06, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	return 1;

}

int ReadBillingDataB(void)
{
	int result = 0;
//	printf("\nBILLING DATA\n");
//	fprintf(fp,"\nBILLING DATA");
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x05, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x05, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	result = GetRequest(0x07, 0x01, 0x00, 0x63, 0x02, 0x00, 0xff, 0x03);
	if(result == 0)
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
	return 1;
}

int ReadBillingDataC(void)
{
	int result = 0;
//	printf("\nBILLING DATA\n");
//	fprintf(fp,"\nBILLING DATA");
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
	return 1;
}


int ReadInstantData(void)
{
	int result = 0;
	long Serialno = 0, digit=0;
	memset(FileName, 0x30, sizeof(FileName));

//	printf("\nINSTANT DATA\n");
//	fprintf(fp,"\nINSTANT DATA");
	result = GetRequest(0x01, 0x00, 0x00, 0x60, 0x01, 0x00, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	Serialno = (long)((long)buffer[16]*65536*256 + (long)buffer[17]*65536 + (long)buffer[18]*256 + (long)buffer[19]);

	digit = countDigits(Serialno,8);

	// jena ltoa(Serialno,FileName+14-digit ,10);
	FileName[14] = '.';
	FileName[15] = 'd';
	FileName[16] = 'd';
	FileName[17] = 't';
	FileName[18] = '\0';

	result = GetRequest(0x01, 0x00, 0x00, 0x60, 0x01, 0x01, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x02, 0x00, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	FileName[6] = buffer[17];
	FileName[0] = '\\';
	FileName[1] = 'D';
	FileName[2] = 'L';
	FileName[3] = 'M';
	FileName[4] = 'S';
	FileName[5] = '\\';
	
	result = GetRequest(0x01, 0x00, 0x00, 0x5E, 0x5B, 0x09, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x04, 0x02, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x01, 0x01, 0x00, 0x00, 0x04, 0x03, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x01, 0x00, 0x00, 0x60, 0x01, 0x04, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x03, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x03, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x00, 0xff, 0x03);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;

	result = GetRequest(0x07, 0x01, 0x00, 0x5E, 0x5B, 0x00, 0xff, 0x02);
	if(result == 0)
	{
		CommError();
		return 0;
	}
	else
		result = 0;
	return 1;
}

uint16_t pppfcs16(uint16_t fcs, uint8_t *cp, int length)
{
	//jena : need to relook the function, all 3 lines are commented
	//while(length--)
	//	fcs = (fcs >> 8) ^ fcstaintFlgb[(fcs ^ *cp++) & 0xff];
	return fcs;
}

int fcs(unsigned char *cp, int length, int flag)
{
	unsigned int trialfcs;

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
			return 1;
		else
			return 0;
	}

}

int SNRM(void)
{
	unsigned long time2 = 0;
					
	ClearPacket();
	printf("|");
	SendBuffer[0] = 0x7e;
	SendBuffer[1] = 0xa0;
	SendBuffer[2] = 0x07;
	SendBuffer[3] = 0x03;
	SendBuffer[4] = 0x41;
	SendBuffer[5] = 0x93;
	SendBuffer[6] = 0x00;
	SendBuffer[7] = 0x00;
	SendBuffer[8] = 0x7e;
	fcs(SendBuffer+1, 5, 1);
	kt_libUART_SendData(g_portNo, SendBuffer, 9);

	delay(500);
	g_intFlg = 0;

	while((g_intFlg == 0) && (time2 < 68265))
		time2++;
	if(time2>=68265)
	{
		errFlg = 1;
		return 0;
	}

	Read(buffer);
/*	for(int i=0;i<buffer[2]+2;i++)
		printf("%02x ",buffer[i]);
	printf("\n");
*/	if(buffer[5] == 0x73)
		return 1;
	else
		return 0;
}

int AARQ(void)
{
	unsigned char c=0;
	unsigned long time2 = 0;
	ClearPacket();
	printf("|");
	SendBuffer[c++] = 0x7E;
	SendBuffer[c++] = 0xA0;
	SendBuffer[c++] = 0x00;
	SendBuffer[c++] = 0x03;
	SendBuffer[c++] = 0x41;
	SendBuffer[c++] = 0x10;
	SendBuffer[c++] = 0x00;	//FCS
	SendBuffer[c++] = 0x00;	//FCS
	SendBuffer[c++] = 0xE6;	//LLC Bytes
	SendBuffer[c++] = 0xE6;
	SendBuffer[c++] = 0x00;
	SendBuffer[c++] = 0x60; //AARQ
	SendBuffer[c++] = 0x11;	// Assign Below
	SendBuffer[c++] = 0xA1; //Application Context Name Tag
	SendBuffer[c++] = 0x09;
	SendBuffer[c++] = 0x06;
	SendBuffer[c++] = 0x07;
	SendBuffer[c++] = 0x60;
	SendBuffer[c++] = 0x85;
	SendBuffer[c++] = 0x74;
	SendBuffer[c++] = 0x05;
	SendBuffer[c++] = 0x08;
	SendBuffer[c++] = 0x01;
	SendBuffer[c++] = 0x01;

	SendBuffer[c++] = 0x8A;
	SendBuffer[c++] = 0x02;
	SendBuffer[c++] = 0x07;
	SendBuffer[c++] = 0x80;
	SendBuffer[c++] = 0x8B;
	SendBuffer[c++] = 0x07;
	SendBuffer[c++] = 0x60;
	SendBuffer[c++] = 0x85;
	SendBuffer[c++] = 0x74;
	SendBuffer[c++] = 0x05;
	SendBuffer[c++] = 0x08;
	SendBuffer[c++] = 0x02;
	SendBuffer[c++] = 0x01;
	SendBuffer[c++] = 0xAC;
	SendBuffer[c++] = strlen((const char*) llssecret) + 2;
	SendBuffer[c++] = 0x80;
	SendBuffer[c++] = strlen((const char*) llssecret);

	for (int x = 0; x < strlen((const char*) llssecret); x++)
		SendBuffer[c++] = llssecret[x];

	SendBuffer[c++] = 0xBE;
	SendBuffer[c++] = 0x10;
	SendBuffer[c++] = 0x04;
	SendBuffer[c++] = 0x0E;
	SendBuffer[c++] = 0x01;
	SendBuffer[c++] = 0x00;
	SendBuffer[c++] = 0x00;
	SendBuffer[c++] = 0x00;
	SendBuffer[c++] = 0x06;
	SendBuffer[c++] = 0x5F;
	SendBuffer[c++] = 0x1F;
	SendBuffer[c++] = 0x04;
	SendBuffer[c++] = 0x00;
	SendBuffer[c++] = 0x00;
	SendBuffer[c++] = 0x18;
	SendBuffer[c++] = 0x1D;
	SendBuffer[c++] = 0xFF;
	SendBuffer[c++] = 0xFF;

	SendBuffer[2] = c+1;
	SendBuffer[12] = c-13;
	SendBuffer[c+2] = 0x7E;

	fcs(SendBuffer+1, 5, 1);
	fcs(SendBuffer+1,c-1, 1);

	//SendPkt(c+3);
	kt_libUART_SendData(g_portNo, SendBuffer, c+3);

	delay(500);

	while((intFlg == 0) && (time2 < 655350))   //68265 //655350
		time2++;
	if(time2>=655350)
	{
		errFlg=1;
		return 0;
	}
	Read(buffer);
/*	for(int i=0;i<buffer[2]+2;i++)
		printf("%02x ",buffer[i]);
*/	if(buffer[28] == 0x00)
	{
		//jena FrameType(buffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
		return 1;
	}
	else
		return 0;
}

int DISC(void)
{
	unsigned long time2 = 0;
	ClearPacket();
	SendBuffer[0] = 0x7e;
	SendBuffer[1] = 0xa0;
	SendBuffer[2] = 0x07;
	SendBuffer[3] = 0x03;
	SendBuffer[4] = 0x41;
	SendBuffer[5] = 0x53;
	SendBuffer[6] = 0x00;
	SendBuffer[7] = 0x00;
	SendBuffer[8] = 0x7e;
	fcs(SendBuffer+1, 5, 1);
	//SendPkt(9);
	kt_libUART_SendData(g_portNo, SendBuffer, 9);

	delay(500);

	while((intFlg == 0) && (time2 < 68265))   //68265 //655350
		time2++;

	if(time2>=68265)
	{
		errFlg=1;
		return 0;
	}

	Read(buffer);

//	for(int i=0;i<buffer[2]+2;i++)
//		printf("%02x ",buffer[i]);
	//printf("\nDISC");
	if(buffer[5] == 0x73)
		return 1;
	else
		return 0;
}

unsigned char Encrypt(unsigned char k)
{
/*	if (k >= 48 && k <= 57)
		 k = k - 48;
	else if (k >= 97 && k <= 122)
		 k = k - 55;
	else if (k >= 65 && k <= 90)
		 k = k - 50;
	else
		 k = k + 95;*/

	if (k >= 0 && k < 9)
		 k++;
	else if(k==9)
		k = 14;
/*	else if (k >= 97 && k <= 122)
		 k = k - 55;*/
	else if (k >= 10 && k <= 15)
		 k = k + 5;
	return k;

}

int GetRequest(unsigned char classID, unsigned char Obis_a, unsigned char Obis_b, unsigned char Obis_c, unsigned char Obis_d, unsigned char Obis_e,unsigned char Obis_f,unsigned char Attr)
{
	unsigned long datablockno=0;
	unsigned char moreblock = 0, ret_lsh=0;
	int counter=0;
	unsigned long time2=0;
	ClearPacket();
	
	//if(PGbar % 20 == 0)
	//{
	//	ErrPrt(1,1,"\nDATA DOWNLOADING\n\n");
	//	PGbar = 0;
	//}
	//printf("|");PGbar++;
	
	SendBuffer[0] = 0x7E;
	SendBuffer[1] = 0xA0;
	SendBuffer[3] = 0x03;
	SendBuffer[4] = 0x41;
	ret_lsh = rrr_s << 5;
	SendBuffer[5] = ret_lsh | 0x10;
	ret_lsh = sss_s << 1;
	SendBuffer[5] = ret_lsh | SendBuffer[5];
	SendBuffer[6] = 0x00;
	SendBuffer[7] = 0x00;
	SendBuffer[8] = 0xE6;
	SendBuffer[9] = 0xE6;
	SendBuffer[10] = 0x00;
	SendBuffer[11] = 0xC0;
	SendBuffer[12] = 0x01;
	SendBuffer[13] = 0x81;
	SendBuffer[14] = 0x00;
	SendBuffer[15] = classID; 
	SendBuffer[16] = Obis_a;
	SendBuffer[17] = Obis_b;
	SendBuffer[18] = Obis_c;
	SendBuffer[19] = Obis_d;
	SendBuffer[20] = Obis_e;
	SendBuffer[21] = Obis_f;
	SendBuffer[22] = Attr;
	
	if(LoadSurveyFlag == 0)
	{
		SendBuffer[23] = 0x00;
		SendBuffer[2] = 0x19;
		SendBuffer[26] = 0x7e;
		fcs(SendBuffer+1, 5, 1);
		fcs(SendBuffer+1, 23, 1);
		//SendPkt(27);
		kt_libUART_SendData(g_portNo, SendBuffer, 27);
	}
	else
	{
		LoadSurveyFlag = 0;
		SendBuffer[23] = 0x01;
		SendBuffer[24] = 0x01;
		SendBuffer[25] = 0x02;
		SendBuffer[26] = 0x04;
		SendBuffer[27] = 0x02;
		SendBuffer[28] = 0x04;
		SendBuffer[29] = 0x12;
		SendBuffer[30] = 0x00;
		SendBuffer[31] = 0x08;
		SendBuffer[32] = 0x09;
		SendBuffer[33] = 0x06;
		SendBuffer[34] = 0x00;
		SendBuffer[35] = 0x00;
		SendBuffer[36] = 0x01;
		SendBuffer[37] = 0x00;
		SendBuffer[38] = 0x00;
		SendBuffer[39] = 0xFF;
		SendBuffer[40] = 0x0F;
		SendBuffer[41] = 0x02;
		SendBuffer[42] = 0x12;
		SendBuffer[43] = 0x00;
		SendBuffer[44] = 0x00;

		SendBuffer[45] = 0x09;
		SendBuffer[46] = 0x0C;
		SendBuffer[47] = YYYY1/256;
		SendBuffer[48] = (unsigned char) YYYY1%256;
		SendBuffer[49] = (unsigned char) MM1;
		SendBuffer[50] = (unsigned char) DD1;
		SendBuffer[51] = 0xFF;
		SendBuffer[52] = 0x00;
		SendBuffer[53] = 0x00;
		SendBuffer[54] = 0x00;
		SendBuffer[55] = 0x00;
		SendBuffer[56] = 0xFF;
		SendBuffer[57] = 0xFF;
		SendBuffer[58] = 0xFF;

		SendBuffer[59] = 0x09;
		SendBuffer[60] = 0x0C;
		SendBuffer[61] = (YYYY2/256);
		SendBuffer[62] = (unsigned char) (YYYY2%256);
		SendBuffer[63] = (unsigned char) MM2;
		SendBuffer[64] = (unsigned char) DD2;
		SendBuffer[65] = 0xFF;
		SendBuffer[66] = 0x00;
		SendBuffer[67] = 0x00;
		SendBuffer[68] = 0x00;
		SendBuffer[69] = 0x00;
		SendBuffer[70] = 0xFF;
		SendBuffer[71] = 0xFF;
		SendBuffer[72] = 0xFF;

		SendBuffer[73] = 0x01;
		SendBuffer[74] = 0x00;
		SendBuffer[75] = 0x00;
		SendBuffer[76] = 0x00;
		SendBuffer[77] = 0x7E;

		SendBuffer[2] = 0x4C;
		fcs(SendBuffer+1, 5, 1);
		fcs(SendBuffer+1, 74, 1);
		//SendPkt(78);
		kt_libUART_SendData(g_portNo, SendBuffer, 78);
	}

	delay(500);
	while((intFlg==0)&&(time2<68265))   //68265 //655350
		time2++;
	if(time2>=68265)
	{
		printf("\nTime Out Error");
		//getch();
		errFlg=1;
		return 0;
	}

	Read(buffer);

	fprintf(fp,"\n");
	for(counter=16;counter<=21;counter++)
		fprintf(fp,"%02X",SendBuffer[counter]);
	fprintf(fp," %02X ",SendBuffer[22]);

	//jena FrameType(buffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

	if (/*bAckRec == true &&*/ buffer[11] == 0xC4 && buffer[12] == 0x02)
	{
		datablockno = (unsigned long) (buffer[15]*0 +((unsigned long)buffer[16])*65536 +((unsigned long)buffer[17])*256 + buffer[18]);
		moreblock = ! buffer[14];
	}

	if (/*bAckRec == true && */buffer[1] == 0xA0)
	{
		for(counter=15;counter<buffer[2]-1;counter++)
			fprintf(fp,"%02X",buffer[counter]);
		return 1;
	}
/*	else if (bAckRec == false)
		return false;*/
	if (moreblock == 1)
	{
		for(counter=23;counter<buffer[2]-1;counter++)
			fprintf(fp,"%02X",buffer[counter]);
	}
	else
	{
		for(counter=15;counter<buffer[2]-1;counter++)
			fprintf(fp,"%02X",buffer[counter]);
	}

	while (buffer[1] == 0xA8)
	{
		ClearPacket();

		SendBuffer[0] = 0x7E;
		SendBuffer[1] = 0xA0;
		SendBuffer[2] = 0x07;
		SendBuffer[3] = 0x03;
		SendBuffer[4] = 0x41;
		ret_lsh = rrr_s << 5;
		SendBuffer[5] = ret_lsh | 0x10;
		SendBuffer[5] = SendBuffer[5] | 0x01;
		fcs(SendBuffer+1, 5, 1);
		SendBuffer[8] = 0x7E;

		//SendPkt(9);
		kt_libUART_SendData(g_portNo, SendBuffer, 9);

		delay(500);
		while((intFlg==0)&&(time2<68265))   //68265 //655350
			time2++;
		if(time2>=68265)
		{
			printf("\nTime Out Error");
			//getch();
			errFlg=1;
			return 0;
		}
		Read(buffer);

		//jena FrameType(buffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

		if (/*bAckRec == true && */buffer[1] == 0xA0)
		{
			for(counter=8;counter<buffer[2]-1;counter++)
				fprintf(fp,"%02X",buffer[counter]);
			break;
		}
		/*else if (bAckRec == false)
			return false;*/
		if (buffer[1] == 0xA8)
		{
			for(counter=8;counter<buffer[2]-1;counter++)
				fprintf(fp,"%02X",buffer[counter]);
		}
	}//While(0xA8)
	while (moreblock)
	{
		moreblock = 0;
		ClearPacket();
	
		SendBuffer[0] = 0x7E;
		SendBuffer[1] = 0xA0;
		SendBuffer[2] = 0x13;
		SendBuffer[3] = 0x03;
		SendBuffer[4] = 0x41;
		ret_lsh = rrr_s << 5;
		SendBuffer[5] = ret_lsh | 0x10;
		ret_lsh = sss_s << 1;
		SendBuffer[5] = ret_lsh | SendBuffer[5];
		SendBuffer[8] = 0xE6;
		SendBuffer[9] = 0xE6;
		SendBuffer[10] = 0x00;
		SendBuffer[11] = 0xC0;
		SendBuffer[12] = 0x02;
		SendBuffer[13] = 0x81;
		SendBuffer[14] = 0x00;
		SendBuffer[15] = 0x00;
		SendBuffer[16] = datablockno / 256;
		SendBuffer[17] = datablockno % 256;
		fcs(SendBuffer+1, 5, 1);
		fcs(SendBuffer+1, 17, 1);
		SendBuffer[20] = 0x7E;

		//SendPkt(21);
		kt_libUART_SendData(g_portNo, SendBuffer, 21);

		delay(500);
		while((intFlg==0)&&(time2<68265))   //68265 //655350
			time2++;
		if(time2>=68265)
		{
			printf("\nTime Out Error");
			//getch();
			errFlg=1;
			return 0;
		}
		Read(buffer);

		//jena FrameType(buffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

		if (/*bAckRec == true &&*/ buffer[11] == 0xC4 && buffer[12] == 0x02)
		{
			datablockno = (unsigned long) (buffer[15]*0 +((unsigned long)buffer[16])*65536 +((unsigned long)buffer[17])*256 + buffer[18]);
			moreblock = ! buffer[14];
		}

		if (/*bAckRec == true &&*/ buffer[1] == 0xA0)
		{
			if (moreblock == 1)
//				for (i = 23; i < buffer[2] - 1; i++)
//					temp += buffer[i].ToString("X2");
				for(counter=23;counter<buffer[2]-1;counter++)
					fprintf(fp,"%02X",buffer[counter]);
			else
//				for (i = 21; i < buffer[2] - 1; i++)
//					temp += buffer[i].ToString("X2");
				for(counter=21;counter<buffer[2]-1;counter++)
					fprintf(fp,"%02X",buffer[counter]);
		}
		if (/*bAckRec == true && */buffer[1] == 0xA8)
		{
			if (moreblock == 1)
//				for (i = 23; i < buffer[2] - 1; i++)
//					temp += buffer[i].ToString("X2");
				for(counter=23;counter<buffer[2]-1;counter++)
					fprintf(fp,"%02X",buffer[counter]);
			else
			{
				if(buffer[20] == 0x81)
//					for (i = 22; i < buffer[2] - 1; i++)
//						temp += buffer[i].ToString("X2");
					for(counter=22;counter<buffer[2]-1;counter++)
						fprintf(fp,"%02X",buffer[counter]);
				else
//					for (i = 23; i < buffer[2] - 1; i++)
//						temp += buffer[i].ToString("X2");
					for(counter=23;counter<buffer[2]-1;counter++)
						fprintf(fp,"%02X",buffer[counter]);
			}

			//break;
		}
/*		else if (bAckRec == false)
			return false;
*/

		while (buffer[1] == 0xA8)
		{
			ClearPacket();

			SendBuffer[0] = 0x7E;
			SendBuffer[1] = 0xA0;
			SendBuffer[2] = 0x07;
			SendBuffer[3] = 0x03;
			SendBuffer[4] = 0x41;
			ret_lsh = rrr_s << 5;
			SendBuffer[5] = ret_lsh | 0x10;
			SendBuffer[5] = SendBuffer[5] | 0x01;
			fcs(SendBuffer+1, 5, 1);
			SendBuffer[8] = 0x7E;

			//SendPkt(9);
			kt_libUART_SendData(g_portNo, SendBuffer, 9);
			delay(500);

			while((intFlg==0)&&(time2<68265))   //68265 //655350
				time2++;
			if(time2>=68265)
			{
				printf("\nTime Out Error");
				//getch();
				errFlg=1;
				return 0;
			}
			Read(buffer);

			//jena FrameType(buffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

			if (1==1/*bAckRec == true*/)
			{
//				for (i = 8; i < buffer[2] - 1; i++)
//					temp += buffer[i].ToString("X2");
				for(counter=8;counter<buffer[2]-1;counter++)
					fprintf(fp,"%02X",buffer[counter]);
			}
//			else if (bAckRec == false)
//				return false;
		}//While(0xA8)
	}//while(moreblock)
/*	if (write == true)
		Swrite.Write(EnryptString(temp) + "\r\n");
	else
		data += EnryptString(temp) + "\r\n";*/
	return 1;
}


void ClearPacket(void)
{
	head = tail = count = intFlg = 0;//errFlg=0;
	memset(SendBuffer,255,sizeof SendBuffer);
	memset(buffer,255,sizeof buffer);
}

int countDigits(long inteiro,int max)
{
  char *string;
  string = (char *) calloc(max+1,sizeof(char));
  sprintf(string,"%lu",inteiro);
  return strlen(string);
}

void DefaultDate(void)
{
	time_t t = time(NULL);
	struct tm d = *localtime(&t);


	DD2 = d.tm_mday;	//jena  .da_day;
	MM2 = d.tm_mon;	// .da_mon;
	YYYY2 = d.tm_year;	// .da_year;

	DD1 = DD2 - 4;
	if(DD1 < 1)
	{
		MM1 = MM2 - 1;
		if(MM1 == 0)
		{
			MM1 = 12;
			YYYY1 = YYYY2-1;
		}
		else
			YYYY1 = YYYY2;

		if(MM1 == 1 || MM1 == 3 || MM1 == 5 || MM1 == 7 || MM1 == 8 || MM1 == 10 || MM1 == 12)
			DD1 = 31 + DD1;
		else if(MM1 == 4 || MM1 == 6 || MM1 == 9 || MM1 == 11)
			DD1 = 30 + DD1;
		else
			if (isleapyear(YYYY2))
				DD1 = 29 + DD1;
			else
				DD1 = 28 + DD1;
	}
	else
	{
		MM1 = MM2;
		YYYY1 = YYYY2;
	}
}


void GetDates(void)
{
	while(1)
	{
		while(1)
		{
			//clrscr();
			printf("LS Start Date\nDD MM YYYY: ");
			scanf("%u %u %u", DD1, MM1, YYYY1);
			if(isdatevalid(MM1,DD1,YYYY1))//(DD1>0 && DD1<31) && (MM1>0 && MM1<12) && (YYYY1> 2005 && YYYY1< 2015))
				break;
			else
			{
				printf("Invalid date");
				//getch();
			}
		}
		while(1)
		{
			//clrscr();
			printf("LS Start Date:%02u %02u %u\n",DD1,MM1,YYYY1);
			printf("LS End Date\nDD MM YYYY: ");
			scanf("%u %u %u",&DD2,&MM2,&YYYY2);

			if(isdatevalid(MM2,DD2,YYYY2))//(DD2>0 && DD2<31) && (MM2>0 && MM2<12) && (YYYY2> 2005 && YYYY2< 2015))
				break;
			else
			{
				printf("Invalid date");
				//getch();
			}

		}

		//clrscr();
		printf("LS Start Date:%02d %02d %d\n",DD1,MM1,YYYY1);
		printf("LS End Date  :%02d %02d %d\n\n",DD2,MM2,YYYY2);

		if(YYYY2 > YYYY1)
			break;
		else if(YYYY2 ==YYYY1 && MM2 > MM1)
			break;
		else if(YYYY2 ==YYYY1 && MM2 == MM1 && DD2 >= DD1)
			break;
		else
		{
			printf("Load Survey End Date\nMust be greater then \nLoad Survey Start Date.\nPress any key..");
			//getch();
		}

/*		if(YYYY2>=YYYY1)
			if(MM2>=MM1)
				if(DD2>=DD1)
					break;
				else
				{
					printf("Load Survey End Date\nMust be greater then \nLoad Survey Start Date.\nPress any key..");
					getch();
				}
			else
			{
				printf("Load Survey End Date\nMust be greater then \nLoad Survey Start Date.\nPress any key..");
				getch();
			}
		else
		{
			printf("Load Survey End Date\nMust be greater then \nLoad Survey Start Date.\nPress any key..");
			getch();
		}*/
	}
}

int selData(void)
{
	//clrscr();
	int choice1 = 0, Instdataflag=0;
	int Result=0;
	char chLS;
	LoadSurveyFlag = 0;
	//unsigned int rep;
	//struct time* d;

	//get LLS Secret
	memset(llssecret, 0, sizeof(llssecret));
	memcpy(llssecret, "KEM_SEC1", 8);

	while(1)
	{
		rrr_c = 0; rrr_c1 = 0; rrr_s = 0; sss_c = 0; sss_c1 = 0; sss_s = 0;
		chLS = 'N';	//Y-selected, N-Not selected, load survey selection
		type = 3;

		if(chLS == 'Y')
			GetDates();
		else
			DefaultDate();
			
		Result = 0;

		fp = fopen("example.ddt", "w");
		if(type == 3)
			fprintf(fp,"\nTYPEC");
		else if(type == 2)
			fprintf(fp,"\nTYPEB");
		else if(type == 1)
			fprintf(fp,"\nTYPEA");

		Result = SNRM();
		if(Result == 1)
		{
			Result =0;
			Result = AARQ();
			if(Result == 1)
			{
				Result = 0;
				Result = ReadInstantData();
				if(Result == 0)
					continue;
				Result = 0;
				if(type == 2)
					Result = ReadBillingDataB();
				else
					Result = ReadBillingDataC();
				if(Result == 0)
					continue;

				Result = 0;
				Result = ReadEventData();
				if(Result == 0)
					continue;

				Result = 0;
				Result = ReadLoadSurvey(3);
				if(Result == 0)
					continue;
				fclose(fp);
				if(Result == 1)
				{
					DISC();
					CopytoMainFile();
				}
				ErrPrt(1,1,"\nMETER READ SUCCESSFULLY.  \nPRESS ANY KEY..");
				getchar();
			}
			else
			{
				ErrPrt(1,1,"\nASSOCIATION FAIL. \nPRESS ANY KEY..");
				getchar();
			}
		}
		else
		{
			ErrPrt(1,1,"\nCOMMUNICATION FAIL.\nCHECK CABLE. \nPRESS ANY KEY..");
			getchar();
		}

	}	

	return 0;
}


void CopytoMainFile(void)
{
	fp =  fopen ("\\example.ddt","r");
	fp1 = fopen ((const char*) FileName,"a");
	fseek(fp1, 0L, SEEK_END);
	printf("\nPLEASE WAIT..\n");
	while (1)
	{
		int ch = fgetc(fp);
		if(ch == -1)
			break;
		fputc(ch, fp1);
	}

	fclose(fp);
	fclose(fp1);
	remove("\\example.ddt");
}

//==================================================================================
uint8_t Write(uint8_t* strngW, uint16_t startByteW, uint16_t n)
{
	int i = 0;

	for(i = startByteW; i < startByteW + n; i++)
	{
		if(!SendByte(strngW[i]))
			return 0;
	}
	
	return 1;
}

int Read(unsigned char *RxDt)
{
	int i=0;
	unsigned long time1;
	time1=0;
	while(1)
	{
		while((rx_flag==0)&&(time1<905350))    //655350
			time1++;
		if(time1>=905350)
			return 0;
		else
			if(head != tail)
			{
				RxDt[i++] = ReadByte();
				time1=0;
				//printf("%02x ",RxDt[i-1]);
			}
			else
			  time1++;
			//printf("\t %d %d",head,tail);
			//RxDt[i++] = MtrFunction.ReadByte();
		//printf("%02x ",RxDt[i-1]);
		if(i>2 && i==RxDt[2]+2)
			break;
	}
	rx_flag=0;
	return 1;
}


