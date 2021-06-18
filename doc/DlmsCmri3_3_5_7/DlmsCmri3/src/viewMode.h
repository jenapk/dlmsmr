/*
 * viewMode.h
 *
 *  Created on: 21-Sep-2015
 *      Author: mundel
 */
//======================================================================
#ifndef _VIEWMODE_H_
#define _VIEWMODE_H_
//======================================================================
/*
 * viewMode.cpp
 *
 *  Created on: 21-Sep-2015
 *      Author: mundel
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/rtc.h>
#include <dirent.h>

//#include "Upload.H"
//#include "Program.H"
#include "display.h"
#include "Config.h"
#include "global.h"
#include "typedef.h"
#include "FUNCTH.h"
#include "rs232.h"
#include "utility.h"
#include "display.h"
#include "AES128.h"
#include "comm.h"
#include "pdsequivalent.h"
#include "Upload.h"
#include "auth.h"
#include "Program.h"

//============================================================================================
int ViewRequest(unsigned char classID,unsigned char Obis_a,unsigned char Obis_b,\
				unsigned char Obis_c,unsigned char Obis_d,unsigned char Obis_e,\
				unsigned char Obis_f,unsigned char Attr);
int View_ModeTypeA(void);
int View_ModeTypeB(void);
unsigned long Calculate_Value (void);
void Display_Value (_UBYTE a_LineNo, _SBYTE* a_PreMessage, double l_Value, _SBYTE* a_PostMessage);
void Display_Value1 (_UBYTE a_LineNo, _SBYTE* a_Message, double l_Value1);
void Display_Signed_Value (_UBYTE a_LineNo, _SBYTE* a_Message, unsigned long l_Value);
//======================================================================
#endif /* _VIEWMODE_H_ */
//======================================================================
