
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include "kt_misc.h"
#include "kt_comm.h"
#include "functh.h"
#include "kt_global.h"

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

unsigned int pppfcs16(unsigned int fcs, unsigned char *cp, int length)
{
	//jena : need to relook the function, all 3 lines are commented
	//while(length--)
	//	fcs = (fcs >> 8) ^ fcstaintFlgb[(fcs ^ *cp++) & 0xff];
	//return fcs;
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
	SendPkt(9);

	//jena delay(500);
	g_intFlg = 0; //jena

	while((g_intFlg == 0) && (time2 < 68265))   //68265 //655350
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

	SendPkt(c+3);
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
	SendPkt(9);
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
		SendPkt(27);

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
		SendPkt(78);

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

		SendPkt(9);
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

		SendPkt(21);
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

			SendPkt(9);
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
	//struct dt d;
	struct tm *d;

	// jena getdate(&d);

	DD2 = d->tm_mday;	//jena  .da_day;
	MM2 = d->tm_mon;	// .da_mon;
	YYYY2 = d->tm_year;	// .da_year;

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

	/*clrscr();
	printf("LS Start Date:%02d %02d %d\n",DD1,MM1,YYYY1);
	printf("LS End Date  :%02d %02d %d\n\n",DD2,MM2,YYYY2);
	getch();*/
}


void GetDates(void)
{
	while(1)
	{
		while(1)
		{
			//clrscr();
			printf("LS Start Date\nDD MM YYYY: ");
			scanf("%u %u %u",&DD1,&MM1,&YYYY1);
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
	int choice = 0, choice1 = 0, Instdataflag=0;
	int Result=0;
	char chLS;
	LoadSurveyFlag = 0;
	//unsigned int rep;
	//struct time* d;

	while(1)
	{
		fflush(stdin);
		ErrPrt(1,1,"\n ENTER LLS SECRET : ");
		//jena gets(llssecret);

		if(strlen((const char*) llssecret) < 4)
		{
			ErrPrt(1,1,"\n LLS SECRET MUST HAVE MORE \n THEN 4 CHARACTERS\n PRESS ANY KEY..");
			//getch();
		}
		else if(strlen((const char*) llssecret) > 16)
		{
			ErrPrt(1,1,"\n LLS SECRET DOESN'T CONTAINS \n MORE THEN 16 CHARACTERS\n PRESS ANY KEY..");
			//getch();
		}

		else
			break;

	}

	while(1)
	{
		fflush(stdin);
		choice = selScr();
		if(choice == 0)
			break;
		rrr_c=0;rrr_c1=0;rrr_s=0;sss_c=0;sss_c1=0;sss_s=0;

		if(choice == 2)
		{
			while(1)
			{
				fflush(stdin);
				ErrPrt(1,1,"\n DO YOU WANT TO\n ENTER LOAD SURVEY \n DATE (Y/N)..");
				scanf("%c",&chLS);
				chLS = toupper(chLS);
				if(chLS == 'Y' || chLS == 'N')
					break;
				else
				{
					ErrPrt(1,1,"\nWRONG CHOICE  \nPRESS ANY KEY..");
					fflush(stdin);
					//getch();
				}


			}
			if(chLS == 'Y')
				GetDates();
			else
				DefaultDate();
				
			Result = 0;
			
			fp = fopen("\\example.ddt","w");
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
		else if(choice == 1)
		{
			Instdataflag = 0;
			while(1)
			{
				fflush(stdin);
				choice1 = selScrSelected();
				if(choice1 == 0 && Instdataflag == 0)
					break;
					
				if(choice1 == 0 && Instdataflag == 1)
				{
					fclose(fp);
					CopytoMainFile();
					break;
				}
					
				if(choice1 == 3)
				{
					while(1)
					{
						fflush(stdin);
						ErrPrt(1,1,"\n DO YOU WANT TO\n ENTER LOAD SURVEY \n DATE (Y/N)..");
						scanf("%c",&chLS);
						chLS = toupper(chLS);
						if(chLS == 'Y' || chLS == 'N')
							break;
						else
						{
							ErrPrt(1,1,"\nWRONG CHOICE  \nPRESS ANY KEY..");
							fflush(stdin);
							//getch();
						}


					}
					if(chLS == 'Y')
						GetDates();
					else
						DefaultDate();
				}
					
				Result = 0;
				rrr_c=0;rrr_c1=0;rrr_s=0;sss_c=0;sss_c1=0;sss_s=0;
				Result = SNRM();
				if(Result == 1)
				{
					Result =0;
					Result = AARQ();
					if(Result == 1)
					{	
						if(Instdataflag == 0)
						{
							Instdataflag = 1;
							fp = fopen("\\example.ddt","w");
							if(type == 3)
								fprintf(fp,"\nTYPEC");
							else if(type == 2)
								fprintf(fp,"\nTYPEB");
							else if(type == 1)
								fprintf(fp,"\nTYPEA");
							Result = 0;
							Result = ReadInstantData();
							if(Result == 0)
								break;
						}
						if(choice1 == 1)
						{
							Result = 0;
							if(type == 2)
								Result = ReadBillingDataB();
							else
								Result = ReadBillingDataC();
							if(Result == 0)
								break;
							else
							{
								DISC();
								//clrscr();
								printf("\nBILLING DATA READ\nSUCCESSFULLY.  \nPRESS ANY KEY..");
								getchar();
							}
						}
						else if(choice1 == 2)
						{
							Result = 0;
							Result = ReadEventData();
							if(Result == 0)
								break;
							else
							{
								DISC();
								//clrscr();
								printf("\nEVENT DATA READ\nSUCCESSFULLY.  \nPRESS ANY KEY..");
								getchar();
							}
						}
						else if(choice1 == 3)
						{
							Result = 0;
							Result = ReadLoadSurvey(3);
							if(Result == 0)
								break;
							else
							{
								DISC();
								//clrscr();
								printf("\nLOAD SURVEY DATA READ \nSUCCESSFULLY.  \nPRESS ANY KEY..");
								getchar();
							}
						}
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

void commPort(void)
{
	//MtrFunction.InitPort(INTR,PORT,PIC,BDRATE);       // initialize the port
	//MtrFunction.CheckBufferEmpty();
}

unsigned char Write(unsigned char *strngW,unsigned int startByteW,unsigned int n)
{
	int i=0;
	for(i=startByteW;i<startByteW+n;i++)
	{
		if(!SendByte(strngW[i]))
			return 0;
//		printf("%02x ",strngW[i]);

	}
	
/*	if(PGbar % 20 == 0)
	{
		ErrPrt(1,1,"\n DATA DOWNLOADING\n\n");
		PGbar = 0;	
	}
	printf("|");PGbar++;*/
	return 1;

}
/*
void meter :: Readack(char *RxDt,int startByteR,int n)
{
	int i;
	for(i=startByteR;i<=startByteR+n;i++)
	{
		RxDt[i] = MtrFunction.ReadByte();
	}

}
*/

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


/*
int meter :: Read(unsigned char *RxDt)
{
	int i=0;
	while(1)
	{
		RxDt[i++] = MtrFunction.ReadByte();
//		printf("%02x ",RxDt[i]);
		if(count>2 && i==RxDt[2]+2)
			break;
	}
	return 1;
}*/
/*
double meter::power(double x,int n)
{ if(n<0)
	{  x=1.0/x;
		n=-n;
	}
	if(n>0)
	{ return x*power(x,n-1);}
	else
	return 1.0;

}

double meter::getData(char *RxDt,int startByte,int n)
{
	double procData= 0.0;
	int j=startByte;
	for(int i=n-1;i>0;i--)
	{
		procData=procData+(double)RxDt[j]*power(256,i);
		j++;
	}


	return procData;
}
int meter:: dTime(int b2Rd)
{
	return (((b2Rd+4)*10/8)*1.60);//1.60
}

*/
/*
unsigned char meter :: writeToFile(unsigned char *WrDt,int startByte,int n, int rpt, int comma )
{
	mdf.open("C:\\test.dat",ios::out|ios::binary|ios::app);


	if(!mdf.fail())
	{
		for(int i=startByte;i<=(n+startByte)-1;i++)
			mdf<<WrDt[i];

		if (comma)
		{ mdf<<',';}
		if(snFlg)
		{   
			mdf<<"CMRI,";
		}
		mdf.close();

		return 1;
	}

	else
	{
		return 0;
	}
}*/
/*
unsigned char meter::writeToAthFile(unsigned char *WrDt,int startByte,int n)
{
	mdf.open("b:auth.dat",ios::out|ios::binary);
	for(int i=startByte;i<=(n+startByte)-1;i++)
	{
		mdf<<WrDt[i];
	}
	mdf.close();
	return 1;
}

unsigned int meter :: crc_16(unsigned char *bytes,int startByte,int n)
{
	unsigned int crc=0xffff;
	for(int i=startByte;i<=n;i++)
	{
		crc ^= bytes[i];
		for(int j=0;j<8;j++)
		{
			if((crc&0x0001)==1)
			{
				crc >>=1;
				crc ^= poly;
			}	else
			{
				crc >>=1;
			}
		}
	}
	return(crc);
}

unsigned int meter :: crc_16LS(unsigned char *bytes,int startByte,int n)
{
	for(int i=startByte;i<=n;i++)
	{
		crcLS ^= bytes[i];
		for(int j=0;j<8;j++)
		{
			if((crcLS&0x0001)==1)
			{
				crcLS >>=1;
				crcLS ^= poly;
			}	else
			{
				crcLS >>=1;
			}
		}
	}
	return(crcLS);
}

unsigned char meter :: proto(unsigned char alg)
{	unsigned int temp;
	hndSk[0]=0xfe;
	hndSk[1]=~(alg+250);
	hndSk[2]=rpt;
	temp=crc_16(hndSk,0,2);
	hndSk[3]=temp>>8;
	hndSk[4]=(unsigned char)temp;
	hndSk[5]=0xff;
	return 1;
}
int meter::RxDt1()
{


	long begin=0,end=0,size=0;
	unsigned int crcSys=0,crcMtr=0,cHk=0;
	MtrFunction.DisablePort();
	commPort();

	remove("b:test.dat");

	clrscr();
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl Meter Info";
	cHk=0;
	cHk = dRead(0);
	if((cHk==0)||(cHk==2))
	{
			clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
	}
	dis = pktInfo[8];
	snoTm();
	delay(100);
	crcSys= crc_16(RxDt,0,bytsToRd);
	crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);
	if(!pktChk)
	{
		if(crcSys!=crcMtr)
		{
				clrscr();
				gotoxy(1,2);
				printf(" CRC ERROR");
				ClrLn(3);
				gotoxy(1,3);
				printf(" Press any key.");
				getch();
				return 0;
		}
	}
	crcSys=crcMtr=0;
	writeToFile(pktInfo,3,4,9,1);
	writeToFile(RxDt,5,5,50,1);
	snFlg=1;
	writeToFile(pktInfo,8,2,50,1);   //8,1,50,1
	snFlg=0;
	writeToFile(pktInfo,7,4+nop,rpt,1); //3+nop
	writeToFile(RxDt,1,bytsToRd,50,1);

	//Inst. Data
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl Inst. Data";
	cHk=0;
	cHk = dRead(7);
	if((cHk==0)||(cHk==2))
	{
			 clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
	}
	//instData();
	delay(100);
	crcSys= crc_16(RxDt,0,bytsToRd);
	crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);
	if(!pktChk)
	{
		if(crcSys!=crcMtr)
		{
				clrscr();
				gotoxy(1,2);
				printf(" CRC ERROR");
				ClrLn(3);
				gotoxy(1,3);
				printf(" Press any key.");
				getch();
				return 0;
		}
	}
	crcSys=crcMtr=0;
	writeToFile(pktInfo,7,4+nop,rpt,1);//3+nop
	writeToFile(RxDt,1,bytsToRd,50,1);

	//hist
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl History";
	 cHk=0;
	cHk = dRead(22);
	if((cHk==0)||(cHk==2))
	{

			clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
	}
	delay(100);
	crcSys= crc_16(RxDt,0,bytsToRd);
	crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);
	if(!pktChk)
	{
		if(crcSys!=crcMtr)
		{
				clrscr();
				gotoxy(1,2);
				printf(" CRC ERROR");
				ClrLn(3);
				gotoxy(1,3);
				printf(" Press any key.");
				getch();
				return 0;
		}
	}
	crcSys=crcMtr=0;
	writeToFile(pktInfo,7,4+nop,rpt,1);//3+nop
	writeToFile(RxDt,1,bytsToRd,50,1);

	//tod
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl Tod";
	 cHk=0;
	cHk = dRead(30);
	if((cHk==0)||(cHk==2))
	{
			clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
	}
	delay(100);
	crcSys= crc_16(RxDt,0,bytsToRd);
	crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);
	if(!pktChk)
	{
		if(crcSys!=crcMtr)
		{
				clrscr();
				gotoxy(1,2);
				printf(" CRC ERROR");
				ClrLn(3);
				gotoxy(1,3);
				printf(" Press any key.");
				getch();
				return 0;
		}
	}
	crcSys=crcMtr=0;
	writeToFile(pktInfo,7,4+nop,rpt,1);//3+nop
	writeToFile(RxDt,1,bytsToRd,50,1);

	//Tamper

	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl Tamper";
	 cHk=0;
	cHk = dRead(14);
	if((cHk==0)||(cHk==2))
	{
			clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
	}
	delay(100);
	crcSys= crc_16(RxDt,0,bytsToRd);
	crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);

	if(!pktChk)
	{
		if(crcSys!=crcMtr)
		{
				clrscr();
				gotoxy(1,2);
				printf(" CRC ERROR");
				ClrLn(3);
				gotoxy(1,3);
				printf(" Press any key.");
				getch();
				return 0;
		}
	}
		crcSys= crc_16(RxDt,0,bytsToRd);
		crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);

	crcSys=crcMtr=0;
	writeToFile(pktInfo,7,4+nop,rpt,1);//3+nop
	writeToFile(RxDt,1,bytsToRd,50,1);

	//load survey
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl Load Survey";
	lsFcnt=0;
	cHk=0;
	cHk = dRead(38);
	if((cHk==0)||(cHk==2))
	{
			clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
	}
	ClrLn(15);
	delay(100);
	gotoxy(1,15);
 if(!pktChk)
  {
	if(index1>0)
	{
		crcLS=0xffff;
		crcSys= crc_16LS(RxDt,0,index1);
		crcSys= crc_16LS(pktInfo,0,index);
		crcMtr=pktInfo[index+1]*pow(256,1)+pktInfo[index+2]*pow(256,0);
		if(crcSys!=crcMtr)
		{
			clrscr();
			gotoxy(1,2);
			printf(" CRC ERROR");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
		}
		crcSys=crcMtr=0;
		writeToFile(RxDt,1,index1,50,0);
		writeToFile(pktInfo,0,index+1,50,1);
	}
	else
	{
		crcSys= crc_16(RxDt,0,bytsToRd);
		crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);
		if(crcSys!=crcMtr)
		{

			clrscr();
			gotoxy(1,2);
			printf(" CRC ERROR");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
		}
		crcSys=crcMtr=0;

		writeToFile(RxDt,1,bytsToRd,50,1);
	}
 }
	delay(1000);


	//send event Data
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl Event Log";
	cHk=0;
	cHk = dRead(52);
	if((cHk==0)||(cHk==2))
	{
			clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;
	}
	delay(100);
	crcSys= crc_16(RxDt,0,bytsToRd);
	crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);
	if(!pktChk)
	{
		if(crcSys!=crcMtr)
		{
				clrscr();
				gotoxy(1,2);
				printf(" CRC ERROR");
				ClrLn(3);
				gotoxy(1,3);
				printf(" Press any key.");
				getch();
				return 0;
		}
	}
	crcSys=crcMtr=0;
	writeToFile(pktInfo,7,4+nop,rpt,1);//3+nop
	writeToFile(RxDt,1,bytsToRd,50,1);

	//send power on off Data
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl P on/off";
	cHk=0;
	cHk = dRead(60);
	if((cHk==0)||(cHk==2))
	{
			clrscr();
			gotoxy(1,2);
			printf(" Comm. error!");
			ClrLn(3);
			gotoxy(1,3);
			printf(" Press any key.");
			getch();
			return 0;

	}
	delay(100);
	crcSys= crc_16(RxDt,0,bytsToRd);
	crcMtr=RxDt[bytsToRd+1]*pow(256,1)+RxDt[bytsToRd+2]*pow(256,0);
	if(!pktChk)
	{
		if(crcSys!=crcMtr)
		{
				clrscr();
				gotoxy(1,2);
				printf(" CRC ERROR");
				ClrLn(3);
				gotoxy(1,3);
				printf(" Press any key.");
				getch();
				return 0;
		}
	}
	crcSys=crcMtr=0;
	writeToFile(pktInfo,7,4+nop,rpt,1);//3+nop
	writeToFile(RxDt,1,bytsToRd,50,1);

	ClrLn(2);
	gotoxy(1,2);
	cout << " Plz Wait...";

	unsigned char a;
	mdf.open("b:test.dat",ios::in|ios::binary|ios::app);
	outfile.open("b:data.dat",ios::out|ios::binary|ios::app);



	begin = mdf.tellg();
	mdf.seekg (0, ios::end);
	end = mdf.tellg();
	mdf.seekg (0, ios::beg);
	size = end-begin;

	// if(size<2000)
	// {
	// clrscr();
	// cout << "File corrupt!"<<endl;
	// cout << "Press any key to cont.";
	// mdf.close();
	// outfile.close();
	// getch();
	// return 0;
	// }


	for(long h=0;h<=size-1;h++)
	{
		mdf.get(a);
		outfile << a;
	}

	mdf.close();
	outfile.close();
	ClrLn(2);
	gotoxy(1,2);
	cout << " Dwnl completed"<<endl;
	cout << " Press any key to cont.";
	getch();
	return 1;
}
*/

int SendPkt(int NoofBytes)
{
	Write(SendBuffer,0,NoofBytes);
	return 0;
}


/*
int meter::dRead(int startByte)
{
	int i, pkts,shft_flg=0;
	rpt=0;bytsToRd=0;nop=0;
	pktChk = 0;
	char ch;
	intFlg=0,errFlg=0;
	unsigned long time2=0;
	Write(buffer,startByte,6);
	while((intFlg==0)&&(time2<68265))   //68265 //655350
	{
		time2++;
	}
	if(time2>=68265)
	{
		errFlg=1;
		return 2;
	}

	// clrscr();
	// cout << time2;
	delay(500);
	head = tail=0;
	if(Read(pktInfo,0,100)==0)
	{
		return 0;
	}
	pkts=pktInfo[7];
	rpt=pktInfo[2];

	nop=pktInfo[10]*2; //pktInfo[9]*2
	if(startByte==38)
	{
		if(lsFcnt==0)
		{
			writeToFile(pktInfo,7,4+nop,rpt,1);    //3+nop
		}
		lsFcnt++;
		  if(pkts==0)
		  {
		  mdf.open("b:test.dat",ios::out|ios::binary|ios::app);
			if(!mdf.fail())
			  {
				 mdf<<",";
			}
			mdf.close();
		  }
	}
	////// total number of bytes  to read ////
	if(pkts==0)
	{
	  pktChk = 1;
	}
	if(pkts==1)
	{
		bytsToRd=(pktInfo[8]*pow(256,1)+pktInfo[9]*pow(256,0))+3; //bytsToRd=pktInfo[8]+3;

	}else
	{
		bytsToRd=pktInfo[8]*pow(256,1)+pktInfo[9]*pow(256,0);//bytsToRd=pktInfo[8];
	}

	index=0;
	index1=0;
	for (i=0;i<=pkts-1;i++)
	{
		intFlg=0,time2=0;
		if (i==0)
		{
			proto(pktInfo[1]);
			Write(hndSk,0,6);
		}
		else {Write(buffer,startByte+7,1);}
		while((intFlg==0)&&(time2<68265))    //655350////6553500
		{
			time2++;
		}
		if(time2>=68265)
		{
			errFlg=1;
			return 2;
		}
		// clrscr();
		// cout <<time2;

		delay(dTime(bytsToRd));
		if (i==0)
		{
			head = tail =0;
			if(Read(RxDt,index,bytsToRd+1)==0)
			{
				return 0;
			}
			index=index+bytsToRd;
		}else if (i==pkts-1)
		{
			index=index+1;
			head = tail =0;
			if(shft_flg)
			{
				index=index-1;
				shft_flg=0;
			}
			if(index1>0)
			{
				if(Read(pktInfo,index,bytsToRd+3)==0)
				{
					return 0;
				}

			}
			else
			{
				if(Read(RxDt,index,bytsToRd+3)==0)
				{
					return 0;
				}
			}
			if(index>28000)
			{

				index1=index;
				index=0;
				shft_flg=1;

			}
			index=index+bytsToRd-1;


		}else
		{
			index=index+1;
			head = tail =0;
			if(shft_flg)
			{
				index=index-1;
				shft_flg=0;
			}
			if(index1>0)
			{
				
				if(Read(pktInfo,index,bytsToRd)==0)
				{
					return 0;
				}
				
			}
			else
			{
				if(Read(RxDt,index,bytsToRd)==0)
				{
					return 0;
				}
			}
			index=index+bytsToRd-1;
			if(index>28000)
			{
				index1=bytsToRd*(i+1);
				index=0;
				shft_flg=1;
				
			}
		}
	}
	bytsToRd=(pktInfo[8]*pow(256,1)+pktInfo[9]*pow(256,0))*pkts;//pktInfo[8]*pkts;
	return 1;

}

void meter::snoTm()
{
	//serial no.//
	slno1=pktInfo[3]*pow(256,3)+pktInfo[4]*pow(256,2)+pktInfo[5]*pow(256,1)+pktInfo[6]*pow(256,0);
	//date//
	dat[0]=RxDt[5];
	dat[1]=RxDt[6];
	dat[2]=RxDt[7];
	//time//
	dat[3]=RxDt[8];
	dat[4]=RxDt[9];
}
void meter::instData()
{
	double ir,iy,ib;
	int vr,vy,vb;
	double sr,sy,sb,qr,qy,qb,pfr,pfy,pfb,pr,py,pb,freq;
	clrscr();
	if(dis==167)
	{
		vr = (RxDt[1]*pow(256,1)+RxDt[2]*pow(256,0))/100;       //getData(RxDt,1,2)/100;
		vy = (RxDt[3]*pow(256,1)+RxDt[4]*pow(256,0))/100;
		vb = (RxDt[5]*pow(256,1)+RxDt[6]*pow(256,0))/100;

		ir = (RxDt[7]*pow(256,1)+RxDt[8]*pow(256,0))/1000;
		iy = (RxDt[9]*pow(256,1)+RxDt[10]*pow(256,0))/1000;
		ib = (RxDt[11]*pow(256,1)+RxDt[12]*pow(256,0))/1000;

		pr =(RxDt[13]*pow(256,1)+RxDt[14]*pow(256,0))/1000;
		py = (RxDt[15]*pow(256,1)+RxDt[16]*pow(256,0))/1000;
		pb =(RxDt[17]*pow(256,1)+RxDt[18]*pow(256,0))/1000;
		//pt= (RxDt[19]*pow(256,1)+RxDt[20]*pow(256,0))/1000;

		sr =(RxDt[21]*pow(256,1)+RxDt[22]*pow(256,0))/1000;
		sy= (RxDt[23]*pow(256,1)+RxDt[24]*pow(256,0))/1000;
		sb= (RxDt[25]*pow(256,1)+RxDt[26]*pow(256,0))/1000;
		//st= (RxDt[27]*pow(256,1)+RxDt[28]*pow(256,0))/1000;

		qr= (RxDt[29]*pow(256,1)+RxDt[30]*pow(256,0))/1000;
		qy= (RxDt[31]*pow(256,1)+RxDt[32]*pow(256,0))/1000;
		qb= (RxDt[33]*pow(256,1)+RxDt[34]*pow(256,0))/1000;
		//qt= (RxDt[35]*pow(256,1)+RxDt[36]*pow(256,0))/1000;

		pfr= (RxDt[37]*pow(256,1)+RxDt[38]*pow(256,0))/1000;
		pfy= (RxDt[39]*pow(256,1)+RxDt[40]*pow(256,0))/1000;
		pfb= (RxDt[41]*pow(256,1)+RxDt[42]*pow(256,0))/1000;
		//pft= (RxDt[43]*pow(256,1)+RxDt[44]*pow(256,0))/1000;
		freq =(RxDt[45]*pow(256,1)+RxDt[46]*pow(256,0))/100;


		gotoxy(1,1);
		cout << "  MACS 2009 Version 2.05"<<endl;
		gotoxy(1,3);
		cout << "  Meter s/no:" << slno1<<endl;

		cout << "  RTC : "<< dat[0]<<"/"<<dat[1] <<"/" <<dat[2] <<" " << dat[3]<<":"<<dat[4]<<endl;
		//gotoxy(2,4);
		//cout<<"_____________________";

		gotoxy(4,5);
		cout  <<" R    Y    B";
		gotoxy(2,6);
		cout<<"---------------------";
		gotoxy(2,7);
		cout <<"P :";
		gotoxy(5,7);
		cout <<setprecision(2)<<pr;
		gotoxy(10,7);
		cout <<setprecision(2)<<py;
		gotoxy(15,7);
		cout <<setprecision(2)<<pb<<endl;
		//gotoxy(17,8);
		//cout <<setprecision(2)<<pt<<endl;
		
		
		cout <<" S :"<<endl;
		gotoxy(5,8);
		cout <<setprecision(2)<<sr;
		gotoxy(10,8);
		cout <<setprecision(2)<<sy;
		gotoxy(15,8);
		cout <<setprecision(2)<<sb<<endl;
		//gotoxy(17,9);
		//cout <<setprecision(2)<<st<<endl;
		
		
		cout <<" Q :";
		gotoxy(5,9);
		cout <<setprecision(2)<<qr;
		gotoxy(10,9);
		cout <<setprecision(2)<<qy;
		gotoxy(15,9);
		cout <<setprecision(2)<<qb<<endl;
		//gotoxy(17,10);
		//cout <<setprecision(2)<<qt<<endl;
		
		

		cout <<" V :";
		gotoxy(5,10);
		cout <<setprecision(2)<<vr;
		gotoxy(10,10);
		cout <<setprecision(2)<<vy;
		gotoxy(15,10);
		cout <<setprecision(2)<<vb<<endl;
		//gotoxy(17,11);
		//cout <<setprecision(4)<<pft<<endl;
		
		
		cout <<" I :";
		gotoxy(5,11);
		cout <<setprecision(2)<<ir;
		gotoxy(10,11);
		cout <<setprecision(2)<<iy;
		gotoxy(15,11);
		cout <<setprecision(2)<<ib << endl;

		
		
		cout <<" Pf:";
		gotoxy(5,12);
		cout <<setprecision(2)<<pfr;
		gotoxy(10,12);
		cout <<setprecision(2)<<pfy;
		gotoxy(15,12);
		cout <<setprecision(2)<<pfb <<endl;
		
		cout << " Fq:" <<freq<<endl;
		
		gotoxy(2,14);
		cout<<"---------------------";
	}
	
	else
	{
		vr = (RxDt[1]*pow(256,1)+RxDt[2]*pow(256,0))/100;       //getData(RxDt,1,2)/100;
		vy = (RxDt[3]*pow(256,1)+RxDt[4]*pow(256,0))/100;
		vb = (RxDt[5]*pow(256,1)+RxDt[6]*pow(256,0))/100;

		ir = (RxDt[7]*pow(256,3)+RxDt[8]*pow(256,2)+RxDt[9]*pow(256,1)+RxDt[10]*pow(256,0))/1000;
		iy = (RxDt[11]*pow(256,3)+RxDt[12]*pow(256,2)+RxDt[13]*pow(256,1)+RxDt[14]*pow(256,0))/1000;
		ib = (RxDt[15]*pow(256,3)+RxDt[16]*pow(256,2)+RxDt[17]*pow(256,1)+RxDt[18]*pow(256,0))/1000;

		pr =(RxDt[19]*pow(256,3)+RxDt[20]*pow(256,2)+RxDt[21]*pow(256,1)+RxDt[22]*pow(256,0))/1000;
		py =(RxDt[23]*pow(256,3)+RxDt[24]*pow(256,2)+RxDt[25]*pow(256,1)+RxDt[26]*pow(256,0))/1000;
		pb =(RxDt[27]*pow(256,3)+RxDt[28]*pow(256,2)+RxDt[29]*pow(256,1)+RxDt[30]*pow(256,0))/1000;
		//pt= (RxDt[19]*pow(256,1)+RxDt[20]*pow(256,0))/1000;

		sr =(RxDt[35]*pow(256,3)+RxDt[36]*pow(256,2)+RxDt[37]*pow(256,1)+RxDt[38]*pow(256,0))/1000;
		sy= (RxDt[39]*pow(256,3)+RxDt[40]*pow(256,2)+RxDt[41]*pow(256,1)+RxDt[42]*pow(256,0))/1000;
		sb= (RxDt[43]*pow(256,3)+RxDt[44]*pow(256,2)+RxDt[45]*pow(256,1)+RxDt[46]*pow(256,0))/1000;
		//st= (RxDt[27]*pow(256,1)+RxDt[28]*pow(256,0))/1000;

		qr= (RxDt[51]*pow(256,3)+RxDt[52]*pow(256,2)+RxDt[53]*pow(256,1)+RxDt[54]*pow(256,0))/1000;
		qy= (RxDt[55]*pow(256,3)+RxDt[56]*pow(256,2)+RxDt[57]*pow(256,1)+RxDt[58]*pow(256,0))/1000;
		qb= (RxDt[59]*pow(256,3)+RxDt[60]*pow(256,2)+RxDt[61]*pow(256,1)+RxDt[62]*pow(256,0))/1000;
		//qt= (RxDt[35]*pow(256,1)+RxDt[36]*pow(256,0))/1000;

		pfr= (RxDt[67]*pow(256,1)+RxDt[68]*pow(256,0))/1000;
		pfy= (RxDt[69]*pow(256,1)+RxDt[70]*pow(256,0))/1000;
		pfb= (RxDt[71]*pow(256,1)+RxDt[72]*pow(256,0))/1000;
		//pft= (RxDt[43]*pow(256,1)+RxDt[44]*pow(256,0))/1000;
		freq =(RxDt[75]*pow(256,1)+RxDt[76]*pow(256,0))/100;


		gotoxy(1,1);
		cout << "  MACS 2009 Version 2.03"<<endl;
		gotoxy(1,3);
		cout << "  Meter s/no:" << slno1<<endl;

		cout << "  RTC : "<< dat[0]<<"/"<<dat[1] <<"/" <<dat[2] <<" " << dat[3]<<":"<<dat[4]<<endl;
		//gotoxy(2,4);
		//cout<<"_____________________";

		gotoxy(4,5);
		cout  <<" R    Y    B";
		gotoxy(2,6);
		cout<<"---------------------";
		gotoxy(2,7);
		cout <<"P :";
		gotoxy(5,7);
		cout <<setprecision(2)<<pr;
		gotoxy(10,7);
		cout <<setprecision(2)<<py;
		gotoxy(15,7);
		cout <<setprecision(2)<<pb<<endl;
		//gotoxy(17,8);
		//cout <<setprecision(2)<<pt<<endl;
		
		
		cout <<" S :"<<endl;
		gotoxy(5,8);
		cout <<setprecision(2)<<sr;
		gotoxy(10,8);
		cout <<setprecision(2)<<sy;
		gotoxy(15,8);
		cout <<setprecision(2)<<sb<<endl;
		//gotoxy(17,9);
		//cout <<setprecision(2)<<st<<endl;
		
		
		cout <<" Q :";
		gotoxy(5,9);
		cout <<setprecision(2)<<qr;
		gotoxy(10,9);
		cout <<setprecision(2)<<qy;
		gotoxy(15,9);
		cout <<setprecision(2)<<qb<<endl;
		//gotoxy(17,10);
		//cout <<setprecision(2)<<qt<<endl;
		

		
		cout <<" V :";
		gotoxy(5,10);
		cout <<setprecision(2)<<vr;
		gotoxy(10,10);
		cout <<setprecision(2)<<vy;
		gotoxy(15,10);
		cout <<setprecision(2)<<vb<<endl;
		//gotoxy(17,11);
		//cout <<setprecision(4)<<pft<<endl;
		
		
		cout <<" I :";
		gotoxy(5,11);
		cout <<setprecision(2)<<ir;
		gotoxy(10,11);
		cout <<setprecision(2)<<iy;
		gotoxy(15,11);
		cout <<setprecision(2)<<ib << endl;
		
		

		cout <<" Pf:";
		gotoxy(5,12);
		cout <<setprecision(2)<<pfr;
		gotoxy(10,12);
		cout <<setprecision(2)<<pfy;
		gotoxy(15,12);
		cout <<setprecision(2)<<pfb <<endl;
		
		cout << " Fq:" <<freq<<endl;

		gotoxy(2,14);
		cout<<"---------------------";
	}


}

int meter::testMode()
{
	char ch,divider;
	unsigned int k;
	double kwh1,kvah1,kwh2,kvah2,kvarh_lag1,kvarh_lag2;
				 gotoxy(1,1);
				cout <<" Please Wait...";
				if((dRead(22)==0)||(dRead(22)==2))
				{
				clrscr();
				gotoxy(1,1);
				printf(" Comm. error!");
				ClrLn(2);
				gotoxy(1,2);
				printf(" Press any key.");
				getch();
				return 0;
				}

				divider=(unsigned int)(pktInfo[12]&0x0c)>>2;

				switch(divider)
				{
				case 0:
					k=1;
					break;
				case 1:
					k=10;
					break;
				case 2:
					k=100;
					break;
				case 3:
					k=1000;
					break;
				}

					kwh1=(RxDt[1]*pow(256,3)+RxDt[2]*pow(256,2)+RxDt[3]*pow(256,1)+RxDt[4]*pow(256,0))/k;
				kvah1=(RxDt[5]*pow(256,3)+RxDt[6]*pow(256,2)+RxDt[7]*pow(256,1)+RxDt[8]*pow(256,0))/k;
				kvarh_lag1=(RxDt[9]*pow(256,3)+RxDt[10]*pow(256,2)+RxDt[11]*pow(256,1)+RxDt[12]*pow(256,0))/k;

				clrscr();
				gotoxy(1,1);
				  cout <<setprecision(5)<<"kwh_1=" <<kwh1;
					gotoxy(1,2);
				  cout <<setprecision(5)<<"kvah_1=" <<kvah1;
				gotoxy(1,3);
				  cout <<setprecision(5)<<"kvarh_lg1=" <<kvarh_lag1;
				gotoxy(1,4);
				cout << " Press any key.";
				getch();
				clrscr();

				gotoxy(1,1);
				  cout <<" Please Wait...";
					if((dRead(22)==0)||(dRead(22)==2))
				{
				clrscr();
				gotoxy(1,1);
				printf(" Comm. error!");
				ClrLn(2);
				gotoxy(1,2);
				printf(" Press any key.");
				getch();
				return 0;
				}

				kwh2=(RxDt[1]*pow(256,3)+RxDt[2]*pow(256,2)+RxDt[3]*pow(256,1)+RxDt[4]*pow(256,0))/k;
				kvah2=(RxDt[5]*pow(256,3)+RxDt[6]*pow(256,2)+RxDt[7]*pow(256,1)+RxDt[8]*pow(256,0))/k;
				kvarh_lag2=(RxDt[9]*pow(256,3)+RxDt[10]*pow(256,2)+RxDt[11]*pow(256,1)+RxDt[12]*pow(256,0))/k;

				clrscr();
				gotoxy(1,1);
				  cout <<setprecision(5)<<"kwh_2=" <<kwh2;
					gotoxy(1,2);
				  cout <<setprecision(5)<<"kvah_2=" <<kvah2;
				gotoxy(1,3);
				  cout <<setprecision(5)<<"kvarh_lg2=" <<kvarh_lag2;
				gotoxy(1,4);
				cout << " Press any key.";
				getch();
				clrscr();
				gotoxy(1,1);
				cout << "    Difference ";
				gotoxy(1,2);
				  cout <<setprecision(5)<<"kwh=" <<kwh2-kwh1;
					gotoxy(1,3);
				  cout <<setprecision(5)<<"kvah=" <<kvah2-kvah1;
				gotoxy(1,4);
				  cout <<setprecision(5)<<"kvarh_lg=" <<kvarh_lag2-kvarh_lag1;
				getch();


	  return 0;
}

int meter::data2bcs()
{
	//ifstream mdf;
	long size;
	char b[5];
	char bcsAck[4];
	char *cHk = "ack";
	int value;
	BDRATE=56000;
	MtrFunction.DisablePort();
	commPort();
	unsigned char a;
	outfile.open("b:data.dat", ios::in|ios::binary);

	if (!outfile.fail())
	{
		clrscr();
		long begin,end;
		long n=0;
		char ch;
		for(int i=0;i<=3;i++)
		{
			b[i]=0;
			bcsAck[i]=0;
		}
		begin = outfile.tellg();
		outfile.seekg (0, ios::end);
		end = outfile.tellg();
		outfile.seekg (0, ios::beg);
		size = end-begin;

		b[3]=size;
		b[2]=size>>8;
		b[1]=size>>16;
		b[0]=size>>24;

		for(int k=0;k<=2;k++)
		{
			gotoxy(1,1);
			cout << "Reading Data";
			for(int i=0;i<=2;i++)
			{
				delay(400);
				gotoxy(13+i,1);
				cout << ".";
			}
			delay(200);
			if(k<2)
			clrscr();
		}


		delay(500);
		gotoxy(1,2);
		cout << "File size :"<<size <<" Bytes";
		delay(1000);
		ClrLn(7);
		gotoxy(1,1);
		cout << "Waiting...";
		gotoxy(1,3);
		cout << "Press Esc to Abort";
		Write(b,0,4);
		delay(10);
		head = tail = 0;
		buf[0]=0;
		buf[1]=0;
		buf[2]=0;

		do
		{
			Readack(bcsAck,0,3);
			bcsAck[3]=NULL;
			value = strcmp(bcsAck,cHk);
			if(kbhit())
			{
				ch=getch();
				if(ch==27)
				{
					outfile.close();
					return 0;
				}
			}

		}while(value !=0);
		ClrLn(9);
		ClrLn(7);
		gotoxy(1,1);
		cout << "Uploading... ";
		if(value==0)
		{
			do
			{
				outfile.get(a);
				outp(PORT,a);
				n++;
				delay(1);
				
				if(kbhit())
				{
					ch=getch();
					if(ch==27)
					{
						outfile.close();
						return 0; 		          
					}
				}				 
				
				
				
			}while(n!=size);
			
			outfile.close();
		}
		
		ClrLn(7);
		ClrLn(8);
		gotoxy(1,1);
		cout << "dwnl cmpld";
		gotoxy(1,2);
		cout << "Press any key to cont.";
		getch();
		
	}
	else
	{
		gotoxy(1,3);
		cout << "No data to transfer";
		getch();
	}

	return 1;

}
void meter::clrdb()
{

	char ccmd[15];
	int i=0;
	ErrPrt(2,1,"EnterPassword:");
	for (i=0;i<5;i++){ccmd[i]=getch();printf("*");}
	ccmd[i]=NULL;
	if(strcmp(ccmd,"clrdb")!=0)
	{
		ClrLn(14);
		ErrPrt(2,1,"Wrong password!");
	}
	else
	{
		ErrPrt(2,1,"Clearing database");
		strcpy(ccmd,"b:data.dat");
		if(remove(ccmd)==0)
		{ErrPrt(2,1,"Database cleared");}
		else
		{ErrPrt(2,1,"No record to clear!");}
	}
	ErrPrt(2,2,"Press key to continue");
	while(!getch());
	clrscr();
	
}

int meter::setRtc()
{
	struct date d1;
	struct time t1;

	unsigned char ccmd[10];
	getdate (&d1);
	gettime (&t1);
	int i;
	rpt=11;bytsToRd=0;nop=0;
	unsigned int temp;
	if(at[0]==1)
	{
		Write(buffer,46,6); 
		delay(500);
		clrscr();
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(15);
			gotoxy(1,15);
			printf(" Comm. error!");
			ClrLn(16);
			gotoxy(1,16);
			printf(" Press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100);  ///////6,7

		
		ccmd[0] = 0x0a;
		ccmd[1] = (unsigned char) t1.ti_min;//0x01; 
		ccmd[2] = (unsigned char) t1.ti_hour;				
		ccmd[3] = 0x001;
		ccmd[4] = (unsigned char) d1.da_day;
		ccmd[5] = (unsigned char) d1.da_mon;
		ccmd[6] = (unsigned char) d1.da_year-2000;

		
		temp=crc_16(ccmd,0,6);
		ccmd[7]=temp>>8;
		ccmd[8]=(unsigned char) temp;
		Write(ccmd,0,9);
		head=tail=0;
		delay(1500);
		Read(RxDt,0,1);
		delay(10);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Meter RTC reseted"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Meter RTC not reseted"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "validity Expired";
		getch();
	}

	return 1;
}


int meter::todTariff()
{
	clrscr();
	unsigned int n1,n2,i,slts,tar;
	char ch;
	rpt=12;bytsToRd=0;nop=0;
	unsigned int temp;
	if(at[22]==1)
	{
		for(i=0;i<=19;i++)
		{
			ccmd1[i] = 0x00;
		}
		
		ccmd1[0]=at[23];
		ccmd1[1]=at[24];
		ccmd1[2]=at[25];
		ccmd1[3]=at[26];
		ccmd1[4]=at[27];
		ccmd1[5]=at[28];
		ccmd1[6]=at[29];
		ccmd1[7]=at[30];
		ccmd1[8]=at[31];
		ccmd1[9]=at[32];
		ccmd1[10]=at[33];
		ccmd1[11]=at[34];
		ccmd1[12]=at[35];
		ccmd1[13]=at[36];
		ccmd1[14]=at[37];
		ccmd1[15]=at[38];
		ccmd1[16]=at[39];
		ccmd1[17]=at[40];
		
		
		
		

		
		// while(1)
		// {
		// gotoxy(1,1);
		// cout << "Enter no. of TOD slots : ";
		// cin  >> slts;
		// if(slts <2 ||slts >8)
		// {
		
		// cout << "Error!Enter value[2-8]";
		// ClrLn(1);
		// }
		// else
		// { 
		// ClrLn(1);
		// ClrLn(2);
		// ccmd1[0] = slts;
		// break;

		// }
		// }
		
		// while(1)
		// {
		// gotoxy(1,1);
		// cout << "Enter no. of Tariff : ";
		// cin  >> tar ;
		// if(tar > slts || tar <=0)
		// {
		
		// cout << "Error!Please re enter";
		// ClrLn(1);
		// }
		// else
		// { 
		// ClrLn(1);
		// ClrLn(2);
		// ccmd1[1] = tar;
		// break;
		
		// }
		// }

		// do
		// {   clrscr();
		// ErrPrt(5,2,"Set TOD Slots");
		// ErrPrt(2,4,"[Format:hh,mm]");
		// ErrPrt(2,5,"[hh:0-23,mm:0-15-30-45]");
		
		// i=1;	
		// do
		// {
		// ErrPrt(2,i+5,"Enter Time ");
		// printf("%d: ",i);
		// j=scanf("%d, %d",&n1,&n2);
		// ClrLn(15);
		// fflush(stdin);
		// if(n1>23)
		// {
		
		// ErrPrt(2,15,"Error!Please re enter");
		// ClrLn(i+5);
		
		// }

		// else
		// {
		// if((n2!=15)&&(n2!=30)&&(n2!=45)&&(n2!=0))
		// {
		// ErrPrt(2,15,"Error!Please re enter");
		// ClrLn(i+5);
		// }
		// else
		// {		
		// n2 /=15;
		// ccmd1[1+i] = (n1*4)+n2;
		// if(i>1)
		// {
		// if(ccmd1[i]<ccmd1[1+i])
		// {
		// i++;
		// }
		// else
		// {
		// ErrPrt(2,15,"Error!Please re enter");
		// ClrLn(i+5);
		// }
		// }
		// if(i==1)
		// {
		// i++;
		// }
		
		// }
		// }
		// }while(i<=slts);
		
		// ClrLn(15);
		// ErrPrt(2,15,"Accept Entries[Y/N]");

		// }while((toupper(getch()))!='Y');

		// do
		// {

		// i=1;
		// clrscr();
		// ErrPrt(6,2," TOD Tariffs");
		// do
		// {
		// cnt1=0;
		// ErrPrt(2,i+3,"Slot");
		// printf("%d Tariff[1-%d]:",i,ccmd1[0]);
		// j=scanf("%d",&n1);
		// fflush(stdin);
		// if(n1>ccmd1[0]||n1>ccmd1[1]||n1<1||j!=1)
		// {
		// ErrPrt(2,15,"Error!Please re enter");
		// ClrLn(i+3);
		// }
		// else
		// {
		// ccmd1[9+i] =n1; 
		// i++;
		// }
		// if(i==slts+1)
		// {
		// for(int d=1;d<=tar;d++)
		// {
		// for(int z=0;z<slts;z++)
		// {
		// if(ccmd1[10+z]==d)
		// {
		// cnt++;
		// }
		// }
		// if(cnt>0)
		// {
		// cnt1++;
		// }
		// cnt=0;
		// }
		// if(cnt1<tar)
		// {
		// i=1;
		// ClrLn(4);
		// ClrLn(5);
		// ClrLn(6);
		// ClrLn(7);
		// ClrLn(8);
		// ClrLn(9);
		// ClrLn(10);
		// ClrLn(11);
		// }	   
		// }
		// }while(i<=slts);
		// ClrLn(15);
		// ErrPrt(2,15,"Accept Entries[Y/N]");
		
		// }while((toupper(getch()))!='Y');
		
		
		
		// while(1)
		// {
		// ClrLn(15);
		// ClrLn(14);
		// ErrPrt(2,14,"EnterPassword:");
		// for (i=0;i<4;i++){ccmd[i]=getch();printf("*");}
		// ccmd[i]=NULL;
		// if(strcmp(ccmd,"data")!=0)
		// {
		// ClrLn(14);
		// ErrPrt(2,14,"Wrong password!");
		// ch=getch();

		// }
		// else
		// {
		// break;  	
		// }
		// if(ch==27)
		// {
		// return 1;
		// } 
		
		// }

		Write(buffer,68,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		


		memset(RxDt,0,sizeof RxDt); 
		temp=crc_16(ccmd1,0,17);
		ccmd1[18]=temp>>8;
		ccmd1[19]=(unsigned char) temp;
		Write(ccmd1,0,20);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " TOD timings changed!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Write TOD failed"<<endl;
			cout << " press any key to cont.";
		}
		getch();
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;


}

int meter::clrBuf()
{
	int i;
	for(i=0;i<=1024;i++)
	{
		buf[i]=0;
	}
	return 0;
}


int meter::clrCmri()
{
	intFlg=0;
	unsigned long time2=0;
	unsigned char ccmd2[15];
	unsigned int crcCMP=0,crcCMRI=0;
	
	if(head>4)
	{
		Read(pktInfo,0,5);
		pktInfo[5]=NULL;
		if(pktInfo[0]==0x63)
		{
			if(strcmp(pktInfo,"clrdb")!=0)
			{
				RxDt[0] = 0xCC;
				Write(RxDt,0,1);
				buf[0]=0;
				buf[1]=0;
				buf[2]=0;
				buf[3]=0;
				buf[4]=0;
				head=tail=n=0;
			}
			else
			{
				
				outfile.open("b:data.dat", ios::in|ios::binary);
				if (outfile.fail())
				{
					RxDt[0] = 0xEE;
					Write(RxDt,0,1);
					buf[0]=0;
					buf[1]=0;
					buf[2]=0;
					buf[3]=0;
					buf[4]=0;
					head=tail=n=0;
					return 0;
				}		
				strcpy(pktInfo,"b:data.dat");
				if(remove(pktInfo)==0)
				{
					RxDt[0] = 0xDD;
					Write(RxDt,0,1);
					buf[0]=0;
					buf[1]=0;
					buf[2]=0;
					buf[3]=0;
					buf[4]=0;
					head=tail=n=0;
				}
			}
			
			return 0;
		}
		else if(pktInfo[0]==0x61)
		{
			strcpy(ccmd2,"b:auth.dat");
			remove(ccmd2);
			
			if(strcmp(pktInfo,"authe")!=0)
			{
				RxDt[0] = 0xCC;
				Write(RxDt,0,1);
				buf[0]=0;
				buf[1]=0;
				buf[2]=0;
				buf[3]=0;
				buf[4]=0;
				buf[5]=0;
				buf[6]=0;
				head=tail=n=0;
				return 0;
			}
			else
			{
				buf[0]=0;
				buf[1]=0;
				buf[2]=0;
				buf[3]=0;
				buf[4]=0;
				buf[5]=0;
				buf[6]=0;
				RxDt[0] = 0xEE;
				Write(RxDt,0,1);
				//delay(1800);
				head=tail=n=0;
				intFlg=0;
				while((intFlg==0)&&(time2<68265))    //655350
				{
					time2++;
				}
				if(time2>=68265)
				{
					errFlg=1;
					return 2;
				}
				delay(500);
				if(Read(RxDt,0,46)==0)
				{
					buf[0]=0;
					buf[1]=0;
					buf[2]=0;
					buf[3]=0;
					buf[4]=0;
					buf[5]=0;
					buf[6]=0;
					head=tail=n=0;
					cout << "comm error";
					return 0;
				}
	            if(n>45)
				{
				 crcCMP=RxDt[44]*pow(256,1)+RxDt[45]*pow(256,0);
	             crcCMRI=crc_16(RxDt,0,43);
				 if(crcCMP==crcCMRI)
				 {
				  writeToAthFile(RxDt,0,44);
				 }
				 else
				 {
				   return 3;       
				 }
				}
				RxDt[0] = 0xDD;
				Write(RxDt,0,1);
				
				buf[0]=0;
				buf[1]=0;
				buf[2]=0;
				buf[3]=0;
				buf[4]=0;
				buf[5]=0;
				buf[6]=0;
				head=tail=n=0;
				return 1;
			}

		}
	}
	return 1;
}

int meter::readAthFile()
{
		unsigned long athCmri;
		 unsigned int ccmd[3];
		
	unsigned char a;
	struct date d1;
	struct time t1;
	unsigned long athFile;
	getdate (&d1);
	gettime (&t1);
	
	int begin=0,end=0,size=0;
	mdf.open("b:auth.dat",ios::in|ios::binary);
	if (!mdf.fail())
	{
		
		begin = mdf.tellg();
		mdf.seekg (0, ios::end);
		end = mdf.tellg();
		mdf.seekg (0, ios::beg);
		size = end-begin;

		
		for(int h=0;h<=size-1;h++)
		{
			mdf.get(a);
			at[h]=a;         
		}

		mdf.close();
		}
	 else
	 {
		 return 0;
	 }
	athCmri = (((long) d1.da_year)-2000)*10000+((int) d1.da_mon)*100+((int) d1.da_day);
	athFile = at[41]*pow(256,2)+at[42]*pow(256,1)+at[43]*pow(256,0);

	if(athCmri>athFile)
	{
		for(int k=0;k<=size-1;k++)
		{
			at[k]=0x00;         
		}
		
	}
	return 1;

}

int meter::mdRst()
{
	unsigned int temp;
	clrscr();
	rpt=15;
	if(at[1]==1)
	{
		Write(buffer,76,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=0xBB;
		ccmd1[1]=0xCC;
		temp=crc_16(ccmd1,0,1);
		ccmd1[2]=temp>>8;
		ccmd1[3]=(unsigned char) temp;
		Write(ccmd1,0,4);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " MD Reseted!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " MD Reset Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;
}

int meter::mdIntPrd()
{
	unsigned int temp;
	clrscr();
	rpt=19;
	if(at[4]==1)
	{
		Write(buffer,82,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[5];                                 //file reading
		temp=crc_16(ccmd1,0,0);
		ccmd1[1]=temp>>8;
		ccmd1[2]=(unsigned char) temp;
		Write(ccmd1,0,3);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;
}

int meter::mdSubIntPrd()
{
	unsigned int temp;
	clrscr();
	rpt=20;
	if(at[6]==1)
	{
		Write(buffer,88,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[7];                  // file reading
		temp=crc_16(ccmd1,0,0);
		ccmd1[1]=temp>>8;
		ccmd1[2]=(unsigned char) temp;
		Write(ccmd1,0,3);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;

}

int meter::ctRatio()
{
	unsigned int temp;
	clrscr();
	rpt=17;
	if(at[16]==1)
	{
		Write(buffer,94,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[17];                  // file reading
		ccmd1[1]=at[18];	
		temp=crc_16(ccmd1,0,1);
		ccmd1[2]=temp>>8;
		ccmd1[3]=(unsigned char) temp;
		Write(ccmd1,0,4);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;

}


int meter::ptRatio()
{
	unsigned int temp;
	clrscr();
	rpt=18;
	if(at[19]==1)
	{
		Write(buffer,100,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[20];                  // file reading
		ccmd1[1]=at[21];	
		temp=crc_16(ccmd1,0,1);
		ccmd1[2]=temp>>8;
		ccmd1[3]=(unsigned char) temp;
		Write(ccmd1,0,4);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;

}

int meter::mdType()
{
	unsigned int temp;
	clrscr();
	rpt=25;
	if(at[8]==1)
	{
		Write(buffer,106,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[9];                  // file reading	
		temp=crc_16(ccmd1,0,0);
		ccmd1[1]=temp>>8;
		ccmd1[2]=(unsigned char) temp;
		Write(ccmd1,0,3);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;

}

int meter::billDate()
{
	unsigned int temp;
	clrscr();
	rpt=21;
	if(at[2]==1)
	{
		Write(buffer,112,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[3];                  // file reading	
		temp=crc_16(ccmd1,0,0);
		ccmd1[1]=temp>>8;
		ccmd1[2]=(unsigned char) temp;
		Write(ccmd1,0,3);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}
	return 1;

}

int meter::ovrLoad()
{
	unsigned int temp;
	clrscr();
	rpt=22;
	if(at[10]==1)
	{
		Write(buffer,118,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}

		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[11];                  // file reading
		ccmd1[1]=at[12];	
		temp=crc_16(ccmd1,0,1);
		ccmd1[2]=temp>>8;
		ccmd1[3]=(unsigned char) temp;
		Write(ccmd1,0,4);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;

}



int meter::undrLoad()
{
	unsigned int temp;
	clrscr();
	rpt=23;
	if(at[13]==1)
	{
		Write(buffer,124,6); 
		delay(500);
		if(Read(RxDt,0,100)==0)
		{
			ClrLn(14);
			gotoxy(2,14);
			printf("Comm error!");
			printf("\n press any key to cont.");
			getch();
			return 0;
		}
		
		proto(RxDt[1]);

		memset(RxDt,0,sizeof RxDt);
		
		Write(hndSk,0,6);
		delay(100);
		head = tail = 0;
		Read(RxDt,0,100); 
		
		ccmd1[0]=at[14];                  // file reading
		ccmd1[1]=at[15];	
		temp=crc_16(ccmd1,0,1);
		ccmd1[2]=temp>>8;
		ccmd1[3]=(unsigned char) temp;
		Write(ccmd1,0,4);
		head = tail = 0;
		delay(1500);
		Read(RxDt,0,1);
		clrscr();
		gotoxy(1,1);
		if(RxDt[0]==221)
		{	
			cout << " Done!"<<endl;
			cout << " press any key to cont.";
		}
		else
		{
			cout << " Failed!"<<endl;
			cout << " press any key to cont.";
		}
		getch();
	}
	else
	{
		clrscr();
		cout << "Validity Expired";
		getch();
	}

	return 1;

}
*/
