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
#include "kt_misc_func.h"
#include "kt_global.h"
#include "kt_libUART.h"

//======================================================================================
int main(void)
{
	printf("\n DAKSH MRI 3.0.0\n ");
	delay(1000);

	memset(pktInfo,0,sizeof pktInfo);
	memset(RxDt,0,sizeof RxDt);

	//com port open
	g_portNo = 16;
	kt_libUART_Open(g_portNo, 9600);


	selData();

	printf("Execution completed.\n ");
}
