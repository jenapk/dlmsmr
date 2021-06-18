//APP_Config.H

//================================================================
#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H
//================================================================
#include <stdio.h>
#include "version.h"

//Target types
//#define LINUX_PC						0x01
#define LINUX_ATMEL						0x02

#define MAKE_LOG_FILE					0x01	//mundel //1 make log //0 No log

#ifdef LINUX_PC
	#include "pdsequivalent.h"
	#define APP_FILE_DLMS_AUTH 			"/home/mundel/admin/conf/AUTH12.txt"
	#define RAW_FILE_NAME				"/home/mundel/admin/conf/EXAMPLE.ddt"
	#define FILE_PROG_TXT				"/home/mundel/admin/conf/PROG.txt"
	#define FILE_TOD_TXT				"/home/mundel/admin/conf/TOD.txt"
	#define FILE_DATE_TXT				"/home/mundel/admin/conf/DATE.txt"


	#define APP_FILE_PATH					"/home/mundel/admin/conf/"
	#define APP_DATABASE_PATH				"/home/mundel/admin/conf/ir.db"		//database path with name
	#define APP_DATA_TXT_FILE				"/home/mundel/admin/conf/padma.txt"	//temporary text datafile
	#define FILE_AUTH_TXT					"/home/mundel/admin/conf/auth.txt"
	#define FILE_AUTH1_TXT					"/home/mundel/admin/conf/auth1.txt"	//for log file
	#define FILE_MSTR_MRI					"/home/mundel/admin/conf/MSTR.MRI"
	#define FILE_MAC_TXT					"/home/mundel/admin/conf/mac.txt"
	#define FILE_SERIAL_TXT					"/home/mundel/admin/conf/Serial.txt"
	#define FILE_EXAMPLE7_TST				"/home/mundel/admin/conf/example7.tst"
	#define FILE_EXAMPLE1_TST				"/home/mundel/admin/conf/example1.tst"
	#define FILE_EXAMPLE2_TST				"/home/mundel/admin/conf/example2.tst"
	#define FILE_EXAMPLE3_TST				"/home/mundel/admin/conf/example3.tst"
	#define FILE_EXAMPLE4_TST				"/home/mundel/admin/conf/example4.tst"
	#define FILE_BILL_MRI					"/home/mundel/admin/conf/BILL.MRI"
	#define FILE_TEMP_DAT					"/home/mundel/admin/conf/temp.dat"
	#define FILE_LSD_MRI					"/home/mundel/admin/conf/LSD.MRI"
	#define FILE_TEMP3_DAT					"/home/mundel/admin/conf/temp3.dat"
	#define FILE_TEMP2_DAT					"/home/mundel/admin/conf/temp2.dat"
	#define FILE_TEMP1_DAT					"/home/mundel/admin/conf/temp1.dat"
	#define FILE_KWH_MRI					"/home/mundel/admin/conf/KWH.MRI"
	#define FILE_TPR_MRI					"/home/mundel/admin/conf/TPR.MRI"
	#define FILE_LS_MRI						"/home/mundel/admin/conf/LS.MRI"
	#define FILE_TAMPER_MRI					"/home/mundel/admin/conf/TEMPER.MRI"
	#define FILE_STAT_TXT					"/home/mundel/admin/conf/stat.txt"
	#define FILE_IRLOG_TXT					"/home/mundel/admin/conf/IRlog.txt"

	#define SERAIL_RF_PORT_NO				16	//for PC COM1  26(ttyACM0) 16(ttyUSB0)
	#define SERAIL_RJ_PORT_NO				26	//in PC both RF and BCS ports are same

#elif LINUX_ATMEL
	#include "commonDefs.h"
	#define APP_FILE_DLMS_AUTH 			"/home/admin/conf/AUTH12.txt"
	#define RAW_FILE_NAME				"/home/admin/conf/EXAMPLE.ddt"
	#define FILE_PROG_TXT				"/home/admin/conf/PROG.txt"
	#define FILE_TOD_TXT				"/home/admin/conf/TOD.txt"
	#define FILE_DATE_TXT				"/home/admin/conf/DATE.txt"


	#define APP_FILE_PATH					"/home/admin/data/"
	#define APP_DATABASE_PATH				"/home/admin/conf/ir.db"				//database path with name
	#define FILE_AUTH_TXT					"/home/admin/data/auth.txt"
	#define FILE_AUTH1_TXT					"/home/admin/data/auth1.txt"		//for log file
	#define FILE_MSTR_MRI					"/home/admin/data/MSTR.MRI"
	#define FILE_MAC_TXT					"/home/admin/data/mac.txt"
	#define FILE_SERIAL_TXT					"/home/admin/data/Serial.txt"
	#define FILE_EXAMPLE7_TST				"/home/admin/data/example7.tst"
	#define FILE_EXAMPLE1_TST				"/home/admin/data/example1.tst"
	#define FILE_EXAMPLE2_TST				"/home/admin/data/example2.tst"
	#define FILE_EXAMPLE3_TST				"/home/admin/data/example3.tst"
	#define FILE_EXAMPLE4_TST				"/home/admin/data/example4.tst"
	#define FILE_BILL_MRI					"/home/admin/data/BILL.MRI"
	#define FILE_TEMP_DAT					"/home/admin/data/temp.dat"
	#define FILE_LSD_MRI					"/home/admin/data/LSD.MRI"
	#define FILE_TEMP3_DAT					"/home/admin/data/temp3.dat"
	#define FILE_TEMP2_DAT					"/home/admin/data/temp2.dat"
	#define FILE_TEMP1_DAT					"/home/admin/data/temp1.dat"
	#define FILE_KWH_MRI					"/home/admin/data/KWH.MRI"
	#define FILE_TPR_MRI					"/home/admin/data/TPR.MRI"
	#define FILE_LS_MRI						"/home/admin/data/LS.MRI"
	#define FILE_TAMPER_MRI					"/home/admin/data/TEMPER.MRI"
	#define FILE_STAT_TXT					"/home/admin/data/stat.txt"
	#define FILE_IRLOG_TXT					"/home/admin/data/IRlog.txt"

	#define SERAIL_RJ_PORT_NO				2	//ttyS2, In PDS, RF and BCS ports are diffarent
	#define SERAIL_RF_PORT_NO				3	//ttyS3, applicable for PDS
#elif LINUX_FREESCALE
	#include <QtGui/QApplication>
	#include "PCE_KBDSM2.h"
	#include "PCE_GLCD.h"
	#include "rf_io.h"			//to control Reset Pin of RF Module

	#define APP_DATABASE_PATH				"/home/admin/conf/ir.db"				//database path with name
	#define APP_DATA_TXT_FILE				"/home/admin/conf/padma.txt"			//temporary text datafile
	#define APP_DATA_LOG_FILE				"/home/admin/conf/log.txt"				//for log file

	#define SERAIL_RF_PORT_NO				24	//ttymxc2
	#define SERAIL_BCS_PORT_NO				25	//ttymxc3
#endif

#define FILE_DATABASE_NAME					"padma.db"								//only database name

//Baud rate Selection	pdsequivalent.h
#define COM_PORT			1
#define BAUD_RATE			9600

//File Buffer
#define FILE_BUFFER_LEN		8192

#define BUFFER_SIZE			1024

#define LCD_INFO_LINE		7

//================================================================
#endif	//_APP_CONFIG_H
//================================================================
