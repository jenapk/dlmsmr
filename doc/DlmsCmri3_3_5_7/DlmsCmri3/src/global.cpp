
#include "typedef.h"
#include <linux/rtc.h>
unsigned char INTR;
unsigned int PORT;
unsigned int PIC;
volatile _UINT n;	
_SINT type;
_SINT head,tail;
volatile unsigned int intFlg,rx_flag,FrameReceiveFlag,count;
unsigned char fg_HLSSecret[20],MeterChallenge[20];
unsigned char state[4][4]; 

unsigned char FileName[20], FrameRcvFlag, g_UserAssociation;
int PGbar;
int LoadSurveyFlag;
int DD1,MM1,YYYY1,DD2,MM2,YYYY2;
unsigned char obis_a,obis_b,obis_c,obis_d,obis_e,obis_f,attr;
unsigned char rrr_c, rrr_c1, rrr_s, sss_c, sss_c1, sss_s;
int ch;
int c;

unsigned char fg_SendBuffer[256],fg_RxBuffer[1025];
long Serialno, VolumeOfData;
unsigned char MD_Int_Flag,BillingDateFlag,Profile_cap_Flag,MD_Reser_Flag,RTC_Flag,Tod_Flag,Phy_add_Flag;

int done;
//Add by mundel
unsigned char g_DispBuffer[22];
struct rtc_time g_RTC;		//will be global
unsigned char g_displayPoint;
unsigned char fg_CtSignflag;
