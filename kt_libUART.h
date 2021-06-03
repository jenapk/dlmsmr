
//=================================================================
#ifndef _kt_libUART_H_
#define _kt_libUART_H_
//=================================================================

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>


uint16_t kt_libUART_Open(uint16_t a_PortNo, uint32_t a_Baudrate);
uint16_t kt_libUART_Poll (uint16_t a_PortNo);
uint16_t kt_libUART_SendByte(int a_PortNo, unsigned char a_DataByte);
uint16_t kt_libUART_SendData(uint16_t a_PortNo, uint8_t *a_pBuffer, uint16_t a_Size);
void     kt_libUART_Close(uint16_t a_PortNo);
uint16_t kt_libUART_GetCTSEnableStatus(uint16_t a_PortNo);
void     kt_libUART_RegCBReceive (uint16_t a_PortNo, void (* a_pFunctionName) (uint8_t a_Data));
uint8_t	 kt_libUART_GetPortNo(char* a_pPort, uint8_t* a_pPortName);

//=================================================================
#endif	//_kt_libUART_H_
//=================================================================


