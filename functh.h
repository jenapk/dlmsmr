/*
 * functh.h
 *
 *  Created on: 03-Jun-2021
 *      Author: pkjena
 */

#ifndef FUNCTH_H_
#define FUNCTH_H_

int SNRM(void);
int AARQ(void);
int DISC(void);
unsigned int pppfcs16(unsigned int fcs, unsigned char *cp, int length);
int fcs(unsigned char *cp, int length, int flag);
int GetRequest(unsigned char classID, unsigned char Obis_a, unsigned char Obis_b, unsigned char Obis_c, unsigned char Obis_d, unsigned char Obis_e,unsigned char Obis_f,unsigned char Attr);
int ReadInstantData(void);
int ReadBillingDataC(void);
int ReadBillingDataB(void);
int ReadEventData(void);
int ReadLoadSurvey(int DateFlag);
void ClearPacket(void);
void CommError(void);
int countDigits(long inteiro,int max);
unsigned char Encrypt(unsigned char k);
void CopytoMainFile(void);
void GetDates(void);
void DefaultDate(void);
/*	int dat[6];
long slno1;
void scr();
void snoTm();
int testMode();
void instData();
void ClrLn(int);*/
void ErrPrt(int,int,char *);
//	void Readack(char *RxDt,int startByteR,int n);
int Read(unsigned char *RxDt);
/*	double power(double x,int n);
double getData(char *RxDt,int startByte,int n );
unsigned char writeToFile(unsigned char *WrDt,int startByte,int n,int rpt, int comma );
unsigned char proto(unsigned char alg);
unsigned int crc_16(unsigned char *bytes,int startByte,int n);
unsigned int crc_16LS(unsigned char *bytes,int startByte,int n);
int dTime(int b2Rd);*/
int SendPkt(int NoofBytes);
void commPort(void);
unsigned char Write(unsigned char *strngW,unsigned int startByteW,unsigned int n);
int isdatevalid(int month, int day, int year);
//	int RxDt1();
int dRead(int startByte);
/*	int data2bcs();
void clrdb();
int setRtc();
int todTariff();
int clrBuf();
int clrCmri();
int authenticate();
unsigned char writeToAthFile(unsigned char *WrDt,int startByte,int n);
int readAthFile();
void prgmScr();
int setPara();
int mdRst();
int mdIntPrd();
int mdSubIntPrd();
int ctRatio();
int ptRatio();
int mdType();
int billDate();
int ovrLoad();
int undrLoad();*/
int selData(void);
int selScr(void);
int selScrSelected(void);
/*	void testScr();  */

#endif /* FUNCTH_H_ */
