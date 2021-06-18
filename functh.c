
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
#include "kt_analyze.h"

#define PPPINITFCS16 0xffff /*initial FCS value*/
#define PPPGOODFCS16 0xf0b8 /*Good final FCS value*/
//#define P 0x8408
#define isleapyear(year) ((!(year % 4) && (year % 100)) || (!(year % 400)))

FILE *fp, *fp1;
unsigned char LoadSurveyFlag;
unsigned char FileName[20];
unsigned int g_intFlg;
unsigned long head, tail;
unsigned int YYYY1, YYYY2;
unsigned char MM1, MM2, DD1, DD2;
unsigned int count;
uint8_t fg_appFnIndex;
uint16_t fg_rxIndex;	//to receive rx bytes
//jena end

unsigned char g_txBuffer[] ={0x7e,0xA0,0x07,0x03,0x41,0x93,0x5A,0x64,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xFF,0xAA,0xBB,0xFE,0x03,0x03,0x03,0x03,0xFF,0xAA,0xBB,0xFE,0x05,0x05,0x05,0x05,0xFF,0xAA,0xBB, 0xFE,0x0B,0x0B,0x0B,0x0B,0xFF,0xFE,0x06,0x06,0x06,0x06,0xFF,0xAA,0xBB,0xFE,0x07,0x07,0x07,0x07,0xFF,0xAA,0xBB,0xFE,0x0c,0x0c,0x0c,0x0c,0xff,0xAA,0xBB,0xFE,0x0F,0x0F,0x0F,0x0F,0xFF,0xFE,0x13,0x13,0x13,0x13,0xFF,0xFE,0x14,0x14,0x14,0x14,0xFF,0xFE,0x11,0x11,0x11,0x11,0xFF,0xFE,0x12,0x12,0x12,0x12,0xFF,0xFE,0x19,0x19,0x19,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF};
unsigned char g_rxBuffer[] ={0x7e,0xA0,0x07,0x03,0x41,0x93,0x5A,0x64,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xFF,0xAA,0xBB,0xFE,0x03,0x03,0x03,0x03,0xFF,0xAA,0xBB,0xFE,0x05,0x05,0x05,0x05,0xFF,0xAA,0xBB, 0xFE,0x0B,0x0B,0x0B,0x0B,0xFF,0xFE,0x06,0x06,0x06,0x06,0xFF,0xAA,0xBB,0xFE,0x07,0x07,0x07,0x07,0xFF,0xAA,0xBB,0xFE,0x0c,0x0c,0x0c,0x0c,0xff,0xAA,0xBB,0xFE,0x0F,0x0F,0x0F,0x0F,0xFF,0xFE,0x13,0x13,0x13,0x13,0xFF,0xFE,0x14,0x14,0x14,0x14,0xFF,0xFE,0x11,0x11,0x11,0x11,0xFF,0xFE,0x12,0x12,0x12,0x12,0xFF,0xFE,0x19,0x19,0x19,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF,0x19,0xFF,0xFE,0x15,0x15,0x15,0x15,0xFF,0xFE,0x16,0x16,0x16,0x16,0xFF,0xFE,0x17,0x17,0x17,0x17,0xFF};
unsigned char llssecret[16];
unsigned char rrr_c=0, rrr_c1=0, rrr_s=0, sss_c=0, sss_c1=0, sss_s=0;

//int fcs(unsigned char *cp, int length, int flag);
//jena void FrameType(unsigned char *cp, unsigned char* rrr_c, unsigned char* rrr_c1, unsigned char* rrr_s, unsigned char* ss_c, unsigned char* sss_c1, unsigned char* sss_s);

en_appState fg_appState;
int (*fpApp[10])(void) = {SNRM, AARQ, 0, 0, 0, 0, 0, 0, 0, 0};

//=================================================================================================
void kt_appStateMachine(void)
{

	switch(fg_appState)
	{
		//---------------------------------
		case APP_STATE_INIT:
			kt_appInit();
			fg_appState = APP_STATE_NEXT;
			break;
		//---------------------------------
		case APP_STATE_IDLE:
			break;
		//---------------------------------
		case APP_STATE_NEXT:
			fpApp[fg_appFnIndex]();
			fg_appState = APP_STATE_IDLE;
			break;
		//---------------------------------
		case APP_STATE_ANALYZE:
			kt_appAnalyze();
			break;
	}
}



//=================================================================================================
void kt_appProcess(void)
{
	while(1)
	{
		kt_appStateMachine();
	}
}

//=================================================================================================
//executes next state
void kt_appSetStateNext(void)
{
	fg_appState = APP_STATE_NEXT;
}

//=================================================================================================
void kt_appSetStateIdle(void)
{
	fg_appState = APP_STATE_IDLE;
}

//=================================================================================================
void ErrPrt(int x, int y, char *str1)
{
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

	Serialno = (long)((long) g_rxBuffer[16] * 65536 * 256 + (long)g_rxBuffer[17]*65536 + (long)g_rxBuffer[18]*256 + (long)g_rxBuffer[19]);

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

	FileName[6] = g_rxBuffer[17];
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
	while(length--)
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

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

//=========================================================================================
//snrm packet request
int SNRM(void)
{
	ClearPacket();
	g_txBuffer[0] = 0x7e;
	g_txBuffer[1] = 0xa0;
	g_txBuffer[2] = 0x07;
	g_txBuffer[3] = 0x03;
	g_txBuffer[4] = 0x61;
	g_txBuffer[5] = 0x93;
	g_txBuffer[6] = 0x00;
	g_txBuffer[7] = 0x00;
	g_txBuffer[8] = 0x7e;
	fcs(g_txBuffer + 1, 5, 1);
	kt_libUART_SendData(g_portNo, g_txBuffer, 9);
	g_intFlg = 0;
	//Read(g_rxBuffer);
/*	for(int i=0;i<g_rxBuffer[2]+2;i++)
		printf("%02x ",g_rxBuffer[i]);
	printf("\n");
*/
//	if(g_rxBuffer[5] == 0x73)
//		return 1;
//	else
//		return 0;
}

int AARQ(void)
{
	unsigned char c=0;
	unsigned long time2 = 0;
	ClearPacket();
	printf("|");
	g_txBuffer[c++] = 0x7E;
	g_txBuffer[c++] = 0xA0;
	g_txBuffer[c++] = 0x00;
	g_txBuffer[c++] = 0x03;
	g_txBuffer[c++] = 0x41;
	g_txBuffer[c++] = 0x10;
	g_txBuffer[c++] = 0x00;	//FCS
	g_txBuffer[c++] = 0x00;	//FCS
	g_txBuffer[c++] = 0xE6;	//LLC Bytes
	g_txBuffer[c++] = 0xE6;
	g_txBuffer[c++] = 0x00;
	g_txBuffer[c++] = 0x60; //AARQ
	g_txBuffer[c++] = 0x11;	// Assign Below
	g_txBuffer[c++] = 0xA1; //Application Context Name Tag
	g_txBuffer[c++] = 0x09;
	g_txBuffer[c++] = 0x06;
	g_txBuffer[c++] = 0x07;
	g_txBuffer[c++] = 0x60;
	g_txBuffer[c++] = 0x85;
	g_txBuffer[c++] = 0x74;
	g_txBuffer[c++] = 0x05;
	g_txBuffer[c++] = 0x08;
	g_txBuffer[c++] = 0x01;
	g_txBuffer[c++] = 0x01;

	g_txBuffer[c++] = 0x8A;
	g_txBuffer[c++] = 0x02;
	g_txBuffer[c++] = 0x07;
	g_txBuffer[c++] = 0x80;
	g_txBuffer[c++] = 0x8B;
	g_txBuffer[c++] = 0x07;
	g_txBuffer[c++] = 0x60;
	g_txBuffer[c++] = 0x85;
	g_txBuffer[c++] = 0x74;
	g_txBuffer[c++] = 0x05;
	g_txBuffer[c++] = 0x08;
	g_txBuffer[c++] = 0x02;
	g_txBuffer[c++] = 0x01;
	g_txBuffer[c++] = 0xAC;
	g_txBuffer[c++] = strlen((const char*) llssecret) + 2;
	g_txBuffer[c++] = 0x80;
	g_txBuffer[c++] = strlen((const char*) llssecret);

	for (int x = 0; x < strlen((const char*) llssecret); x++)
		g_txBuffer[c++] = llssecret[x];

	g_txBuffer[c++] = 0xBE;
	g_txBuffer[c++] = 0x10;
	g_txBuffer[c++] = 0x04;
	g_txBuffer[c++] = 0x0E;
	g_txBuffer[c++] = 0x01;
	g_txBuffer[c++] = 0x00;
	g_txBuffer[c++] = 0x00;
	g_txBuffer[c++] = 0x00;
	g_txBuffer[c++] = 0x06;
	g_txBuffer[c++] = 0x5F;
	g_txBuffer[c++] = 0x1F;
	g_txBuffer[c++] = 0x04;
	g_txBuffer[c++] = 0x00;
	g_txBuffer[c++] = 0x00;
	g_txBuffer[c++] = 0x18;
	g_txBuffer[c++] = 0x1D;
	g_txBuffer[c++] = 0xFF;
	g_txBuffer[c++] = 0xFF;

	g_txBuffer[2] = c+1;
	g_txBuffer[12] = c-13;
	g_txBuffer[c+2] = 0x7E;

	fcs(g_txBuffer+1, 5, 1);
	fcs(g_txBuffer+1,c-1, 1);

	//SendPkt(c+3);
	kt_libUART_SendData(g_portNo, g_txBuffer, c+3);
	while((intFlg == 0) && (time2 < 655350))   //68265 //655350
		time2++;
	if(time2>=655350)
	{
		errFlg=1;
		return 0;
	}
	Read(g_rxBuffer);
/*	for(int i=0;i<g_rxBuffer[2]+2;i++)
		printf("%02x ",g_rxBuffer[i]);
*/	if(g_rxBuffer[28] == 0x00)
	{
		//jena FrameType(g_rxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);
		return 1;
	}
	else
		return 0;
}

int DISC(void)
{
	unsigned long time2 = 0;
	ClearPacket();
	g_txBuffer[0] = 0x7e;
	g_txBuffer[1] = 0xa0;
	g_txBuffer[2] = 0x07;
	g_txBuffer[3] = 0x03;
	g_txBuffer[4] = 0x41;
	g_txBuffer[5] = 0x53;
	g_txBuffer[6] = 0x00;
	g_txBuffer[7] = 0x00;
	g_txBuffer[8] = 0x7e;
	fcs(g_txBuffer+1, 5, 1);
	//SendPkt(9);
	kt_libUART_SendData(g_portNo, g_txBuffer, 9);

	delay(500);

	while((intFlg == 0) && (time2 < 68265))   //68265 //655350
		time2++;

	if(time2>=68265)
	{
		errFlg=1;
		return 0;
	}

	Read(g_rxBuffer);

//	for(int i=0;i<g_rxBuffer[2]+2;i++)
//		printf("%02x ",g_rxBuffer[i]);
	//printf("\nDISC");
	if(g_rxBuffer[5] == 0x73)
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
	
	g_txBuffer[0] = 0x7E;
	g_txBuffer[1] = 0xA0;
	g_txBuffer[3] = 0x03;
	g_txBuffer[4] = 0x41;
	ret_lsh = rrr_s << 5;
	g_txBuffer[5] = ret_lsh | 0x10;
	ret_lsh = sss_s << 1;
	g_txBuffer[5] = ret_lsh | g_txBuffer[5];
	g_txBuffer[6] = 0x00;
	g_txBuffer[7] = 0x00;
	g_txBuffer[8] = 0xE6;
	g_txBuffer[9] = 0xE6;
	g_txBuffer[10] = 0x00;
	g_txBuffer[11] = 0xC0;
	g_txBuffer[12] = 0x01;
	g_txBuffer[13] = 0x81;
	g_txBuffer[14] = 0x00;
	g_txBuffer[15] = classID;
	g_txBuffer[16] = Obis_a;
	g_txBuffer[17] = Obis_b;
	g_txBuffer[18] = Obis_c;
	g_txBuffer[19] = Obis_d;
	g_txBuffer[20] = Obis_e;
	g_txBuffer[21] = Obis_f;
	g_txBuffer[22] = Attr;
	
	if(LoadSurveyFlag == 0)
	{
		g_txBuffer[23] = 0x00;
		g_txBuffer[2] = 0x19;
		g_txBuffer[26] = 0x7e;
		fcs(g_txBuffer+1, 5, 1);
		fcs(g_txBuffer+1, 23, 1);
		//SendPkt(27);
		kt_libUART_SendData(g_portNo, g_txBuffer, 27);
	}
	else
	{
		LoadSurveyFlag = 0;
		g_txBuffer[23] = 0x01;
		g_txBuffer[24] = 0x01;
		g_txBuffer[25] = 0x02;
		g_txBuffer[26] = 0x04;
		g_txBuffer[27] = 0x02;
		g_txBuffer[28] = 0x04;
		g_txBuffer[29] = 0x12;
		g_txBuffer[30] = 0x00;
		g_txBuffer[31] = 0x08;
		g_txBuffer[32] = 0x09;
		g_txBuffer[33] = 0x06;
		g_txBuffer[34] = 0x00;
		g_txBuffer[35] = 0x00;
		g_txBuffer[36] = 0x01;
		g_txBuffer[37] = 0x00;
		g_txBuffer[38] = 0x00;
		g_txBuffer[39] = 0xFF;
		g_txBuffer[40] = 0x0F;
		g_txBuffer[41] = 0x02;
		g_txBuffer[42] = 0x12;
		g_txBuffer[43] = 0x00;
		g_txBuffer[44] = 0x00;

		g_txBuffer[45] = 0x09;
		g_txBuffer[46] = 0x0C;
		g_txBuffer[47] = YYYY1/256;
		g_txBuffer[48] = (unsigned char) YYYY1%256;
		g_txBuffer[49] = (unsigned char) MM1;
		g_txBuffer[50] = (unsigned char) DD1;
		g_txBuffer[51] = 0xFF;
		g_txBuffer[52] = 0x00;
		g_txBuffer[53] = 0x00;
		g_txBuffer[54] = 0x00;
		g_txBuffer[55] = 0x00;
		g_txBuffer[56] = 0xFF;
		g_txBuffer[57] = 0xFF;
		g_txBuffer[58] = 0xFF;

		g_txBuffer[59] = 0x09;
		g_txBuffer[60] = 0x0C;
		g_txBuffer[61] = (YYYY2/256);
		g_txBuffer[62] = (unsigned char) (YYYY2%256);
		g_txBuffer[63] = (unsigned char) MM2;
		g_txBuffer[64] = (unsigned char) DD2;
		g_txBuffer[65] = 0xFF;
		g_txBuffer[66] = 0x00;
		g_txBuffer[67] = 0x00;
		g_txBuffer[68] = 0x00;
		g_txBuffer[69] = 0x00;
		g_txBuffer[70] = 0xFF;
		g_txBuffer[71] = 0xFF;
		g_txBuffer[72] = 0xFF;

		g_txBuffer[73] = 0x01;
		g_txBuffer[74] = 0x00;
		g_txBuffer[75] = 0x00;
		g_txBuffer[76] = 0x00;
		g_txBuffer[77] = 0x7E;

		g_txBuffer[2] = 0x4C;
		fcs(g_txBuffer+1, 5, 1);
		fcs(g_txBuffer+1, 74, 1);
		//SendPkt(78);
		kt_libUART_SendData(g_portNo, g_txBuffer, 78);
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

	Read(g_rxBuffer);

	fprintf(fp,"\n");
	for(counter=16;counter<=21;counter++)
		fprintf(fp,"%02X",g_txBuffer[counter]);
	fprintf(fp," %02X ",g_txBuffer[22]);

	//jena FrameType(g_rxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

	if (/*bAckRec == true &&*/ g_rxBuffer[11] == 0xC4 && g_rxBuffer[12] == 0x02)
	{
		datablockno = (unsigned long) (g_rxBuffer[15]*0 +((unsigned long)g_rxBuffer[16])*65536 +((unsigned long)g_rxBuffer[17])*256 + g_rxBuffer[18]);
		moreblock = ! g_rxBuffer[14];
	}

	if (/*bAckRec == true && */g_rxBuffer[1] == 0xA0)
	{
		for(counter=15;counter<g_rxBuffer[2]-1;counter++)
			fprintf(fp,"%02X",g_rxBuffer[counter]);
		return 1;
	}
/*	else if (bAckRec == false)
		return false;*/
	if (moreblock == 1)
	{
		for(counter=23;counter<g_rxBuffer[2]-1;counter++)
			fprintf(fp,"%02X",g_rxBuffer[counter]);
	}
	else
	{
		for(counter=15;counter<g_rxBuffer[2]-1;counter++)
			fprintf(fp,"%02X",g_rxBuffer[counter]);
	}

	while (g_rxBuffer[1] == 0xA8)
	{
		ClearPacket();

		g_txBuffer[0] = 0x7E;
		g_txBuffer[1] = 0xA0;
		g_txBuffer[2] = 0x07;
		g_txBuffer[3] = 0x03;
		g_txBuffer[4] = 0x41;
		ret_lsh = rrr_s << 5;
		g_txBuffer[5] = ret_lsh | 0x10;
		g_txBuffer[5] = g_txBuffer[5] | 0x01;
		fcs(g_txBuffer+1, 5, 1);
		g_txBuffer[8] = 0x7E;

		//SendPkt(9);
		kt_libUART_SendData(g_portNo, g_txBuffer, 9);

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
		Read(g_rxBuffer);

		//jena FrameType(g_rxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

		if (/*bAckRec == true && */g_rxBuffer[1] == 0xA0)
		{
			for(counter=8;counter<g_rxBuffer[2]-1;counter++)
				fprintf(fp,"%02X",g_rxBuffer[counter]);
			break;
		}
		/*else if (bAckRec == false)
			return false;*/
		if (g_rxBuffer[1] == 0xA8)
		{
			for(counter=8;counter<g_rxBuffer[2]-1;counter++)
				fprintf(fp,"%02X",g_rxBuffer[counter]);
		}
	}//While(0xA8)
	while (moreblock)
	{
		moreblock = 0;
		ClearPacket();
	
		g_txBuffer[0] = 0x7E;
		g_txBuffer[1] = 0xA0;
		g_txBuffer[2] = 0x13;
		g_txBuffer[3] = 0x03;
		g_txBuffer[4] = 0x41;
		ret_lsh = rrr_s << 5;
		g_txBuffer[5] = ret_lsh | 0x10;
		ret_lsh = sss_s << 1;
		g_txBuffer[5] = ret_lsh | g_txBuffer[5];
		g_txBuffer[8] = 0xE6;
		g_txBuffer[9] = 0xE6;
		g_txBuffer[10] = 0x00;
		g_txBuffer[11] = 0xC0;
		g_txBuffer[12] = 0x02;
		g_txBuffer[13] = 0x81;
		g_txBuffer[14] = 0x00;
		g_txBuffer[15] = 0x00;
		g_txBuffer[16] = datablockno / 256;
		g_txBuffer[17] = datablockno % 256;
		fcs(g_txBuffer+1, 5, 1);
		fcs(g_txBuffer+1, 17, 1);
		g_txBuffer[20] = 0x7E;

		//SendPkt(21);
		kt_libUART_SendData(g_portNo, g_txBuffer, 21);

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
		Read(g_rxBuffer);

		//jena FrameType(g_rxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

		if (/*bAckRec == true &&*/ g_rxBuffer[11] == 0xC4 && g_rxBuffer[12] == 0x02)
		{
			datablockno = (unsigned long) (g_rxBuffer[15]*0 +((unsigned long)g_rxBuffer[16])*65536 +((unsigned long)g_rxBuffer[17])*256 + g_rxBuffer[18]);
			moreblock = ! g_rxBuffer[14];
		}

		if (/*bAckRec == true &&*/ g_rxBuffer[1] == 0xA0)
		{
			if (moreblock == 1)
//				for (i = 23; i < g_rxBuffer[2] - 1; i++)
//					temp += g_rxBuffer[i].ToString("X2");
				for(counter=23;counter<g_rxBuffer[2]-1;counter++)
					fprintf(fp,"%02X",g_rxBuffer[counter]);
			else
//				for (i = 21; i < g_rxBuffer[2] - 1; i++)
//					temp += g_rxBuffer[i].ToString("X2");
				for(counter=21;counter<g_rxBuffer[2]-1;counter++)
					fprintf(fp,"%02X",g_rxBuffer[counter]);
		}
		if (/*bAckRec == true && */g_rxBuffer[1] == 0xA8)
		{
			if (moreblock == 1)
//				for (i = 23; i < g_rxBuffer[2] - 1; i++)
//					temp += g_rxBuffer[i].ToString("X2");
				for(counter=23;counter<g_rxBuffer[2]-1;counter++)
					fprintf(fp,"%02X",g_rxBuffer[counter]);
			else
			{
				if(g_rxBuffer[20] == 0x81)
//					for (i = 22; i < g_rxBuffer[2] - 1; i++)
//						temp += g_rxBuffer[i].ToString("X2");
					for(counter=22;counter<g_rxBuffer[2]-1;counter++)
						fprintf(fp,"%02X",g_rxBuffer[counter]);
				else
//					for (i = 23; i < g_rxBuffer[2] - 1; i++)
//						temp += g_rxBuffer[i].ToString("X2");
					for(counter=23;counter<g_rxBuffer[2]-1;counter++)
						fprintf(fp,"%02X",g_rxBuffer[counter]);
			}

			//break;
		}
/*		else if (bAckRec == false)
			return false;
*/

		while (g_rxBuffer[1] == 0xA8)
		{
			ClearPacket();

			g_txBuffer[0] = 0x7E;
			g_txBuffer[1] = 0xA0;
			g_txBuffer[2] = 0x07;
			g_txBuffer[3] = 0x03;
			g_txBuffer[4] = 0x41;
			ret_lsh = rrr_s << 5;
			g_txBuffer[5] = ret_lsh | 0x10;
			g_txBuffer[5] = g_txBuffer[5] | 0x01;
			fcs(g_txBuffer+1, 5, 1);
			g_txBuffer[8] = 0x7E;

			//SendPkt(9);
			kt_libUART_SendData(g_portNo, g_txBuffer, 9);
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
			Read(g_rxBuffer);

			//jena FrameType(g_rxBuffer, rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s);

			if (1==1/*bAckRec == true*/)
			{
//				for (i = 8; i < g_rxBuffer[2] - 1; i++)
//					temp += g_rxBuffer[i].ToString("X2");
				for(counter=8;counter<g_rxBuffer[2]-1;counter++)
					fprintf(fp,"%02X",g_rxBuffer[counter]);
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
	memset(g_txBuffer, 255, sizeof g_txBuffer);
	memset(g_rxBuffer, 255, sizeof g_rxBuffer);
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

void kt_appInit(void)
{
	//get LLS Secret
	memset(llssecret, 0, sizeof(llssecret));
	memcpy(llssecret, "KEM_SEC1", 8);
	fg_appFnIndex = 0;	//point to 1st function SNRM

	//registering receiving routine
	kt_libUART_RegCBReceive (g_portNo, dlmsReceiveISR);
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

	//registering receiving routine
	kt_libUART_RegCBReceive (g_portNo, dlmsReceiveISR);

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


//=================================================================================================
//routine called while a single byte received from UART (pollig mode)
static void dlmsReceiveISR(uint8_t a_Byte)
{
	printf("%02X ", a_Byte);
	g_rxBuffer[fg_rxIndex++] = a_Byte;

	if(g_rxBuffer[0] != 0x7E) fg_rxIndex = 0;

	if(fg_rxIndex > 3)
	{
		if(fg_rxIndex >= g_rxBuffer[2] + 2)
		{
			//printf("\nComplete frame received.\n");
			fg_appState = APP_STATE_ANALYZE;
		}
	}
}


