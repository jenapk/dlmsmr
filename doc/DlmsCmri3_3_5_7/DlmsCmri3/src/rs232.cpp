#include <stdlib.h>

#include "typedef.h"
#include "rs232.h"
#include "Config.h"
#include "display.h"
#include "utility.h"
#include "comm.h"
#include "hhuLcd.h"

_SINT fg_PortList[MAX_PORT_COUNT], fg_Error;

struct termios fg_PortConfigNew, fg_PortConfigOld[MAX_PORT_COUNT];

static char fg_PortName[MAX_PORT_COUNT][MAX_PORT_WIDTH]=
	{"/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4",
	"/dev/ttyS5", "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8", "/dev/ttyS9",
	"/dev/ttyS10", "/dev/ttyS11", "/dev/ttyS12", "/dev/ttyS13", "/dev/ttyS14",
	"/dev/ttyS15", "/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB2", "/dev/ttyUSB3",
	"/dev/ttyUSB4", "/dev/ttyUSB5", "/dev/ttymxc0", "/dev/ttymxc1", "/dev/ttymxc2",
	"/dev/ttymxc3", "/dev/ttyACM0"};

static void (*CB_RS232Receive) (_UBYTE a_DataByte) = 0x00;		//callback function on receiving
void CreateRJPortThread(void);
static _UBYTE fg_PortInit;

//======================================================================================================
_UINT RS232_PortOpen(_UINT a_PortNO, _ULONG a_Baudrate)
{
	_ULONG l_Baudrate;

	if (fg_PortInit) return (0);

	if ( (a_PortNO >= MAX_PORT_COUNT ) || (a_PortNO < 0) )
	{
		HAL_DebugPrint(LCD_INFO_LINE, (_SBYTE*) "Illegal COM Port Number");
		return(1);
	}

	switch(a_Baudrate)
	{
		case 50     :   l_Baudrate = B50;   break;
		case 75     :   l_Baudrate = B75;   break;
		case 110    :  l_Baudrate = B110;   break;
		case 134    :  l_Baudrate = B134;   break;
		case 150    :  l_Baudrate = B150;   break;
		case 200    :  l_Baudrate = B200;   break;
		case 300    :  l_Baudrate = B300;   break;
		case 600    :  l_Baudrate = B600;   break;
		case 1200   : l_Baudrate = B1200;   break;
		case 1800   : l_Baudrate = B1800;   break;
		case 2400   : l_Baudrate = B2400;   break;
		case 4800   : l_Baudrate = B4800;   break;
		case 9600   : l_Baudrate = B9600;   break;
		case 19200  :l_Baudrate = B19200;   break;
		case 38400  :l_Baudrate = B38400;   break;
		case 57600  :l_Baudrate = B57600;   break;
		case 115200 :l_Baudrate = B115200;  break;
		case 230400 :l_Baudrate = B230400;  break;
		case 460800 :l_Baudrate = B460800;  break;
		case 500000 :l_Baudrate = B500000;  break;
		case 576000 :l_Baudrate = B576000;  break;
		case 921600 :l_Baudrate = B921600;  break;
		case 1000000:l_Baudrate = B1000000; break;

		default:
			printf("\nInvalid a_Baudrate");
            return(1);
            break;
	}

	fg_PortList [a_PortNO] = open(fg_PortName [a_PortNO], O_RDWR | O_NOCTTY | O_NDELAY);

	if(fg_PortList[a_PortNO] == -1)
	{
		perror("\nPort Access Error");
		while(1);
	}

	fg_Error = tcgetattr(fg_PortList[a_PortNO], fg_PortConfigOld + a_PortNO);

	if(fg_Error == -1)
	{
		close(fg_PortList[a_PortNO]);
		perror("unable to read Port Settings ");
		return(1);
	}

	memset(&fg_PortConfigNew, 0, sizeof(fg_PortConfigNew));  /* clear the new struct */

	fg_PortConfigNew.c_cflag = l_Baudrate | CS8 | CLOCAL | CREAD;
	fg_PortConfigNew.c_iflag = IGNPAR;
	fg_PortConfigNew.c_oflag = 0;
	fg_PortConfigNew.c_lflag = 0;
	fg_PortConfigNew.c_cc[VMIN] = 0;      /* block untill n bytes are received */
	fg_PortConfigNew.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

	fg_Error = tcsetattr(fg_PortList[a_PortNO], TCSANOW, &fg_PortConfigNew);

	if(fg_Error == -1)
	{
		close(fg_PortList[a_PortNO]);
		perror("Unable to adjust Port Settings ");
		return(1);
	}

	fg_PortInit = 0x01;		//Port initialted
	Port_RegCBReceive(&HPL_ReceiveByte);
	CreateRJPortThread();
	return(0);
}

//======================================================================================================
_UINT RS232_PortPollRead (_UINT a_PortNo)
{
	_UINT l_Count;
	_UBYTE l_Byte, l_Return;

	if (fg_PortInit != 0x01)
	{
		return 0;
	}

	l_Return = 0x00;
	l_Count = 0x00;

	do
	{
		l_Return = read(fg_PortList[a_PortNo], &l_Byte, 1);	//Read single byte

		if (l_Return)
		{
			l_Count++;
			CB_RS232Receive (l_Byte);
			fprintf(stderr, " %02X", l_Byte);
		}

	}while(l_Return);

	return (l_Count);	//Total number of byte received
}

//======================================================================================================
_UINT RS232_PortSendByte(int a_PortNo, unsigned char a_DataByte)
{
	int l_Count;

	fprintf(stderr, "\n---->"); 	//outward symbol
	fprintf(stderr,"%02X ", a_DataByte);
	fprintf(stderr," - Sent\n");

	l_Count = write(fg_PortList[a_PortNo], &a_DataByte, 1);

	if(l_Count < 0)
		return(1);
	else
		return(0);
}

//======================================================================================================
_UINT RS232_PortSendString(_UINT a_PortNo, _UBYTE *a_pBuffer, _UINT a_Size)
{
	_UBYTE l_Count;

	fprintf(stderr, "\n---->"); 	//outward symbol

	for(l_Count = 0x00; l_Count < a_Size; l_Count++)
	{
		fprintf(stderr,"%02X ", *(a_pBuffer + l_Count));
	}

	fprintf(stderr," - Sent\n");

	return(write(fg_PortList[a_PortNo], a_pBuffer, a_Size));
}

//======================================================================================================
//Resetting the serial port settings
void RS232_PortClose (_UINT a_PortNo)
{
	fprintf(stderr, "\n****** fg_PortInit = %d *****\n", fg_PortInit);
	if(fg_PortInit == 1)
	{
		CB_RS232Receive = 0x00;
		fg_PortInit = 0x00;

//		close (fg_PortList [a_PortNo]);
//		tcsetattr (fg_PortList[a_PortNo], TCSANOW, fg_PortConfigOld + a_PortNo);
		fprintf(stderr, "\nPort closed\n");
	}
	return;
}

//======================================================================================================
_UINT RS232_PortGetCTSEnableStatus(_UINT a_PortNo)
{
	_UINT l_Status;

	l_Status = ioctl(fg_PortList[a_PortNo], TIOCMGET, &l_Status);

	if(l_Status & TIOCM_CTS)
		return(1);
	else
		return(0);
}

//================================================================
//Register callback for receiving
void Port_RegCBReceive ( void (* a_pFunctionName) (_UBYTE a_Data) )
{
	CB_RS232Receive = a_pFunctionName;
	return;
}
//================================================================

void * RunRJPort(void *threadid)
{
	fprintf(stderr, "\n Uart threads Running");
	while(fg_PortInit)
	{
		RS232_PortPollRead(SERAIL_RJ_PORT_NO);
		if(fg_PortInit == 0x00)
		{
			//fprintf(stderr, "*");
			fprintf(stderr, "\nuartO2 threads exit %d", fg_PortInit);
			//break;
			return 0;
		}
	}
	fprintf(stderr, "\nuartO2 threads exit111 %d", fg_PortInit);
	//pthread_exit(threadid);
}
//==============================================================================
void CreateRJPortThread(void)
{
	pthread_t threads[1];
	int l_rc;
	//int l_Count;

	fprintf(stderr, "\nCreate uartO2 threads");

	l_rc = pthread_create(&threads[0], NULL, RunRJPort, (void *) 0);

	if(l_rc)
	{
		fprintf(stderr, "\n unable to Create Uart threads");
		exit(-1);
	}

	fprintf(stderr, "\n Uart threads successfully created");
	//pthread_exit(NULL);
}
//==============================================================================

