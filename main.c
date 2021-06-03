/************************************************************

Program:      DAKSH MRI 3.0.0
File:         DAKSH.cpp
Function:     Main (complete program listing in this file)
Description:  Downlaod the DLMS complient Meters. 
Author:       PK Jena
Environment:  GCC (Linux)
Notes:
Revisions:    3.0.0  ##/##/2010 First release
************************************************************/


#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include "functh.h"
#include "kt_misc.h"
#include "kt_global.h"

int main(void)
{
	int Port=5;

	printf("\n DAKSH MRI 3.0.0\n ");
	delay(1000);

	while(1)
	{
		printf("1.COMM PORT 1 \n2.COMM PORT 2 \n0.EXIT \nENTER CHOICE: ");
		//cin >> Port;
		scanf("%d",&Port);
		if(Port >= 0 && Port <= 2)
			break;
		else
		{
			printf("\nWRONG CHOICE  \nPRESS ANY KEY..");
			fflush(stdin);
			getchar();
		}
	}

	if (Port == 0)
	{
		return(0);
	}

	while(1)
	{
		printf("1.TYPE A   2.TYPE B \n3.TYPE C   0.EXIT \nENTER CHOICE: ");
		scanf("%d", &type);

		if((type >= 0) && (type <= 3))
			break;
		else
		{
			printf("\nWRONG CHOICE  \nPRESS ANY KEY..");
			fflush(stdin);
			getchar();
		}
	}

	//set baudrate
	if (Port == 1)
	{
		//INTR=0x0C;
		//PORT=0x3f8;
		//PIC=4;
		//BDRATE=9600;
	}
	else if (Port == 2)
	{
		//INTR=0x0B;
		//PORT=0x2f8;
		//PIC=3;
		//BDRATE=9600;
	}

	if(type == 0)
	{
		//clrscr();
		return(0);
	}
	
	if (Port == 1 || Port == 2)
	{
		commPort();
		memset(pktInfo,0,sizeof pktInfo);
		memset(RxDt,0,sizeof RxDt);
		selData();
	}

//	getch();
}
