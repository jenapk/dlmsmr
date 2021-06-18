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
#include <pthread.h>
#include "functh.h"

#include "kt_misc_func.h"
#include "kt_global.h"
#include "kt_libUART.h"

pthread_t fg_thdUART;
pthread_t fg_thdApp;

uint32_t x;
//======================================================================================
int main(void)

{
	int l_iRetVal;
	printf("\n================================\n");
	printf(  "KT DLMS Client Application 0.9.0\n");
	printf("================================\n");
	delay(1000);

	memset(pktInfo,0,sizeof pktInfo);
	memset(RxDt,0,sizeof RxDt);

	//com port open
	g_portNo = 16;
	kt_libUART_Open(g_portNo, 9600);

	l_iRetVal = pthread_create(&fg_thdUART, NULL, kt_uartPoll, NULL);
	if(l_iRetVal != 0)
	{
		printf("Thread (UART) creation failed.\n");
		return(-1);
	}

	printf("Thread (UART) created \n");

	l_iRetVal = pthread_create(&fg_thdApp, NULL, kt_appProcess, NULL);
	if(l_iRetVal != 0)
	{
		printf("Thread (App) creation failed.\n");
		return(-1);
	}

	printf("Thread (App) created \n");


	//join threads
	pthread_join(fg_thdApp, NULL);
	pthread_join(fg_thdUART, NULL);

	printf("\nExecution completed.\n ");
}
