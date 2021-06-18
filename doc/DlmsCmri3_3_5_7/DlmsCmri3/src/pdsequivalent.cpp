#include <sys/select.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "typedef.h"
#include "Config.h"
#include "utility.h"
#include "display.h"
#include "global.h"
#include "hhuLcd.h"

#ifdef LINUX_ATMEL
	#include "commonDefs.h"

void ClearLine (_UBYTE a_LineNo)
{
	goto_xy (a_LineNo, 1);
	LcdPrintf("                     ");
}
//****************************************************************
#elif LINUX_FREESCALE
	#include <fcntl.h>
	#include <linux/input.h>
	#include "PCE_KBDSM2.h"
	#include "PCE_GLCD.h"

	#define HHU_INPUT_DEVICE "/dev/input/event0"
	typedef struct input_event struct_user_input;
//----------------------------
int goto_xy(int row,int col)
{
	PCE_GLCDGotoxy(row, col);
	return(0);
}

//----------------------------
int LcdClear(void)
{
	PCE_GLCDClrscr();
	return(0);
}

//----------------------------
void buzzer_on (void)
{
	return;
}

//----------------------------
void buzzer_off (void)
{
	return;
}

//----------------------------
_SINT LcdPrintf(const char *appbuf1)
{
	fprintf(stderr, appbuf1);
	PCE_GLCDPrint((char*) *appbuf1);
	return(0);
}
//----------------------------
_SINT initDisplay(void)
{
	PCE_GLCDInit();
	return(0);
}

//----------------------------
//_UBYTE kb_hit(void)
//{
//	return(0);
//}

void ClearLine (_UBYTE a_LineNo)
{
	_UBYTE l_Column = 0x00;

	PCE_GLCDGotoxy(a_LineNo, 1);

	for (l_Column = 1; l_Column < 21; l_Column++)
	{
		PCE_GLCDPrintXYCh (a_LineNo, l_Column, ' ');
	}
}

int LcdPrint_Image(const char *imgbuff,unsigned int Imgcnt)
{
	return(0);
}

_UBYTE _getkey(void)
{
	_UBYTE l_KeyCode = 0;
    _SBYTE l_ByteRead = 0;
    _SINT l_Handle = 0;

    struct_user_input l_sInputEvent;
    l_Handle = open(HHU_INPUT_DEVICE, O_RDONLY);

    if(l_Handle == -1)
    {
    	perror("input");
        return -1;
    }

    while(1)
    {
    	l_ByteRead = read(l_Handle, &l_sInputEvent, sizeof(struct_user_input));

    	if(l_ByteRead == -1)
        {
    		break;
        }

    	if((l_sInputEvent.type == 0x01) && (l_sInputEvent.value == 0x01))
        {
    		break;
        }
    }

    close(l_Handle);

    if(l_ByteRead == -1)
    {
    	printf("Keypad input read fail...\n");
        return -1;
    }

    l_KeyCode = l_sInputEvent.code;

    switch (l_KeyCode)
    {
    	case KEY_0:	l_KeyCode = 48; break;
    	case KEY_1: l_KeyCode = 49; break;
    	case KEY_2: l_KeyCode = 50; break;
    	case KEY_3: l_KeyCode = 51; break;
    	case KEY_4: l_KeyCode = 52; break;
    	case KEY_5: l_KeyCode = 53; break;
    	case KEY_6: l_KeyCode = 54; break;
    	case KEY_7: l_KeyCode = 55; break;
    	case KEY_8: l_KeyCode = 56; break;
    	case KEY_9: l_KeyCode = 57; break;
    }

    fprintf(stderr, "KeyVal @ getkey = %d\n", l_KeyCode);
    return l_KeyCode;
}

//****************************************************************
#else //PC
//----------------------------
int goto_xy(int row,int col)
{
	//not in PC
	return(0);
}

//----------------------------
int LcdClear(void)
{
	return(0);
}

//----------------------------
void buzzer_on (void)
{
	return;
}

//----------------------------
void buzzer_off (void)
{
	return;
}

//----------------------------
_SINT LcdPrintf(const char *appbuf1)
{
	fprintf(stderr, appbuf1);
	return(0);
}

//----------------------------
_UBYTE _getkey(void)
{
	_UBYTE l_Ch = 0;

	cin >> l_Ch;
	return(l_Ch);
}

//----------------------------
_SINT initDisplay(void)
{
	return(0);
}

//===========================================================================
int LcdPrint_Image(const char *imgbuff,unsigned int Imgcnt)
{
	_UBYTE l_Row, l_Col, l_Count;
	_UBYTE l_Value = 0x00;
	_UINT l_Pixel, l_Index;

	for (l_Row = 1; l_Row <= 64; l_Row++)
	{
		l_Pixel = 0x00;

		for (l_Col = 1; l_Col <= 16; l_Col++)
		{
			l_Index = l_Row - 1 + l_Col - 1;
			l_Value = imgbuff[l_Index];

			for (l_Count = 1; l_Count <= 8; l_Count++)
			{
				l_Pixel = ((l_Col -1) * 8) + l_Count;
				printf("%d, %u\n", l_Row, l_Pixel);

				l_Value <<= 1;	//1 bit shift left
			}

			printf("\n");
		}
	}

	return(0);
}

//================================================================

unsigned char kb_hit(void)
{

	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;

#if 0

	struct timeval tv;
	fd_set rdfs;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
	return(FD_ISSET(STDIN_FILENO, &rdfs));
#endif
}
//================================================================

#endif

//=================================================================
//this function will fill the buffer with HHU serial number and returns
//length of HHU serial number written to the buffer
_UBYTE APP_GetHHUSlNo (_UBYTE* a_Dest)
{
#ifdef LINUX_ATMEL
	fprintf(stderr,"test point 1234511\n");
	get_machineid( (_SBYTE*) a_Dest);
	fprintf(stderr, "test point 1234512\n");
#elif LINUX_FREESCALE
	get_machineid_FS( (_SBYTE*) a_Dest);
#else
	memcpy(a_Dest, "09899600810", 11);
#endif

	return(11);
}

//=========================================================================
//getting machine ID from freescale HHU
int get_machineid_FS(_SBYTE* a_pData)
{
	_UBYTE l_Char = 0;
	_UBYTE l_Buffer[50];
	_SBYTE * l_pSlNo;
	_UINT l_Count = 0;
	FILE* l_pDataFile = 0;

	l_pDataFile = fopen("/home/pinfo/serial.txt", "r");	//read only file

	if (!l_pDataFile)
	{
		//fclose(l_pDataFile);
		fprintf(stderr,"\n%s ", "SLNo File not found");
		return(0);
	}

	memset(l_Buffer, 0, sizeof(l_Buffer));

	while (!feof(l_pDataFile))
	{
		l_Char = fgetc(l_pDataFile);
		l_Buffer[l_Count++] = l_Char;
	}

	fclose(l_pDataFile);
	l_pSlNo = (_SBYTE *) strstr( (const _SBYTE*) l_Buffer, "SLNO");
	l_pSlNo += 6;	//skip 6 bytes

	for (l_Char = 0; l_Char < 8; l_Char++)
	{
		if( ((*l_pSlNo >= '0') && (*l_pSlNo <= '9')) ||
			((*l_pSlNo >= 'A') && (*l_pSlNo <= 'Z')) ||
			((*l_pSlNo >= 'a') && (*l_pSlNo <= 'z')) )
		{
			printf("%c", *l_pSlNo);
			*a_pData++ = *l_pSlNo;
		}

		l_pSlNo++;
	}

	printf("\n");

	return(0);
}
//=========================================================================
//This function will receive numeric data from user, Enter is must at end
//ESC will return 0
_UINT APP_GetUserNumericString (void)
{
	_UINT l_RetVal;

#ifdef LINUX_ATMEL
	_UBYTE l_Buffer[8];
	_UBYTE l_Pose, l_Key;

	l_RetVal = 0x00;
	memset(l_Buffer, 0, 8);
	l_Pose = 0x00;

	//Only numeric keys allowed

	for (;;)
	{
		l_Key = _getkey();

		if( (l_Key >= '0') && (l_Key <= '9') )
		{
			if (l_Pose <= 8)
			{
				l_Buffer[l_Pose++] = l_Key;	// - 0x30);
			}
		}
		else if(l_Key == BKSP)
		{
			if (l_Pose)
			{
				l_Pose--;
				l_Buffer[l_Pose] = 0x00;
			}
		}

		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) l_Buffer);

		if(l_Key == ESC)
		{
			memset(l_Buffer, 0, 8);
			break;
		}
		else if(l_Key == ENT)
		{
			break;
		}
	}

	l_RetVal = atoi( (_SBYTE*) l_Buffer);

#elif LINUX_FREESCALE
	_UBYTE l_Buffer[10];
	_UBYTE l_Pose, l_Key;

	l_RetVal = 0x00;
	memset(l_Buffer, 0, sizeof(l_Buffer));
	l_Pose = 0x00;

	//Only numeric keys allowed

	for (;;)
	{
		l_Key = _getkey();

		if( (l_Key >= '0') && (l_Key <= '9') )
		{
			if (l_Pose <= 8)
			{
				l_Buffer[l_Pose++] = l_Key;	// - 0x30);
			}
		}
		else if(l_Key == KEY_A)		//for backspace
		{
			if (l_Pose)
			{
				l_Pose--;
				l_Buffer[l_Pose] = 0x00;
			}
		}
		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) l_Buffer);

		if(l_Key == KEY_ESC)
		{
			memset(l_Buffer, 0, sizeof(l_Buffer));
			break;
		}
		else if(l_Key == KEY_ENTER)
		{
			break;
		}
	}

	l_RetVal = atoi( (_SBYTE*) l_Buffer);
#else
	l_RetVal = 0x00;
	cin >> l_RetVal;
#endif

	return(l_RetVal);
}

//---------------------------------------------------------------------------
//returns -1 for Next, -2 for previous, 0- nothing, all positive values
//as user input
_SINT APP_GetUserAlphaNumeric (void)
{
	_SINT l_RetVal = 0x00;

#ifdef LINUX_ATMEL
	_UBYTE l_Buffer[8];
	_UBYTE l_Pose, l_Key;

	l_RetVal = 0x00;
	memset(l_Buffer, 0, 8);
	l_Pose = 0x00;

	//Only numeric keys allowed
	for (;;)
	{
		l_Key = _getkey();

		if( (l_Key >= '0') && (l_Key <= '9') )	//Numeric Range
		{
			if (l_Pose <= 8)
			{
				l_Buffer[l_Pose] = l_Key;
				l_Pose++;
			}
		}
		else if(l_Key == BKSP)		//backspace for edit
		{
			if (l_Pose)
			{
				l_Pose--;
				l_Buffer[l_Pose] = 0x00;
			}
		}

		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) l_Buffer);

		if(l_Key == ESC)
		{
			memset(l_Buffer, 0, 8);
			break;
		}
		else if(l_Key == ENT)
		{
			break;
		}
		else if (l_Key == F2 )	//Prev. Page
		{
			l_RetVal = -2;
			break;
		}
		else if (l_Key == F3 )	//Next Page
		{
			l_RetVal = -3;
			break;
		}
	}

	if (l_RetVal >= 0)
	{
		l_RetVal = atoi( (_SBYTE*) l_Buffer);
	}
#elif LINUX_FREESCALE
	_UBYTE l_Buffer[10];
	_UBYTE l_Pose, l_Key;

	l_RetVal = 0x00;
	memset(l_Buffer, 0, sizeof(l_Buffer));
	l_Pose = 0x00;

	//Only numeric keys allowed
	for (;;)
	{
		l_Key = _getkey();

		if( (l_Key >= '0') && (l_Key <= '9') )	//Numeric Range
		{
			if (l_Pose <= 8)
			{
				l_Buffer[l_Pose] = l_Key;
				l_Pose++;
			}
		}
		else if(l_Key == KEY_A)		//backspace for edit
		{
			if (l_Pose)
			{
				l_Pose--;
				l_Buffer[l_Pose] = 0x00;
			}
		}

		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) l_Buffer);

		if(l_Key == KEY_ESC)
		{
			memset(l_Buffer, 0, sizeof(l_Buffer));
			break;
		}
		else if(l_Key == KEY_ENTER)
		{
			break;
		}
		else if (l_Key == PCE_KBDSM2_KEY_UPARROW )	//Prev. Page
		{
			l_RetVal = -2;
			break;
		}
		else if (l_Key == KEY_C )	//Next Page
		{
			l_RetVal = -3;
			break;
		}
	}

	if (l_RetVal >= 0)
	{
		l_RetVal = atoi( (_SBYTE*) l_Buffer);
	}
#else
	l_RetVal = 0x00;
	cin >> l_RetVal;
#endif

	fprintf(stderr, "Key Val @ alpha = %d", l_RetVal);
	return(l_RetVal);
}
//=============================================================
void HAL_DispImage (void)
{
	//Image Processed for Height =64, Width = 128
	LcdClear();
	//Print FW Version
	memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
	sprintf( (_SBYTE*) g_DispBuffer, "MSEDCL CMRI Ver%d.%d%d", APP_VER_MAJOR, APP_VER_MINOR, APP_VER_REV);
	HAL_DebugPrint(1, (_SBYTE*) g_DispBuffer);
	sleep(1);
	return;
}

void HAL_PleaseWait(_UBYTE a_Line)
{
	HAL_DebugPrint(a_Line, (_SBYTE*) "Please Wait ...");
	return;
}
//===============================================================
void APP_BuzzerSuccess (void)
{
#ifndef LINUX_PC
	_UINT l_Count;

	for(l_Count = 0x00; l_Count < 4 ; l_Count++)
	{
		system("/home/admin/bin/buzzer");
		system("/home/admin/bin/buzzer");
		system("/home/admin/bin/buzzer");
		UTL_Wait(200);
	}
#elif LINUX_FREESCALE
	for(l_Count = 0x00; l_Count < 4 ; l_Count++)
		{
			system("/home/admin/bin/buzzer");
			system("/home/admin/bin/buzzer");
			system("/home/admin/bin/buzzer");
		}
#endif

	return;
}
//=========================================================
void APP_BuzzerFailure (void)
{
#ifndef LINUX_PC
	_UINT l_Count;

	for(l_Count = 0x00; l_Count < 8; l_Count++)
	{
		system("/home/admin/bin/buzzer");
		UTL_Wait(100);
		system("/home/admin/bin/buzzer");
		UTL_Wait(100);

	}
#elif LINUX_FREESCALE
	for(l_Count = 0x00; l_Count < 8; l_Count++)
		{
			system("/home/admin/bin/buzzer");
			UTL_Wait(50);
			system("/home/admin/bin/buzzer");
		}
#endif

	return;
}
//============================================================

//=========================================================================
//This function will receive numeric data from user, Enter is must at end
//ESC will return 0
void APP_GetLLS_Key_String (_UBYTE* a_pBuffer)
{
	_UBYTE l_Buffer[25];

#ifdef LINUX_ATMEL
	_UBYTE l_Pose, l_Key;

	memset(l_Buffer, 0, sizeof(l_Buffer));
	l_Pose = 0x00;

	for (;;)
	{
		l_Key = _getkey();

		if( ((l_Key >= '0') && (l_Key <= '9')) ||\
			((l_Key >= 'a') && (l_Key <= 'z')) ||\
			((l_Key >= 'A') && (l_Key <= 'Z')) )
		{
			if (l_Pose <= 22)
			{
				l_Buffer[l_Pose++] = l_Key;	// - 0x30);
			}
		}
		else if(l_Key == BKSP)
		{
			if (l_Pose)
			{
				l_Pose--;
				l_Buffer[l_Pose] = 0x00;
			}
		}

		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) l_Buffer);

		if(l_Key == ESC)
		{
			memset(l_Buffer, 0, sizeof(l_Buffer));
			break;
		}
		else if(l_Key == ENT)
		{
			break;
		}
	}

#else
	cin >> l_Buffer;
#endif
	strcpy((_SBYTE*)a_pBuffer, (const char*)l_Buffer);
	return;
}
