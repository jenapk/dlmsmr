/*
 * kt_global.c
 *
 *  Created on: 03-Jun-2021
 *      Author: pkjena
 */

unsigned char INTR;
unsigned int PORT;
unsigned int PIC;
unsigned int BDRATE;

unsigned int rx_flag,pktChk;
unsigned char ccmd[55];
unsigned char ccmd1[40];
unsigned char RxDt[150];
unsigned char hndSk[6];
unsigned char pktInfo[150];
unsigned char at[55];
unsigned int bytsToRd, nop, rpt, index, index1, dis, crcLS;
unsigned int n, intFlg;
int errFlg,lsFcnt,snFlg;
int type;
