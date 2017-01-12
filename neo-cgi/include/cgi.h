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


#define NET_EHT0 "eth0"
#define NET_EHT1 "eth1"



#define SER_IP_DEFAULT "192.168.1.2"
#define PORT_DEFAULT (80)
#define DEV_IP_DEFAULT "192.168.1.141"

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


#endif



