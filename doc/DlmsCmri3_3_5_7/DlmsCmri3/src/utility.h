//============================================================================================
#ifndef _UTILITY_H_
#define _UTILITY_H_
//============================================================================================
#include "typedef.h"
#include "global.h"

//----------------------------
//Exposed function
void UTL_ConvertIntToASCDec(_UBYTE* a_pDest,_UINT a_Value);							//Integer to ASCII decinal - 0x0123 to "0123"
void UTL_ConvertASCHexToHex (_UBYTE* a_pDest, _UBYTE* a_pSrc, _UBYTE a_Len);		//ASCII Hex to Hex "1234" to
void UTL_ConvertHexStrToASCHexStr (_UBYTE* a_pDest, _UBYTE* a_pSrc, _UBYTE a_Len);
void UTL_ReverseString (_UBYTE* a_pStart, _UBYTE a_Len);
_UBYTE UTL_CompareString(_UBYTE* a_pStr1, _UBYTE* a_pStr2, _UBYTE a_Len);			//comparing two unsigned string
void UTL_Wait(_SINT a_miliSec);
void UTL_RTCInitialize (void);
void UTL_RTCRead(struct rtc_time* a_sRTC);
_ULONG UTL_GetHHUUTCTime(void);
void ConvertUTCtoRTC(_ULONG a_UTCinSeconds, RtcTypeDef *a_pRtcStuct);
//============================================================================================
#endif /* _UTILITY_H_ */
//============================================================================================
