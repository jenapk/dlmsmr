
#include "kt_global.h"
#include "kt_comm.h"
#include "kt_libUART.h"


Port ComPort;
const int size = 1024;
int head,tail,count;
char buf[1025];

char ReadByte(void)
{
	char ch;
	ch = buf[tail++];
	if(tail >= size) tail = 0;
	return ch;
}

char SendByte(char ch)
{
	kt_libUART_SendByte(g_portNo, ch);
	return 1;
}

