//display.cpp
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include "typedef.h"
#include "display.h"
#include "utility.h"
#include "pdsequivalent.h"
#include "Config.h"
#include "hhuLcd.h"

using namespace std;

//static variables
static _UINT fg_UserChoice;

//=========================================================================
void HAL_DispMenu (void)
{
	LcdClear();
	fg_UserChoice = 0x00;
	HAL_PleaseWait(4);

#ifdef LINUX_ATMEL
	system("/home/admin/bin/reset_low");
	UTL_Wait(100);//sleep(1);
	system("/home/admin/bin/power_off");
#endif

	//main menu
	HAL_DebugPrint(1, (_SBYTE*) "1.Data Download");
	HAL_DebugPrint(2, (_SBYTE*) "2.PC Interface");
	HAL_DebugPrint(3, (_SBYTE*) "3.Commissioning");
	HAL_DebugPrint(4, (_SBYTE*) "4.Global Search");

#ifdef MODULE_LOAD_SURVEY
	HAL_DebugPrint(5, (_SBYTE*) "5.Load Survey");
#endif

#ifdef MODULE_CRITICAL_PARAMETERS
	HAL_DebugPrint(6, (_SBYTE*) "6.Critical   0.Exit");
#else
	HAL_DebugPrint(6, (_SBYTE*) "0.Exit");
#endif

	HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) "Enter Choice :");
	fg_UserChoice = APP_GetUserAlphaNumeric ();	//get user's choice
	LcdClear();

	if ( (fg_UserChoice != MENU_DATA_DOWNLOAD) && (fg_UserChoice != MENU_BCS_INTERFACE) && \
		 (fg_UserChoice != MENU_COMMISSION) && (fg_UserChoice != MENU_GLOBAL_REPORT) && \
		 (fg_UserChoice != MENU_LOAD_SURVEY) && (fg_UserChoice != MENU_CRITICAL_PARAMETER) && \
		 (fg_UserChoice != MENU_EXIT))
	{
		HAL_DebugPrint(LCD_INFO_LINE,(_SBYTE*) "Invalid Choice");
		fg_UserChoice = 0xFF;
		//_getkey();
	}

	if (fg_UserChoice == MENU_DATA_DOWNLOAD)	//Downloading
	{
//		_UBYTE l_DownladingChoice = 0x00;
//		HAL_DebugPrint(1,(_SBYTE*) "   Downloading   ");
//		HAL_DebugPrint(2, (_SBYTE*) "1.Auto select");
//		HAL_DebugPrint(3, (_SBYTE*) "2.Manual select");
//		HAL_DebugPrint(4, (_SBYTE*) "3.Selective DWND & RC");
//		HAL_DebugPrint(5, (_SBYTE*) "4.Network Status");
//		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) "Enter Choice :");
//		l_DownladingChoice = APP_GetUserAlphaNumeric();			//get user's choice

	}

	else
	{
		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) "Please wait..." );
	}

	return;
}

//=========================================================================
void HAL_DispWelcome (void)
{
	LcdClear();
}

//=========================================================================
_UINT HAL_DispGetCurrentUserChoice (void)
{
	return(fg_UserChoice);
}

//=========================================================================
void HAL_DebugPrint (_UBYTE a_LineNo,_SBYTE* a_pData)
{
	fprintf(stderr,"\n%s ", a_pData);

#ifdef LINUX_ATMEL
	ClearLine(a_LineNo);
	goto_xy (a_LineNo, 1);
	LcdPrintf(a_pData);					//print in a_LineNo row 1st column
#endif

	return;
}
//=========================================================================
_UBYTE HAL_GetFromUser (void)
{
	_UBYTE l_Char = 0x00;

#ifdef LINUX_PC
	cin >> l_Char;
#else
	l_Char =  _getkey();
#endif

	return(l_Char);
}
