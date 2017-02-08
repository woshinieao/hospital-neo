#ifndef CGI_H_
#define CGI_H_

#include "util.h"


#define CGI_LOG
#define NO_ERR	(0)
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



#define SERVER_VERSION 0x01010101


#endif



