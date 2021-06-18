#ifndef __COMMONDEFS_H__
#define __COMMONDEFS_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif


#define BUFF_SIZE 160
#define TWO 0x02
#define THREE 0x03
#define NULL_TERM '\0'
#define SUCCESS 0
#define FAILED -1

#define AT91_PIN_PA0 1
#define	AT91_PIN_PA1 2
#define	AT91_PIN_PA2 3
#define	AT91_PIN_PA3 4
#define	AT91_PIN_PA4 5
#define	AT91_PIN_PA5 6
#define	AT91_PIN_PA6 7
#define	AT91_PIN_PA7 8
#define	AT91_PIN_PA8 9
#define	AT91_PIN_PA9 10
#define	AT91_PIN_PA10 11
#define	AT91_PIN_PA11 12
#define	AT91_PIN_PA12 13
#define	AT91_PIN_PA13 14
#define	AT91_PIN_PA14 15
#define	AT91_PIN_PA15 16
#define	AT91_PIN_PA16 17
#define	AT91_PIN_PA17 18
#define	AT91_PIN_PA18 19
#define	AT91_PIN_PA19 20
#define	AT91_PIN_PA20 21
#define	AT91_PIN_PA21 22
#define	AT91_PIN_PA22 23
#define	AT91_PIN_PA23 24
#define	AT91_PIN_PA24 25
#define	AT91_PIN_PA25 26
#define	AT91_PIN_PA26 27
#define	AT91_PIN_PA27 28
#define	AT91_PIN_PA28 29
#define	AT91_PIN_PA29 30
#define	AT91_PIN_PA30 31
#define	AT91_PIN_PA31 32

#define	AT91_PIN_PB0 33
#define	AT91_PIN_PB1 34
#define	AT91_PIN_PB2 35
#define	AT91_PIN_PB3 36
#define	AT91_PIN_PB4 37
#define	AT91_PIN_PB5 38
#define	AT91_PIN_PB6 39
#define	AT91_PIN_PB7 40
#define	AT91_PIN_PB8 41
#define	AT91_PIN_PB9 42
#define	AT91_PIN_PB10 43
#define	AT91_PIN_PB11 44
#define	AT91_PIN_PB12 45
#define	AT91_PIN_PB13 46
#define	AT91_PIN_PB14 47
#define	AT91_PIN_PB15 48
#define	AT91_PIN_PB16 49
#define	AT91_PIN_PB17 50
#define	AT91_PIN_PB18 51
#define	AT91_PIN_PB19 52
#define	AT91_PIN_PB20 53
#define	AT91_PIN_PB21 54
#define	AT91_PIN_PB22 55
#define	AT91_PIN_PB23 56
#define	AT91_PIN_PB24 57
#define	AT91_PIN_PB25 58
#define	AT91_PIN_PB26 59
#define	AT91_PIN_PB27 60
#define	AT91_PIN_PB28 61
#define	AT91_PIN_PB29 62
#define	AT91_PIN_PB30 63
#define	AT91_PIN_PB31 64

#define	AT91_PIN_PC0 65
#define	AT91_PIN_PC1 66
#define	AT91_PIN_PC2 67
#define	AT91_PIN_PC3 68
#define	AT91_PIN_PC4 69
#define	AT91_PIN_PC5 70
#define	AT91_PIN_PC6 71
#define	AT91_PIN_PC7 72
#define	AT91_PIN_PC8 73
#define	AT91_PIN_PC9 74
#define	AT91_PIN_PC10 75
#define	AT91_PIN_PC11 76
#define	AT91_PIN_PC12 77
#define	AT91_PIN_PC13 78
#define	AT91_PIN_PC14 79
#define	AT91_PIN_PC15 80
#define	AT91_PIN_PC16 81
#define	AT91_PIN_PC17 82
#define	AT91_PIN_PC18 83
#define	AT91_PIN_PC19 84
#define	AT91_PIN_PC20 85
#define	AT91_PIN_PC21 86
#define	AT91_PIN_PC22 87
#define	AT91_PIN_PC23 88
#define	AT91_PIN_PC24 89
#define	AT91_PIN_PC25 90
#define	AT91_PIN_PC26 91
#define	AT91_PIN_PC27 92
#define	AT91_PIN_PC28 93
#define	AT91_PIN_PC29 94
#define	AT91_PIN_PC30 95
#define	AT91_PIN_PC31 96


/************************************ keypad return values ********************************************/
#define F1      201
#define F2      202
#define F3      203
#define F4      204
#define F5      205

#define TAB     206
#define BL      207
#define DEL     208

#define ESC     209
#define BKSP    210
#define PF      211

#define UP      216  
#define DOWN    217  
#define RIGHT   218  
#define LEFT    219  

#define SHIFT   220 
#define SECOND  221 

#define CAPS    212
#define ENT      30 



unsigned char kb_hit(void); //This function returns the key hit 

/*****************************************MACHINE ID*******************************************************/

int get_machineid(char *machine_buff);  //This function is used to get the machine id of the device to the buffer passed as argument


/*******************************************eeprom*******************************************************/ 

int eepcheck (const char *eappbuff);    /*This function is used by the app developers to pass input as 
				    plaintext verified againist eeprom LUT return 0 on success and nonzero on Failure.*/

/****************************************** rfid ***********************************************************/

unsigned char mif_request(void);
	/*This function is used to send a request to card*/
unsigned char mif_halt(void);
	/*This function is used to stop the transactions*/
unsigned char mif_auth(unsigned char mif_auth_mode, unsigned char mif_auth_sec);
	/*This function is used for authentication*/
unsigned char mif_select(void);
	/*This function is used to select the card*/
unsigned char mif_anti_col(void);
	/*This function is used to enable the anti colision*/
unsigned char mif_read_card(unsigned char *mif_read_data, unsigned char mif_read_add);

	/* This function is used to read the data from RFID card.Here you have to mention 
          the buffer into which the data has to come as first argument and from which 
           address loction the data has to be read as second argument */


unsigned char mif_write_card(unsigned char *mif_write_data, unsigned char mif_write_add);

	 /* This function is used to write the data to RFID card.Here you have to pass the 
            data to be written as first argument and to which address loction the data has 
            to be written as second argument */



unsigned char mif_load_key(unsigned char mif_lk_mode, unsigned char mif_lk_sec, unsigned char *mif_lk_key);

extern unsigned char mif_ser_no[4];


/***************************************graphical display *************************************************/


int initDisplay(void);			     /*Intialization of Dispaly.This api must be called at 
						the beginning of your application only once	*/

void exitDisplay(void);				/*close Display*/	


int LcdClear(void);                    	      /* This function is used to clear the viewportdisplay and the						    					cursor position will be at the end.So becarefull 
						 after clearing set the cursor where u want by 
						 using goto_xy function.*/ 

int FullLcdClear(void);			      /*clears the entire display including the last row and the 
						cursor position will be set at the end*/	


int LcdPrintf(const char *appbuf1);  		      /* This function is used to display the data at 
						 present cursor position	*/



int LcdPrint_Image(const char *imgbuff,unsigned int Imgcnt); /*This function is used to display the image on screen.pass the image buff as the first 
							argument and buffer count as the second argument*/ 


int goto_xy(int row,int col);		      /* This function is used to set the cursor position
						 at the mentioned coordinates i.e row and col	*/		

int  position_cursor(int cursor);	      /* This function is used at the time of inputting 						   						the data by user i.e if u want to manually 
						change the data if u pass the argument as 
						argument :0 -> backspace,1 -> leftarrow,
						2 -> uparrow,3 -> downarrow,4 ->rightarrow */

int key_LcdPrintf(char buf1);                 /* This function is used to display the data at the 
						time of accepting the input from user through the 
						key pad and to display characters on the display 
						with cursor at the end */

int  checksignal(int sig);		      /* This function will display the signal stren on the  display.*/	


int battery_check(void);		      /*check for low battery returns 2 if low battery otherwise return 0 */
			
			// The above four functions returns 0: ON SUCCESS  
   			// 	         returns 1(or)nonzero: ON FAILURE  



/***************************************       2" Impact Printer              *************************************************/


int  PaperFeed(void);		//This function is used to feed the paper up


/***************************************            keypad          ***********************************************************/


unsigned char _getkey(void);   /*This function will return the character code pressed from the  				    keypad*/

void shift_enable(void);   	//enable shift key
void shift_disable(void);	//disable shift key
int shift_status(void);        //check the status of the shift key

void second_enable(void);	//enable second key
void second_disable(void);	//disable second key
int second_status(void);       //check the status of the second key

void CAPS_enable(void);		//enable CAPS key
void CAPS_disable(void); 	//disable CAPS key
int CAPS_status(void);         //check the status of the CAPS key

/*If you are using scanf for scaning the characters instead of above function*/

void Disable_keypadecho(void); /*This function is called when you dont want to 
				 display the characters pressed by user*/

void Enable_keypadecho(void); /*This function is called when you want to display the 
				characters pressed by user*/

void Mask_keypadecho(void);   /*This function is called when you wnat to mask the characters pressed   by user*/	 

/***************************************             GSM                    *************************************************/


int iotoggle(int PinNum,int Status);    /* This functions is used to toggle the io pin.i.e to make 			        		      					  the gpio pin either high or low first argument is the						      						 pin number and second argument is the HIGH or LOW state					     						of that pin.If status is 0 indicates LOW and 1 indicates					       					       HIGH */

int ioread(int PinNum);			/* This function is used to read the status of the pin 
					   whether the pin is in HIGH state or LOW state */


int gsm_init(void);			// This function is used to power up the GSM 


int gsm_exit(void);			// This function is used to power down the GSM


		// The above four functions return 0 : ON SUCCESS
		// 	      return 1 (or) non-zero : ON FAILURE
		 	      

void sendATcommand(const char *cmd,char *responsebuff); 	//pass the AT command as argument 1 and receive its respone in responsebuff 
								//which is argument 2

char * get_ccid(void);	       		//This function returns the ccid of the sim and writes ccidnumber to below mentioned file name  
					//     "/home/admin/systemutils/modem/ccidnum"

int get_signalstrength(void); 		// This function returns the signal strength

void buzzer(void); 			// This function is used to ON the buzzer
	
void buzzer_on(void);			//This function is used to ON keypad buzzer
void buzzer_off(void);			//This function is used to OFF keypad buzzer
int buzzer_status(void);		//This function tells the status of keypad buzzer whether enabled or not.

#ifdef __cplusplus
}
#endif

#endif   /*____COMMONDEFS_H__*/
