//=======================================================================================
#ifndef _KT_COMM_H_
#define _KT_COMM_H_
//=======================================================================================

typedef struct Port{
	int Uart;
	char lcb;
	int baud;
}Port;

void InitPort(unsigned char INTR, unsigned int PORT, unsigned int PIC, unsigned int BDRATE);
char ReadByte(void);//(int Wait = 200);
char SendByte(char ch);
void CheckBufferEmpty(void);
void DisablePort(void);

//=======================================================================================
#endif //_KT_COMM_H_
//=======================================================================================

