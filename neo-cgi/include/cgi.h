#ifndef CGI_H_
#define CGI_H_

#include "util.h"


#define CGI_LOG
#define NO_ERR	(1)
#define SET_ERR		(-1)
#define GET_METHOD   (0)
#define PSOT_METHOD   (1)
#define MAX_SERIAL_NUM_LEN (256)

#define MAX_DEVID_LEN	(128)

#define MAIN_CFG_FILE "/mnt/mmc1/server.conf"
#define NET_CONFIG_FILE "/etc/network/interfaces-bak"


#define SER_IP_DEFAULT "192.168.1.2"
#define PORT_DEFAULT (80)
#define DEV_IP_DEFAULT "192.168.1.141"
#define DEV_GATEWAY_DEFAULT "192.168.1.1"
#define DEV_NETMASK_DEFAULT "255.255.255.0"



#define TIME_JIFF_DEFAULT (5)



typedef int (*CgiProc)();

typedef struct _config_cb
{
	char item[32];
	CgiProc cgi;

}tCfgCall;

typedef struct _class_member
{
	char item[32];

}tClassMember;

#define VERSION_YEAR ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10  \
	+ (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))  
#define VERSION_MONTH (__DATE__ [2] == 'n' ? 1 :\
	__DATE__ [2] == 'b' ? 2 :\
	__DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) :\
	__DATE__ [2] == 'y' ? 5 :\
	__DATE__ [2] == 'n' ? 6 :\
	__DATE__ [2] == 'l' ? 7 :\
	__DATE__ [2] == 'g' ? 8 :\
	__DATE__ [2] == 'p' ? 9 :\
	__DATE__ [2] == 't' ? 10 :\
	__DATE__ [2] == 'v' ? 11 : 12)  
#define VERSION_DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
    + (__DATE__ [5] - '0'))  
#define VERSION_DATE_AS_INT (((VERSION_YEAR - 2017) * 12 + VERSION_MONTH-1) * 31 + VERSION_DAY) 

#define VERSION_TIME 	((__TIME__[0]-48)*100000 \
						+(__TIME__[1]-48)*10000\
						+(__TIME__[3]-48)*1000\
						+(__TIME__[4]-48)*100\
						+(__TIME__[6]-48)*10\
						+(__TIME__[7]-48))
/* 根据VERSION_DATE_AS_INT 计算年月日
YEAR = (VERSION_DATE_AS_INT/(31*12)+2017)
MONTH = (VERSION_DATE_AS_INT/31 - (YEAR*12))
DAY = (VERSION_DATE_AS_INT-((VERSION_YEAR - 2017) * 12 + VERSION_MONTH-1) * 31)

*/


#define SERVER_VERSION 0x0102
/******************************************************************************
* File Name     : cgi.h
* Module Name   : cgi
* Description   : 配置模块
* Author/Date   : 聂鳌/ 2017.2.8
* -----------------------------------History-----------------------------------
* Modifier/Date : 2017.2.8
* Modify Reason : 稳定发现第一版
* Version:        1.1.2.1
* -----------------------------------------------------------------------------


******************************************************************************/

#endif



