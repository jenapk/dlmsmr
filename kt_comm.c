
#include "kt_comm.h"

Port ComPort;
const int size = 1024;
int head,tail,count;
//char buf[size+1];
char buf[1025];


void CheckBufferEmpty(void)
{
	// jena while(!(inp(ComPort.Uart + 5) & 0x40));
}


void InitPort(unsigned char INTR, unsigned int PORT, unsigned int PIC, unsigned int BDRATE) //(int baud)
{
	//jena - port intialization will be done here
}

char ReadByte(void)
{
	char ch;
	ch = buf[tail++];
	if(tail >= size) tail = 0;
	return ch;
}

char SendByte(char ch)
{
	while(1)
	{
	   // jena if((inp(ComPort.Uart + 5) & 0x20))
	   break;
	}

	//if (ch!='\0')
	//jena outp(ComPort.Uart,ch);
	return 1;
}


//void interrupt newisr(...)
//{
	/* jena
	rx_flag=1;
	intFlg=1;
	buf[head++] = inp(PORT); //(0x3f8);
	n++;
	if(head >= size) head = 0;
	count++;
	asm {
			cli
			mov al,20h
			out 20h,al
			sti
	}
	jena */
//}

void DisablePort(void)
{
	/* jena
	unsigned bite;
	bite = inp(0x21);
	bite = bite | 0x18;
	outp(0x21, bite);
	bite = inp(ComPort.Uart+3);
	bite = bite & 0x7f;
	outp(ComPort.Uart+3, bite);
	outp(ComPort.Uart+1,0);
	outp(0x20, 0x20);

	jena */
}

// int CPort::rsinCnt(void)
// {
  // int i;
  // i = head-tail;
  // return (i<0 ? -(i) : i);
// }


