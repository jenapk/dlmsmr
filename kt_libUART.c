
#include <stdint.h>
#include "kt_libUART.h"
#include "kt_config_app.h"

//UART
#define MAX_PORT_COUNT	26		//total number of ports can be handled
#define MAX_PORT_WIDTH	13		//to hold the port name in array

const char fg_PortName[MAX_PORT_COUNT][MAX_PORT_WIDTH]={
	"/dev/ttyS0",	//0
	"/dev/ttyS1",	//1
	"/dev/ttyS2",	//2
	"/dev/ttyS3",	//3
	"/dev/ttyS4",	//4
	"/dev/ttyS5",	//5
	"/dev/ttyS6",	//6
	"/dev/ttyS7",	//7
	"/dev/ttyS8",	//8
	"/dev/ttyS9",	//9
	"/dev/ttyS10",	//10
	"/dev/ttyS11",	//11
	"/dev/ttyS12",	//12
	"/dev/ttyS13",	//13
	"/dev/ttyS14",	//14
	"/dev/ttyS15",	//15
	"/dev/ttyUSB0",	//16
	"/dev/ttyUSB1",	//17
	"/dev/ttyUSB2",	//18
	"/dev/ttyUSB3",	//19
	"/dev/ttyUSB4",	//20
	"/dev/ttyACM0",	//21
	"/dev/ttymxc0",	//22
	"/dev/ttymxc1",	//23
	"/dev/ttymxc2",	//24
	"/dev/ttymxc3"	//25
};

int16_t fg_PortList[MAX_PORT_COUNT];
int16_t fg_Error = 0;

struct termios fg_PortConfigNew, fg_PortConfigOld[MAX_PORT_COUNT];
static void (*CB_RS232Receive[MAX_PORT_COUNT])(uint8_t a_DataByte) = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//======================================================================================================
uint16_t kt_libUART_Open(uint16_t a_PortNO, uint32_t a_Baudrate)
{
	uint32_t l_Baudrate = 0;

	if ( (a_PortNO >= MAX_PORT_COUNT ) || (a_PortNO < 0) )
	{
		printf("Illegal COM Port Number");
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
        printf("ERROR : Port %s, %d\n", fg_PortName [a_PortNO], a_PortNO);
		return(1);  //while(1);
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

	return(0);
}

//======================================================================================================
uint16_t kt_libUART_Poll(uint16_t a_PortNo)
{
	uint16_t l_Count = 0;
	uint8_t l_Byte = 0, l_Return = 0;

	l_Return = 0x00;
	l_Count = 0x00;

	do
	{
		l_Return = read(fg_PortList[a_PortNo], &l_Byte, 1);	//Read single byte

		if (l_Return)
		{
			l_Count++;
			CB_RS232Receive[a_PortNo] (l_Byte);
		}

	}while(l_Return);

	return (l_Count);	//Total number of byte received
}

//======================================================================================================
uint16_t kt_libUART_SendByte(int a_PortNo, unsigned char a_DataByte)
{
	int l_Count;

	l_Count = write(fg_PortList[a_PortNo], &a_DataByte, 1);

	if(l_Count < 0)
		return(1);
	else
		return(0);
}

//======================================================================================================
uint16_t kt_libUART_SendData(uint16_t a_PortNo, uint8_t *a_pData, uint16_t a_Size)
{
	uint8_t l_Count = 0;

	fprintf(stderr, "\n---->"); 	//outward symbol

	for(l_Count = 0x00; l_Count < a_Size; l_Count++)
	{
		fprintf(stderr,"%02X ", *(a_pData + l_Count));
	}

	fprintf(stderr," - Sent\n");

	return(write(fg_PortList[a_PortNo], a_pData, a_Size));
}

//======================================================================================================
//Resetting the serial port settings
void kt_libUART_Close (uint16_t a_PortNo)
{
	CB_RS232Receive[a_PortNo] = 0x00;

	close (fg_PortList [a_PortNo]);
	tcsetattr (fg_PortList[a_PortNo], TCSANOW, fg_PortConfigOld + a_PortNo);

	return;
}

//======================================================================================================
uint16_t kt_libUART_GetCTSEnableStatus(uint16_t a_PortNo)
{
	uint16_t l_Status = 0;

	l_Status = ioctl(fg_PortList[a_PortNo], TIOCMGET, &l_Status);

	if(l_Status & TIOCM_CTS)
		return(1);
	else
		return(0);
}

//================================================================
//Register callback for receiving
void kt_libUART_RegCBReceive (uint16_t a_PortNo, void (* a_pFunctionName) (uint8_t a_Data))
{
	CB_RS232Receive[a_PortNo] = a_pFunctionName;
	return;
}

//================================================================
uint8_t kt_libUART_GetPortNo(char* a_pPort, uint8_t* a_pPortName)
{
	static char fg_portStr[16];
    uint8_t l_retVal = 0xFF;
    uint8_t l_count = 0;
	FILE *fptr;

	memcpy(fg_portStr,"/dev/", 5);

    //------------------------------------------------
	// Reading Port File
	fptr = fopen(a_pPort,"r");

	if(fptr == NULL)
	{
		printf("...Port file opening error [%s]\n", a_pPort);
		return(l_retVal);
	}

    fscanf(fptr, "%[^\n]", &fg_portStr[5]);
    fclose(fptr);

    //-------------------------------------------------------
    // searching for port number
    for(l_count = 0; l_count < MAX_PORT_COUNT; l_count++)
    {
        if(strcmp(fg_portStr, fg_PortName[l_count]) == 0)
        {
            l_retVal = l_count;
            memcpy(a_pPortName, fg_portStr, 16);
            //printf("Matched Port : %s, %d\n", fg_portStr, l_retVal);
            break;
        }
    }

    if(l_count == MAX_PORT_COUNT)
    {
        printf("..No port matching ERROR [%s]\n.", a_pPort);
    }

    return(l_retVal);
}
