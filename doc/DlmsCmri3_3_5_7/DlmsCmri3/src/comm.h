
//==========================================================================================================================
#ifndef _COMM_H
#define _COMM_H
//==========================================================================================================================
#include "typedef.h"

typedef struct _port{
	int Uart;
	char lcb;
	int baud;
}Port;

void InitPort(unsigned char INTR, unsigned int PORT, unsigned int PIC, unsigned int BDRATE);
unsigned char ReadByte();//(int Wait = 200);
char SendByte(char ch);
void CheckBufferEmpty(void);
void DisablePort();
void HPL_ReceiveByte(_UBYTE a_Byte);

//==========================================================================================================================
#endif  //_COMM_H
//==========================================================================================================================







