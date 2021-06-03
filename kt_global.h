//================================================================================
//#ifndef KT_GLOBAL_H_
//#define KT_GLOBAL_H_
//================================================================================
#include "kt_typedef.h"


extern uint8_t g_portNo;


extern unsigned char INTR;
extern unsigned int PORT;
extern unsigned int PIC;
extern unsigned int BDRATE;

extern unsigned int rx_flag,pktChk;
extern unsigned char ccmd[55];
extern unsigned char ccmd1[40];
extern unsigned char RxDt[150];
extern unsigned char hndSk[6];
extern unsigned char pktInfo[150];
extern unsigned char at[55];
//extern unsigned int bytsToRd, nop, rpt, index, index1, dis, crcLS;
extern unsigned int n, intFlg;
extern int errFlg,lsFcnt,snFlg;
extern int type;

//================================================================================
//#endif //KT_GLOBAL_H_
//================================================================================

