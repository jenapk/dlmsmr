/*
 * kt_misc.cpp
 *
 *  Created on: 03-Jun-2021
 *      Author: pkjena
 */

#include <unistd.h>

void delay(unsigned int a_msDely)
{
	usleep(a_msDely * 1000);
}
