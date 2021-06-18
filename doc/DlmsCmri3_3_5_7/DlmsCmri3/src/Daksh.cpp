/************************************************************
Program:      DAKSH MRI
File:         DAKSH.cpp
Function:     Main (complete program listing in this file)
Description:  Downlaod the DLMS complient Meters.
Author:       Dinesh Singh, PK Jena (HTOS Compatible), Nemaram Mundel(Linux OS Compatible)
Environment:  Turbo C++ version 3, Windows 3.1, DOS 6.0., HTOS, Linux
Notes:
Revisions:    3.0.0  ##/##/2010 First release
			  4.0.0	 //Compatible to HTOS
			  3.5.0 // Compatible to Linux OS release Sep 2015
************************************************************/
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/rtc.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>

#include "rs232.h"
#include "FUNCTH.h"
#include "comm.h"
#include "Config.h"
#include "utility.h"

//static struct termios oldt;
void changemode(int dir);

int main (void)
{
#ifdef LINUX_ATMEL
	fprintf(stderr, "\n%s file remove\n", FILE_IRLOG_TXT);
	remove(FILE_IRLOG_TXT);
	initDisplay();
	LcdClear();
#endif
	mkdir("/DLMS", 777);
	fprintf(stderr, "\nDAKSH CMRI(1001-0615)\n");
	//sleep(1);
	selData();
}

void changemode(int dir)
{
  static struct termios oldt, newt;
 
  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
