
//=================================================================
#ifndef _RS232_H
#define _RS232_H
//=================================================================

#include <stdio.h>
#include <string.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "typedef.h"

#define MAX_PORT_COUNT	27		//total number of ports can be handled
#define MAX_PORT_WIDTH	13		//to hold the port name in array

_UINT RS232_PortOpen(_UINT a_PortNO, _ULONG a_Baudrate);
//_UINT RS232_PortPollRead (_UINT a_PortNo, _UBYTE *a_pBuffer, _UINT a_Size);
_UINT RS232_PortPollRead (_UINT a_PortNo);
_UINT RS232_PortSendByte(int a_PortNo, unsigned char a_DataByte);
_UINT RS232_PortSendString(_UINT a_PortNo, _UBYTE *a_pBuffer, _UINT a_Size);
void RS232_PortClose (_UINT a_PortNo);
_UINT RS232_PortGetCTSEnableStatus(_UINT a_PortNo);
void Port_RegCBReceive ( void (* a_pFunctionName) (_UBYTE a_Byte) );

//=================================================================
#endif	//_RS232_H
//=================================================================


