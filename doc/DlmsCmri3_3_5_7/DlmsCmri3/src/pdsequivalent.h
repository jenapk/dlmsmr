//========================================================================================
#ifndef _PSDEQUIVALENT_H_
#define _PSDEQUIVALENT_H_
//========================================================================================
#include "typedef.h"
#include "Config.h"

#ifdef LINUX_PC
	#define ENT		13

int goto_xy(int row,int col);
int LcdClear(void);
_UBYTE _getkey(void);
int initDisplay(void);
_SINT LcdPrintf(const char *appbuf1);
int LcdPrint_Image(const char *imgbuff,unsigned int Imgcnt);

#endif

void ClearLine (_UBYTE a_LineNo);
void ClearScreen(void);
void buzzer_on (void);
void buzzer_off (void);
_UBYTE APP_GetHHUSlNo (_UBYTE* a_Dest);
_UINT APP_GetUserNumericString (void);
void HAL_DispImage (void);
void HAL_PleaseWait(_UBYTE a_Line);
_SINT APP_GetUserAlphaNumeric (void);
void APP_BuzzerSuccess (void);
void APP_BuzzerFailure (void);
int get_machineid_FS(_SBYTE *machine_buff);
unsigned char kb_hit(void);
void APP_GetLLS_Key_String (_UBYTE* a_pBuffer);
//========================================================================================
#endif /* _PSDEQUIVALENT_H_ */
//========================================================================================
