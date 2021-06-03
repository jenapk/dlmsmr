/*
 * functh.h
 *
 *  Created on: 03-Jun-2021
 *      Author: pkjena
 */
//=============================================================================================================
#ifndef FUNCTH_H_
#define FUNCTH_H_
//=============================================================================================================
#include "kt_typedef.h"


typedef enum
{
	APP_STATE_INIT,
	APP_STATE_IDLE,
	APP_STATE_NEXT,
	APP_STATE_ANALYZE
}en_appState;


static void dlmsReceiveISR(uint8_t a_Byte);
void kt_appInit(void);
void kt_appProcess(void);

//------------------------------------------------------------
int SNRM(void);
int AARQ(void);
int DISC(void);
uint16_t pppfcs16(uint16_t fcs, uint8_t *cp, int length);
int fcs(uint8_t *cp, int length, int flag);
int GetRequest(uint8_t classID, uint8_t Obis_a, uint8_t Obis_b, uint8_t Obis_c, uint8_t Obis_d, uint8_t Obis_e,uint8_t Obis_f,uint8_t Attr);
int ReadInstantData(void);
int ReadBillingDataC(void);
int ReadBillingDataB(void);
int ReadEventData(void);
int ReadLoadSurvey(int DateFlag);
void ClearPacket(void);
void CommError(void);
int countDigits(long inteiro,int max);
uint8_t Encrypt(uint8_t k);
void CopytoMainFile(void);
void GetDates(void);
void DefaultDate(void);
void ErrPrt(int,int,char *);
int Read(uint8_t *RxDt);
int SendPkt(int NoofBytes);
void commPort(void);
uint8_t Write(uint8_t* strngW, uint16_t startByteW, uint16_t n);
int isdatevalid(int month, int day, int year);
int dRead(int startByte);
int selData(void);
int selScr(void);
int selScrSelected(void);
/*	void testScr();  */

//=============================================================================================================
#endif /* FUNCTH_H_ */
//=============================================================================================================

