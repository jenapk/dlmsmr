//HAL_DISPLAY.H

//=======================================================
#ifndef _HPL_DISPLAY_H
#define _HPL_DISPLAY_H
//=======================================================
#include "typedef.h"

//display menu list
#define MENU_MAIN								0x00
	#define MENU_DATA_DOWNLOAD					0x01
		#define MENU_DATA_DOWNLOAD_MANUAL				0x11
		#define MENU_DATA_DOWNLOAD_AUTO					0x12
		#define MENU_SPECIALMODE						0x13
		#define MENU_CHECK_DWND_STATUS_OF_A_NWK			0x14
	#define MENU_BCS_INTERFACE					0x02
	#define MENU_COMMISSION						0x03
		#define MENU_COMMISSION_NEW_NETWORK				0x31
		#define MENU_COMMISSION_EXISTING 				0x32
		#define MENU_COMMISSION_GROUP_1BY1				0x33
		#define MENU_COMMISSION_UNCOMMISSION_METERLIST	0x34
	#define MENU_GLOBAL_REPORT					0x04
	#define MENU_LOAD_SURVEY					0x05
	#define MENU_CRITICAL_PARAMETER				0x06
		#define MENU_CRITICAL_PASSWORD					0x61
		#define MENU_CRITICAL_MDRESET					0x62
		#define MENU_CRITICAL_TOD						0x63
		#define MENU_CRITICAL_RTC						0x64
#define MENU_EXIT								0x00

//Function Prototypes
_UINT HAL_DispGetCurrentUserChoice (void);
_UBYTE HAL_DispGetCurrentMenu (void);
_UBYTE HAL_GetFromUser (void);		//getting key stroke from user

void HAL_DispMenu (void);
void APP_DispLine (_UBYTE a_LineNo, _SBYTE* a_pData);	//to display in aline
void HAL_DebugPrint (_UBYTE a_LineNo,_SBYTE* a_pData);
void APP_DispRefresh (void);							//refresh the display contents
void HAL_DispWelcome (void);
//=======================================================
#endif	//_HPL_DISPLAY_H
//=======================================================
