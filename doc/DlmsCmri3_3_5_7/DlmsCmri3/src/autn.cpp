#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "comm.h"
#include "Config.h"
#include "FUNCTH.h"
#include "utility.h"
#include "auth.h"
#include "hhuLcd.h"
#include "display.h"
#include "rs232.h"

#if 1

unsigned char L_RxBuffer[100],L_Index;
int CMRI_AUTH (void)
{
	FILE *stream_wa;
    FILE  *fp2;
	char Write_Flag=0;
	int i,j,time2;
	static int fg_Timeout = 9;
    unsigned char retval = 0;

    LcdClear();
    HAL_DebugPrint(1, (_SBYTE*) "    MRI SERVER    \n");
    HAL_DebugPrint(2, (_SBYTE*) "If no communication");
    HAL_DebugPrint(3, (_SBYTE*) "it automatic exit ");
	ClrPacket();
	//stream_wa = fopen("\\AUTH12.txt","w");
	// while(!kbhit())
	// {
    time2=0;
   	while(time2 < fg_Timeout)// 905350  //655350
   	{
   		memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
   		sprintf((_SBYTE*)g_DispBuffer, "after %d sec", (fg_Timeout-time2));
		HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
   		time2 += 3;
//#ifdef LINUX_ATMEL
//	while(l_TempVal == 0xFF)
//	{
//		l_TempVal = kb_hit();
//		fprintf(stderr,"\nPress a key l_TempVal = %lu\n", l_TempVal);
//
//#else
//	while(!kb_hit()) 	//		!kbhit()
//	{
//#endif

        Read_auth(L_RxBuffer);
		if((L_RxBuffer[0]=='A')&&(L_RxBuffer[1]=='U')&&(L_RxBuffer[2]=='T')&&(L_RxBuffer[3]=='H')&&(L_RxBuffer[L_Index-1]=='#'))
		{
            time2=0;
			if((L_RxBuffer[4] == '1')&&(L_RxBuffer[5] == '6'))   //comopare   16   after auth first frame decision to create new file
            {
                Write_Flag = 0;
            }
			if(Write_Flag==0)
            {
                stream_wa = fopen(APP_FILE_DLMS_AUTH,"w");
                fprintf(stream_wa,"\n");
            }
			for(i=4;i<L_Index-1;i++)
            {
                fprintf(stream_wa,"%c",L_RxBuffer[i]);
            }
			fprintf(stream_wa,"\n");
			if(!Write_Flag)
			{
				//SendByte(0x7F);
				RS232_PortSendByte(SERAIL_RJ_PORT_NO, 0x7F);

			}
			else
            {
                //SendByte(0x7E);
                RS232_PortSendByte(SERAIL_RJ_PORT_NO, 0x7E);
            }
			ClrPacket();
			Write_Flag =1;
			
		}
		else if((L_RxBuffer[0]=='T')&&(L_RxBuffer[1]=='O')&&(L_RxBuffer[2]=='D')&&(L_Index >= (17+(L_RxBuffer[3]-48)*9)))
		{
            time2=0;
			fclose(stream_wa);
			stream_wa = fopen(FILE_TOD_TXT,"w");
			for(i=3;i<17;i++)
            {
                fprintf(stream_wa,"%c",L_RxBuffer[i]);//no of tod and activation date
            }
			fprintf(stream_wa,"\n");
			for(j=0;j<(L_RxBuffer[3]-48);j++)
			{
				for(i=0;i<9;i++)
				fprintf(stream_wa,"%c",L_RxBuffer[j*9+17+i]);//13
				fprintf(stream_wa,"\n");
			}
			//SendByte(0x7E);//7E
			RS232_PortSendByte(SERAIL_RJ_PORT_NO, 0x7E);
			fclose(stream_wa);
			ClrPacket();
		}
        
        else if((L_RxBuffer[0]=='P')&&(L_RxBuffer[1]=='R')&&(L_RxBuffer[2]=='O')&&(L_Index >= 28))
		{
            time2=0;
			stream_wa = fopen(FILE_PROG_TXT,"w");
            fp2 = fopen(FILE_DATE_TXT,"w");
    
            for(j=19;j<27;j++)
            {
                fprintf(fp2,"%c",L_RxBuffer[j]);
            }
			for(i=3;i<17;i++)
            {
                fprintf(stream_wa,"%c",L_RxBuffer[i]);
            }
			//00( Md int period) ,00 (billing date)  , 00 (profile cap. period )  , 00000Physical address
			//SendByte(0x7E);//7E
			RS232_PortSendByte(SERAIL_RJ_PORT_NO, 0x7E);
			fclose(stream_wa);
            fclose(fp2);
			ClrPacket();
			LcdClear();
			HAL_DebugPrint(1, (_SBYTE*) "CMRI SUCESSFULLY");
			HAL_DebugPrint(2, (_SBYTE*) "AUTHENTICATE");
			HAL_DebugPrint(3, (_SBYTE*) "PRESS ANY KEY EXIT");
			_getkey();
			return(1);
		}
        else if((L_RxBuffer[0]=='R')&&(L_RxBuffer[1]=='T')&&(L_RxBuffer[2]=='C')&&(L_RxBuffer[3]=='1')&&(L_RxBuffer[L_Index - 1]=='#'))
		{
			#if 0
            time2=0;
            //hex_decimal            
            L_RxBuffer[5]  =  ((L_RxBuffer[5]-48)  * 10) + (L_RxBuffer[6] -48) ;
            L_RxBuffer[8]  =  ((L_RxBuffer[8]-48)  * 10) + (L_RxBuffer[9] -48) ;
            L_RxBuffer[13] =  ((L_RxBuffer[13]-48) * 10) + (L_RxBuffer[14]-48) ;
            L_RxBuffer[16] =  ((L_RxBuffer[16]-48) * 10) + (L_RxBuffer[17]-48) ;
            L_RxBuffer[19] =  ((L_RxBuffer[19]-48) * 10) + (L_RxBuffer[20]-48) ;
            L_RxBuffer[22] =  ((L_RxBuffer[22]-48 )* 10) + (L_RxBuffer[23]-48) ;            

				sd.day  = L_RxBuffer[5];
				sd.month = L_RxBuffer[8];
				sd.year  = L_RxBuffer[13]+2000;
				st.hour  =  L_RxBuffer[16];
				st.minute  = L_RxBuffer[19];
				st.second  = L_RxBuffer[22];
				st.hsecond  =0;
				//pokeb(0x0040,0x003f,1);
				
				retval = _dos_setdate(&sd);
				if(retval == 0)
				{
					retval = _dos_settime(&st);
					if(retval == 0)
					{
						//pokeb(0x0040,0x003f,0);
                        LcdClear();
						printf("\n\nAuto Sink : Done\n\nPress A Key");
						//ch=0xdd;
						//WritePort((unsigned char*)&ch,1);
					}
					else
					{
						//pokeb(0x0040,0x003f,0);
                        LcdClear();
						printf("\n\nAuto Sink : Failed\n\nPress A Key");
						//ch=0xcc;
						//WritePort((unsigned char*)&ch,1);
					}
				}
				else
				{
					//pokeb(0x0040,0x003f,0);
					printf("\n\nAuto Sink : Failed\n\nPress A Key");
					//ch=0xcc;
					//WritePort((unsigned char*)&ch,1);
				}
			LcdClear();
            
            //printf(" \n %d %d %d %d %d %d",L_RxBuffer[5],L_RxBuffer[8],L_RxBuffer[13],L_RxBuffer[16],L_RxBuffer[19],L_RxBuffer[22]);_getkey();
            
        	UTL_Wait(10);
            SendByte(0x7F);
            ClrPacket();
			#endif
        }
        else if((L_RxBuffer[0]=='R')&&(L_RxBuffer[1]=='T')&&(L_RxBuffer[2]=='C')&&(L_RxBuffer[3]=='0')&&(L_RxBuffer[L_Index - 1]=='#'))
		{
            time2=0;
            UTL_Wait(100);
            //SendByte(0x7F);
            RS232_PortSendByte(SERAIL_RJ_PORT_NO, 0x7F);
            printf("\nRtc Not Sync");
            ClrPacket();           
        }
	}

	//fclose(stream_wa);
	//exit(0);
	//fprintf(stderr, "test point 2\n");
	return(1);
}
int Read_auth(unsigned char *RxDt)
{
	unsigned long time1;
	//UTL_Wait(200);
//#ifdef LINUX_ATMEL
//	while(kb_hit() == 0xFF)
//#else
//	while(!kb_hit()) 	//		!kbhit()
//#endif
	while(1)
	{
		time1=0;
		while((rx_flag==0)&&(time1<3000)) // 905350  //655350
        {
            UTL_Wait(1);
			time1++;     
        
        }        
		if(time1>=3000)
        {
			rx_flag = 0;
			return 0;
        }
		else
        {
			if(head != tail)
			{
				RxDt[L_Index++] = ReadByte();
			}
        }
        
		if( ((RxDt[L_Index-1] == '#')&&(RxDt[0]=='A'))||\
			((RxDt[0]=='T') &&(L_Index >= (17+(RxDt[3]-48)*9)))||\
			((RxDt[0]=='P') && (L_Index >= 28) ) || ((RxDt[0]=='R')&&(RxDt[1]=='T')&&(RxDt[2]=='C')&&(RxDt[L_Index - 1]=='#')))
		{
			break;
		}
	}
	rx_flag=0;
	return 1;
	//exit(1);
}

void ClrPacket(void)
{
	L_Index = tail = head = 0;
	memset(L_RxBuffer,255,sizeof(L_RxBuffer));
}
char hex_decimal(char hex)   /* Function to convert hexadecimal to decimal. */
{
	unsigned char sum=0;
	sum = (((hex/16)*10)+hex%16); 
	return sum;
}

#endif
