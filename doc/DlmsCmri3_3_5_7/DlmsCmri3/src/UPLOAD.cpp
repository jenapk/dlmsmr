#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/select.h>

#include "global.h"
#include "FUNCTH.h"
#include "display.h"
#include "hhuLcd.h"
#include "Config.h"
#include "utility.h"


#if 1
//static struct ffblk ffblk;
static int done,c;
static unsigned char filename[200][13];//[30][13]
static unsigned char SAPAdd[200];  //30
static int pktnumber = 0,globalj=0;
static unsigned char ServerAdd = 0;

FILE *fp2, *fp3,*Ustream_r;


void FileTransfer(void)
{
	int i=0;
	fg_SendBuffer[1] = 0xA8;

	if(pktnumber == 0)
	{
		fp2 = NULL;
		for(i=0;i<200;i++)   //30
			if(ServerAdd == SAPAdd[i])
			{
				FileName[0] = '/';
				FileName[1] = 'D';
				FileName[2] = 'L';
				FileName[3] = 'M';
				FileName[4] = 'S';
				FileName[5] = '/';
				FileName[6]  = filename[i][0];
				FileName[7]  = filename[i][1];
				FileName[8]  = filename[i][2];
				FileName[9]  = filename[i][3];
				FileName[10]  = filename[i][4];
				FileName[11]  = filename[i][5];
				FileName[12]  = filename[i][6];
				FileName[13]  = filename[i][7];
				FileName[14]  = '.';
				FileName[15]  = 'd';
				FileName[16]  = 'd';
				FileName[17]  = 't';
				FileName[18]  = '\0';
				fp2 =  fopen((const char*)FileName,"r");
				if(fp2 == NULL)
				{
					fprintf(stderr, "file opening error %s",FileName);
				}
				break;
			}
		fg_SendBuffer[c++] = 0xC4;
		fg_SendBuffer[c++] = 0x01;
		fg_SendBuffer[c++] = 0x81;
		fg_SendBuffer[c++] = 0x00;
		fg_SendBuffer[c++] = 0x01;
		if(fp2 == NULL)
		{
			fg_SendBuffer[1] = 0xA0;
			fg_SendBuffer[c++] = 0x00;
			fg_SendBuffer[2] = c+1;
			fg_SendBuffer[c+2] = 0x7E;
			fcs(fg_SendBuffer+1, 5, 1);
			fcs(fg_SendBuffer+1, c-1, 1);
			SendPkt(c+3);
			return;
		}
		fg_SendBuffer[c++] = i;
		fg_SendBuffer[c++] = 0x09;
		fg_SendBuffer[c++] = 117;

		for(i=0;i<117;i++)
		{
			ch = fgetc(fp2);
			if(ch == -1)
			{
				fg_SendBuffer[18] = c-19;
				fg_SendBuffer[1] = 0xA0;
				fclose(fp2);
				break;
			}
			fg_SendBuffer[c++] = ch;
		}
	}
	else
	{
		fg_SendBuffer[c++] = 0x09;
		fg_SendBuffer[c++] = 126;
		for(i=0;i<126;i++)
		{
			VolumeOfData++;
			ch = fgetc(fp2);
			if(ch == -1)
			{
				fg_SendBuffer[9] = c-10;
				fg_SendBuffer[1] = 0xA0;
				fclose(fp2);
				break;
			}
			fg_SendBuffer[c++] = ch;
		}
	}
	fg_SendBuffer[2] = c+1;
	fg_SendBuffer[c+2] = 0x7E;
	fcs(fg_SendBuffer+1, 5, 1);
	fcs(fg_SendBuffer+1, c-1, 1);
	SendPkt(c+3);
}

void SAPAssignment(void)
{
	int i=0,k=0;
	DIR *dir;
	struct dirent *ent;

	if((dir = opendir("/DLMS")) != NULL)
	{
		while((ent = readdir(dir)) != NULL)
		{
			strcpy((_SBYTE*)filename[i], (const char*)ent->d_name);
			if(filename[i][0] != '.')
			{
				i++;
				SAPAdd[i-1] = i + 16;
			}
		}
		closedir(dir);
	}
	else
	{
		perror("Could not open directory");
		return;
	}

	fg_SendBuffer[1] = 0xA8;
/*	///mundel
	done = findfirst("\\DLMS\\*.*",&ffblk,0);
	while (!done)
	{
		//printf("  %s\n", ffblk.ff_name);
		strcpy(filename[i++],ffblk.ff_name);
		SAPAdd[i-1] = i + 16;
		done = findnext(&ffblk);
		if(i==200)   //30
			break;
	}
*/
	if(pktnumber == 0)
	{
		fg_SendBuffer[c++] = 0xC4;
		fg_SendBuffer[c++] = 0x01;
		fg_SendBuffer[c++] = 0x81;
		fg_SendBuffer[c++] = 0x00;
		fg_SendBuffer[c++] = 0x01;
		fg_SendBuffer[c++] = i;
		for(globalj = 0; globalj < 7 && globalj < i; globalj++)
		{
			fg_SendBuffer[c++] = 0x02;
			fg_SendBuffer[c++] = 0x02;
			fg_SendBuffer[c++] = 0x12;
			fg_SendBuffer[c++] = 0x00;
			fg_SendBuffer[c++] = SAPAdd[globalj];
			fg_SendBuffer[c++] = 0x09;
			fg_SendBuffer[c++] = 0x08;
			for(k = 0; k < 8; k++)
			{
				fg_SendBuffer[c++] = filename[globalj][k];
			}
		}
		if(globalj < i+1 && i < 8)
		  fg_SendBuffer[1] = 0xA0;
	}
	else
	{
		for(int l=0; l<7 && globalj < i;l++,globalj++)
		{
			fg_SendBuffer[c++] = 0x02;
			fg_SendBuffer[c++] = 0x02;
			fg_SendBuffer[c++] = 0x12;
			fg_SendBuffer[c++] = 0x00;
			fg_SendBuffer[c++] = SAPAdd[globalj];
			fg_SendBuffer[c++] = 0x09;
			fg_SendBuffer[c++] = 0x08;
			for(k = 0; k < 8; k++)
				fg_SendBuffer[c++] = filename[globalj][k];
		}
		if(globalj == i)
			fg_SendBuffer[1] = 0xA0;
	}
	fg_SendBuffer[2] = c+1;
	fg_SendBuffer[c+2] = 0x7E;
	fcs(fg_SendBuffer+1, 5, 1);
	fcs(fg_SendBuffer+1, c-1, 1);
	SendPkt(c+3);
}

int selData_Upload(void)
{
	unsigned char delfile[50];
	static int fg_Timeout = 30;
	char ch;
	int time2 = 0;
	unsigned int rep,EndFlag=0,TempFlag=0,year =0,time1=0;
    //struct dostime_t reset;
    //struct dosdate_t reset1;
	LcdClear();
	LoadSurveyFlag = 0;
	PGbar=0;
	HAL_DebugPrint(1, (_SBYTE*) "    MRI SERVER   ");
	HAL_DebugPrint(2, (_SBYTE*) "If no communication");
	HAL_DebugPrint(3, (_SBYTE*) "it automatic exit ");
	time2=0;
	while(time2 < fg_Timeout)// 905350  //655350
	{
		memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
		sprintf((_SBYTE*)g_DispBuffer, "after %d sec", (fg_Timeout-time2));
		HAL_DebugPrint(4, (_SBYTE*) g_DispBuffer);
		time2 += 3;

		if((EndFlag>=2)&&(TempFlag))
		{
			EndFlag =0;
		}
		Read(fg_RxBuffer);
		if(FrameRcvFlag == 1)
		{
			time2 = 0;
			FrameRcvFlag = 0;
			if(TempFlag)
			{
				///ErrPrt(1,2,"PRESS ANY KEY EXIT."); ///mundel
			}
			c = 0;
			if(fg_RxBuffer[5] == 0x53)
			{				      
                EndFlag++;
				fg_SendBuffer[c++] = 0x7E;
				fg_SendBuffer[c++] = 0xA0;
				fg_SendBuffer[c++] = 0x07;
				fg_SendBuffer[c++] = fg_RxBuffer[4];
				fg_SendBuffer[c++] = fg_RxBuffer[3];
				fg_SendBuffer[c++] = 0x73;
				fg_SendBuffer[c++] = 0x8E;
				fg_SendBuffer[c++] = 0x7F;
				fg_SendBuffer[c++] = 0x7E;
				fcs(fg_SendBuffer+1, 5, 1);
				SendPkt(c);
				ClearPacket();
			}
			else if(fg_RxBuffer[5] == 0x93)
			{
				fg_SendBuffer[c++] = 0x7E;
				fg_SendBuffer[c++] = 0xA0;
				fg_SendBuffer[c++] = 0x1E;
				fg_SendBuffer[c++] = fg_RxBuffer[4];
				fg_SendBuffer[c++] = fg_RxBuffer[3];
				fg_SendBuffer[c++] = 0x73;
				fg_SendBuffer[c++] = 0x8E;
				fg_SendBuffer[c++] = 0x7F;
				fg_SendBuffer[c++] = 0x81;
				fg_SendBuffer[c++] = 0x80;
				fg_SendBuffer[c++] = 0x12;
				fg_SendBuffer[c++] = 0x05;
				fg_SendBuffer[c++] = 0x01;
				fg_SendBuffer[c++] = 0x80;
				fg_SendBuffer[c++] = 0x06;
				fg_SendBuffer[c++] = 0x01;
				fg_SendBuffer[c++] = 0x80;
				fg_SendBuffer[c++] = 0x07;
				fg_SendBuffer[c++] = 0x04;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x01;
				fg_SendBuffer[c++] = 0x08;
				fg_SendBuffer[c++] = 0x04;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x01;
				fg_SendBuffer[c++] = 0x53;
				fg_SendBuffer[c++] = 0x3B;
				fg_SendBuffer[c++] = 0x7E;
				fcs(fg_SendBuffer+1, 5, 1);
				fcs(fg_SendBuffer+1, c-4, 1);
				SendPkt(c);
				ClearPacket();
			}
			else if(fg_RxBuffer[8] == 0xE6 && fg_RxBuffer[9] == 0xE6 && fg_RxBuffer[10] == 0x00)
			{
				fg_SendBuffer[c++] = 0x7E;
				fg_SendBuffer[c++] = 0xA0;
				fg_SendBuffer[c++] = 0x37;
				fg_SendBuffer[c++] = fg_RxBuffer[4];
				fg_SendBuffer[c++] = fg_RxBuffer[3];
				fg_SendBuffer[c++] = 0x30;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0xE6;
				fg_SendBuffer[c++] = 0xE7;
				fg_SendBuffer[c++] = 0x00;
				if(fg_RxBuffer[11] == 0x60)
				{
                   // LcdClear();
					TempFlag = 1;
					//ErrPrt(1,2,"START TIME         "); sys_Time(3);
					fg_SendBuffer[c++] = 0x61;
					fg_SendBuffer[c++] = 0x29;
					fg_SendBuffer[c++] = 0xA1; 
					fg_SendBuffer[c++] = 0x09;
					fg_SendBuffer[c++] = 0x06;
					fg_SendBuffer[c++] = 0x07;
					fg_SendBuffer[c++] = 0x60; 
					fg_SendBuffer[c++] = 0x85;
					fg_SendBuffer[c++] = 0x74;
					fg_SendBuffer[c++] = 0x05; 
					fg_SendBuffer[c++] = 0x08; 
					fg_SendBuffer[c++] = 0x01; 
					fg_SendBuffer[c++] = 0x01;
					fg_SendBuffer[c++] = 0xA2; 
					fg_SendBuffer[c++] = 0x03; 
					fg_SendBuffer[c++] = 0x02; 
					fg_SendBuffer[c++] = 0x01; 
					fg_SendBuffer[c++] = 0x00; 
					fg_SendBuffer[c++] = 0xA3;
					fg_SendBuffer[c++] = 0x05; 
					fg_SendBuffer[c++] = 0xA1;
					fg_SendBuffer[c++] = 0x03;
					fg_SendBuffer[c++] = 0x02; 
					fg_SendBuffer[c++] = 0x01;
					fg_SendBuffer[c++] = 0x00; 
					fg_SendBuffer[c++] = 0xBE;
					fg_SendBuffer[c++] = 0x10;
					fg_SendBuffer[c++] = 0x04; 
					fg_SendBuffer[c++] = 0x0E;
					fg_SendBuffer[c++] = 0x08; 
					fg_SendBuffer[c++] = 0x00; 
					fg_SendBuffer[c++] = 0x06;
					fg_SendBuffer[c++] = 0x5F; 
					fg_SendBuffer[c++] = 0x1F; 
					fg_SendBuffer[c++] = 0x04; 
					fg_SendBuffer[c++] = 0x00; 
					fg_SendBuffer[c++] = 0x00; 
					fg_SendBuffer[c++] = 0x10;
					fg_SendBuffer[c++] = 0x1C; 
					fg_SendBuffer[c++] = 0x00; 
					fg_SendBuffer[c++] = 0xC8;
					fg_SendBuffer[c++] = 0x00; 
					fg_SendBuffer[c++] = 0x07;
					fg_SendBuffer[c++] = 0xB2; 
					fg_SendBuffer[c++] = 0x2A;
					fg_SendBuffer[c++] = 0x7E;
					fcs(fg_SendBuffer+1, 5, 1);
					fcs(fg_SendBuffer+1, c-4, 1);
					SendPkt(c);
					ClearPacket();
				}
				else if(fg_RxBuffer[11] == 0xC0)
				{
					obis_a = fg_RxBuffer[16];
					obis_b = fg_RxBuffer[17];
					obis_c = fg_RxBuffer[18];
					obis_d = fg_RxBuffer[19];
					obis_e = fg_RxBuffer[20];
					obis_f = fg_RxBuffer[21];
					attr   = fg_RxBuffer[22];
					if(fg_RxBuffer[15] == 0x11 && fg_RxBuffer[16] == 0x00 && fg_RxBuffer[17] == 0x00 && fg_RxBuffer[18] == 0x29 && fg_RxBuffer[19] == 0x00 && fg_RxBuffer[20] == 0x00 && fg_RxBuffer[21] == 0xFF)
					{
						pktnumber = 0;
						SAPAssignment();
						pktnumber++;
					}
					else if(fg_RxBuffer[15] == 0x01 && fg_RxBuffer[16] == 0x00 && fg_RxBuffer[17] == 0x00 && fg_RxBuffer[18] == 0x60 && fg_RxBuffer[19] == 0x32 && fg_RxBuffer[20] == 0x00 && fg_RxBuffer[21] == 0xFF)
					{
						pktnumber = 0;
						ServerAdd = fg_RxBuffer[3] >> 1;
						FileTransfer();
						pktnumber++;
					}
					else if(fg_RxBuffer[15] == 0x01 && fg_RxBuffer[16] == 0x00 && fg_RxBuffer[17] == 0x00 && fg_RxBuffer[18] == 0x60 && fg_RxBuffer[19] == 0x32 && fg_RxBuffer[20] == 0x01 && fg_RxBuffer[21] == 0xFF)
					{
						ServerAdd = fg_RxBuffer[3] >> 1;
						fg_SendBuffer[c++] = 0xC4;
						fg_SendBuffer[c++] = 0x01;
						fg_SendBuffer[c++] = 0x81;
						fg_SendBuffer[c++] = 0x00;
						fg_SendBuffer[c++] = 0x01;
						fg_SendBuffer[c++] = 0x00;
						fg_SendBuffer[1] = 0xA0;
						fg_SendBuffer[2] = c+1;
						fg_SendBuffer[c+2] = 0x7E;
						fcs(fg_SendBuffer+1, 5, 1);
						fcs(fg_SendBuffer+1, c-1, 1);
						SendPkt(c+3);
						//system("del DLMS\\*.* /Q");
						/*	///mundel
						done = findfirst("\\DLMS\\*.*",&ffblk,0);
						while (!done)
						{
							strcpy((_SBYTE*)delfile,"\\DLMS\\");
							strcat((_SBYTE*)delfile,ffblk.ff_name);
							remove((const char*)delfile);
							done = findnext(&ffblk);
                            ErrPrt(1,6,"     ::Erased::     ");
 
						}
						*/
                        Read(fg_RxBuffer);
                        // while( (fg_RxBuffer[0] != 0x01 ) && (time1 < 15000) )    //655350
                        if(fg_RxBuffer[0] == 0x00 )
                        {
                           printf("\n  CMRI RTC NOT SET");
                           UTL_Wait(500);
                        }
                        else 
                        {
                        	/*	///mundel
                          	reset1.day=fg_RxBuffer[4];
                        	reset1.month=fg_RxBuffer[3];
                            year=  (fg_RxBuffer[1]*0x100+fg_RxBuffer[2]); 
                        	reset1.year=year;
                        	_dos_setdate(&reset1);                    
                        	reset.hour=fg_RxBuffer[5];
                        	reset.minute=fg_RxBuffer[6];
                        	reset.second = 0;
                        	_dos_settime(&reset);
                        	*/
                            // UTL_Wait(1);
                            // time1++;
                            fg_SendBuffer[0] = 'O';
                            fg_SendBuffer[1] = 'K';                          
                            SendPkt(2);
                           // printf(" [%d] [%d] [%d] [%d] [%d] [%d]",fg_RxBuffer[0],fg_RxBuffer[1],fg_RxBuffer[2],fg_RxBuffer[3],fg_RxBuffer[4],fg_RxBuffer[5],fg_RxBuffer[6]);
                            printf("\n  CMRI RTC SET");
                            UTL_Wait(500);
                            
                        }                  
                        
					}
					else
					{
						fg_SendBuffer[c++] = 0xC4;
						fg_SendBuffer[c++] = 0x01;
						fg_SendBuffer[c++] = 0x81;
						fg_SendBuffer[c++] = 0x01;
						fg_SendBuffer[c++] = 0x0D;		
						fg_SendBuffer[1] = 0xA0;
						fg_SendBuffer[2] = c+1;
						fg_SendBuffer[c+2] = 0x7E;
						fcs(fg_SendBuffer+1, 5, 1);
						fcs(fg_SendBuffer+1, c-1, 1);
						SendPkt(c+3);
					}
				}
			}
			else if(fg_RxBuffer[5]&0x01 == 0x01)
			{
				memset(g_DispBuffer, 0, sizeof(g_DispBuffer));
				sprintf((_SBYTE*)g_DispBuffer, "VOLUME:%lu BYTES", VolumeOfData);
				HAL_DebugPrint(5, (_SBYTE*) g_DispBuffer);

				fg_SendBuffer[c++] = 0x7E;
				fg_SendBuffer[c++] = 0xA0;
				fg_SendBuffer[c++] = 0x37;
				fg_SendBuffer[c++] = fg_RxBuffer[4];
				fg_SendBuffer[c++] = fg_RxBuffer[3];
				fg_SendBuffer[c++] = 0x30;
				fg_SendBuffer[c++] = 0x00;
				fg_SendBuffer[c++] = 0x00;
				if(obis_a == 0x00 && obis_b == 0x00 && obis_c == 0x29 && obis_d == 0x00 && obis_e == 0x00 && obis_f == 0xFF)
				{
					SAPAssignment();
				}
				if(obis_a == 0x00 && obis_b == 0x00 && obis_c == 0x60 && obis_d == 0x32 && obis_e == 0x00 && obis_f == 0xFF)
				{
					FileTransfer();
				}
			}
			//================================ propriety  method for data transfer between BCS and CMRI  ===============================================
		
			
			//===================================================================================================================================
		}
        else 
        {
            UTL_Wait(100);
            VolumeOfData = 0;
        }
	}
	return 0;
}

#endif
