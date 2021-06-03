/*
 * kt_misc_func.c
 *
 *  Created on: Nov 3, 2020
 *      Author: pkjena
 */


/* includes */
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

//==================================================================================
void delay_ms(uint16_t msec)
{
    struct timespec ts = {.tv_sec = msec / 1000, .tv_nsec = (msec % 1000) * 1000000};
    nanosleep(&ts, NULL);
}

/*============================================================================*/
// calculating xor byte
uint8_t kt_miscGetXOR (uint8_t* a_pBuffer, uint8_t a_Len)
{
    uint8_t l_Counter, l_Temp = 0;

    for(l_Counter = 0; l_Counter < a_Len; l_Counter++)
    {
        l_Temp ^= *a_pBuffer++;
    }

    return(l_Temp);
}

/*============================================================================*/
void delay(unsigned int a_msDely)
{
	usleep(a_msDely * 1000);
}
