#include <stdio.h>
#include "comm.h"
#include "global.h"
//#include "TOOLS.H"



const int size = 1024;
//int head,tail,count;
unsigned char buf[size+1];
#if 0
Port ComPort;

void interrupt (*oldisr)(...);
void interrupt newisr(...);

void CheckBufferEmpty(void)
{
  while(!(inp(ComPort.Uart + 5) & 0x40));
}


void InitPort(unsigned char INTR, unsigned int PORT, unsigned int PIC, unsigned int BDRATE) //(int baud)
{
	char ch;
	unsigned intmask,OldSetting;
	unsigned char msb,lsb,lcb;
	unsigned divisor;
	unsigned bite;
	int irq;
	ComPort.Uart =PORT; //0x3f8;
	ComPort.lcb = 0x03;    /////////////03

	   disable();
	   head = tail = count = 0;
	   irq =PIC; //4;
	   divisor = 115200l/BDRATE;;
	   msb = divisor >> 8;
	   lsb = (divisor << 8) >> 8;
	   outportb(ComPort.Uart+3,128);
	   outportb(ComPort.Uart,lsb);
	   outportb(ComPort.Uart + 1,msb);
	   outportb(ComPort.Uart + 3,ComPort.lcb);
	   outportb(ComPort.Uart + 1,0);
	   outp(ComPort.Uart+2,0x07);	
	   outp(ComPort.Uart+4, 0x0b);
	   outp(ComPort.Uart+6, 0x80);
	   oldisr = getvect(INTR);
	   setvect(INTR,newisr);
	   intmask = 1<<(irq%8);
	   asm cli
	   OldSetting = inp(0x21);
	   outp(0x21,OldSetting & ~intmask);
	   asm sti
	   outp(ComPort.Uart + 1,0);
	   inp(ComPort.Uart + 5);
	   inp(ComPort.Uart + 6);
	   inp(ComPort.Uart);
	   outp(ComPort.Uart+1, 0x01);
	   outp(0x20, 0x20);
	   enable();

}

char SendByte(char ch)
{
	while(1)
	{
	   if((inp(ComPort.Uart + 5) & 0x20))
	   break;
	}
	  outp(ComPort.Uart,ch);
	return TRUE;
}



void DisablePort()
{
	unsigned bite;
	bite = inp(0x21);
	bite = bite | 0x18;
	outp(0x21, bite);
	bite = inp(ComPort.Uart+3);
	bite = bite & 0x7f;
	outp(ComPort.Uart+3, bite);
	outp(ComPort.Uart+1,0);
	outp(0x20, 0x20);
}



//====================	timer interrupt	===================

const int TIMER_INTR = 0x1c;

void interrupt (*prev_timer)(...);
void interrupt new_timer(...);

unsigned long milisec, elapsed = 0;
volatile int  timer_done;

void activate(void)
{
	prev_timer = getvect(TIMER_INTR);
	setvect(TIMER_INTR, new_timer);
}

void deactivate(void)
{
	setvect(TIMER_INTR, prev_timer);
}


void interrupt new_timer(...)
{
	outportb(0x3B2,0x08); //backlight on
	prev_timer();
}

//=============================================================
#endif

unsigned char ReadByte()
{
	unsigned char ch;
	ch = buf[tail++];
	//printf("%02x ",buf[tail-1]);
	if(tail >= size) tail = 0;
	return ch;
}


void HPL_ReceiveByte(_UBYTE a_Byte)
{
	rx_flag=1;
	intFlg=1;
	buf[head++] = a_Byte; //(0x3f8);
	n++;
	if(head >= size) head = 0;
	//count++;
}

