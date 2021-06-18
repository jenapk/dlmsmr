//===============================================================
#ifndef _EXTERN_H
#define _EXTERN_H
//===============================================================
#include "typedef.h"

extern unsigned char INTR;
extern unsigned int PORT;
extern unsigned int PIC;
extern volatile unsigned int intFlg,rx_flag,FrameReceiveFlag,count;
extern volatile _UINT n;	
extern _SINT type;
extern _SINT head,tail;
extern  unsigned char fg_HLSSecret[20],MeterChallenge[20];
extern unsigned char state[4][4]; 

extern unsigned char FileName[20], FrameRcvFlag, g_UserAssociation;
extern int PGbar;
extern int LoadSurveyFlag;
extern int DD1,MM1,YYYY1,DD2,MM2,YYYY2;
extern unsigned char obis_a,obis_b,obis_c,obis_d,obis_e,obis_f,attr;
extern unsigned char rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s;
extern int ch;
//extern int c;
extern unsigned char fg_SendBuffer[256],fg_RxBuffer[1025];
extern long Serialno, VolumeOfData;
extern unsigned char MD_Int_Flag,BillingDateFlag,Profile_cap_Flag,MD_Reser_Flag,RTC_Flag,Tod_Flag,Phy_add_Flag;

//extern int done;
extern unsigned char g_DispBuffer[22];
extern struct rtc_time g_RTC;		//will be global
extern unsigned char g_displayPoint;
extern unsigned char fg_CtSignflag;

typedef struct
{
	_UBYTE Day;
	_UBYTE Month;
	_UBYTE Hour;
	_UBYTE Minute;
	_UBYTE Second;
	_UINT Year;
}RtcTypeDef;

//===============================================================
#endif	//_EXTERN_H
//===============================================================
