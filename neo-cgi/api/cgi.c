#include <stdlib.h>
#include <string.h>
#include "fcgi_stdio.h"
#include "cgi.h"
#include "util.h"
#include "json.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "net_util.c"


static char ser_ip[32]  ;
static int	ser_port ;
static char dev_ip[32]  ;
static int dev_port ;
static int time_jiffies;

static tClassMember ser_cfg_class[]= {
	{"server_ip"},
	{"server_port"},
};
static tClassMember dev_cfg_class[] = {
	{"device_ip"},
	{"device_port"},
	{"device_netmask"},
	{"device_gateway"},
};

static tClassMember time_cfg_class[] = {
	{"interval"},
	
};

static tClassMember wifi_cfg_class[] = {
	{"wifi"},
	{"ask"},
	
};

static tClassMember ip_interfacesEth[]= {
	{"auto eth0"},
	{"iface eth0 inet"},
	{"address"},
	{"netmask"},
	{"gateway"},
};

static tClassMember ip_interfacesWlan[]= {
	{"auto wlan0"},
	{"iface wlan0 inet"},
	{"address"},
	{"netmask"},
	{"gateway"},
};





int cgi_set_return(int err)
{
	char *out;
	json *root = NULL;
	json *stat ,*jcpu,*jmem,*jtime,*jdisk;
	char str[64]= {0};
	
	time_t timer;

	time_t tNow = time(NULL);
	sprintf(str,"%ld",tNow);


	root = json_CreateObject();
	
	if(err == NO_ERR)
		json_AddStringToObject(root, "flag","OK");
	else
		json_AddStringToObject(root, "flag","ERROR");
	
	json_AddStringToObject(root, "IP",dev_ip);
	json_AddStringToObject(root, "time",str);
	out = json_Print(root);	
	printf("%s\n",out);	
	free(out);
	json_Delete(root);
	return err;

}



int check_cfg()
{
	char buf[256]={0};
	if(access(MAIN_CFG_FILE,F_OK) !=0){
		FILE *fd;
		fd = fopen(MAIN_CFG_FILE,"a+");
		if(fd == NULL )
		{
			cgiLog(" no config file and create  failed !   ");
			
		}
	
	fclose(fd);
	char buf_port[16];
	char buf_time[16];
	sprintf(buf_port,"%d",PORT_DEFAULT);
	sprintf(buf_time,"%d",TIME_JIFF_DEFAULT);

	CFG_set_key(MAIN_CFG_FILE, "SERVER", "server_port", buf_port);
	CFG_set_key(MAIN_CFG_FILE, "SERVER", "server_ip", SER_IP_DEFAULT);
	
	CFG_set_key(MAIN_CFG_FILE, "DEV", "port", buf_port);
	CFG_set_key(MAIN_CFG_FILE, "DEV", "address", DEV_IP_DEFAULT);

	CFG_set_key(MAIN_CFG_FILE, "DEV", "gateway", DEV_GATEWAY_DEFAULT);
	CFG_set_key(MAIN_CFG_FILE, "DEV", "netmask", DEV_NETMASK_DEFAULT);
	
	CFG_set_key(MAIN_CFG_FILE, "TIME", "interval", buf_time);
	CFG_set_key(MAIN_CFG_FILE, "SYSTEM", "reboot", "false");

		
	}

	
	if( CFG_get_key(MAIN_CFG_FILE, "SERVER", "server_ip", buf ) == CFG_OK)
		strcpy(ser_ip,buf);
	else
		strcpy(ser_ip,SER_IP_DEFAULT);

	if( CFG_get_key(MAIN_CFG_FILE, "SERVER", "server_port", buf ) == CFG_OK)
			ser_port= atoi(buf);
	else
		ser_port = PORT_DEFAULT;
	if( CFG_get_key(MAIN_CFG_FILE, "DEV", "address", buf ) == CFG_OK)
		strcpy(dev_ip,buf);
	else
		strcpy(dev_ip,DEV_IP_DEFAULT);

	if( CFG_get_key(MAIN_CFG_FILE, "DEV", "port", buf ) == CFG_OK)
			dev_port= atoi(buf);
	else
		dev_port = PORT_DEFAULT;

	if( CFG_get_key(MAIN_CFG_FILE, "TIME", "interval", buf ) == CFG_OK)
		time_jiffies= atoi(buf);
	else
		time_jiffies = 5;
	
	
	return 0;	
	
}


int fix_net_config(char *ip ,char *mask, char *gateway)
{
	int offet_r = 0;
	char buf[1024];
	char *p = NULL;
	char *pDst[3] = {NULL};
	int i=0;
	if(ip ==NULL && mask== NULL && gateway==NULL)
		return SET_ERR;
	pDst[0] = ip,pDst[1] = mask,pDst[2]=gateway;
	int iTemNum= sizeof(ip_interfacesWlan)/sizeof(tClassMember);
	FILE *fd = fopen(NET_CONFIG_FILE,"a");
	memset(buf,0,1024);

	while(fgets(buf,1024,fd) !=EOF)
	{
		if(strcmp(ip_interfacesEth[0].item,buf) != 0)
			continue;
			memset(buf,0,1024);
			fgets(buf,1024,fd);			
			if((strcmp(buf,ip_interfacesEth[1].item)) ==NULL)
				return SET_ERR;
			for( i=0;i<3;i++)
			{
				memset(buf,0,1024);
				fgets(buf,1024,fd);				
				if((p=strstr(buf,ip_interfacesEth[i+2].item)) ==NULL)
					return SET_ERR;
				sprintf(buf+sizeof(ip_interfacesEth[i+2].item)," %s",pDst[i]);
				offet_r =lseek(fd,(0-strlen(buf)),SEEK_CUR);
				fputs(buf,fd);
			}
			break;
	}
	fclose(fd);

}


int set_server_ip()
{
	char *p = NULL;
	int ret;
	p = get_cgi(ser_cfg_class[0].item);

	if(p==NULL)
		return SET_ERR;		
	strncpy(ser_ip ,p,strlen(p));
	cgiLog("set_server_ip :%s\n",ser_ip);
	ret =CFG_set_key(MAIN_CFG_FILE, "SERVER", "server_ip", ser_ip);
	return ret;
}

int set_server_port()
{
	
	char *p = NULL;
	p = get_cgi(ser_cfg_class[1].item);
	if(p==NULL)
		return SET_ERR;

	ser_port = atoi(p);
	cgiLog("set_server_port :%d\n",ser_port);
	return CFG_set_key(MAIN_CFG_FILE, "SERVER", "server_port", p);
}


int set_dev_ip()
{
	char *p = NULL;
	int iRet = 0;
	char buf[128];
	p = get_cgi(dev_cfg_class[0].item);
	if(p==NULL)
		return SET_ERR;
	strncpy(dev_ip ,p,strlen(p));
	cgiLog("set_dev_ip :%s\n",dev_ip);
	return CFG_set_key(MAIN_CFG_FILE, "DEV", "address", dev_ip);
		
}

int set_dev_port()
{

	char *p = NULL;
	p = get_cgi(dev_cfg_class[1].item);
	if(p==NULL)
		return SET_ERR;

	dev_port = atoi(p);
	
	cgiLog("set_dev_port :%d\n",dev_port);
	return CFG_set_key(MAIN_CFG_FILE, "DEV", "port", p);
}

int set_netmask()
{

	char *p = NULL;
	p = get_cgi(dev_cfg_class[2].item);
	if(p==NULL)
		return SET_ERR;

	
	cgiLog("set_netmask :%s\n",p);
	return CFG_set_key(MAIN_CFG_FILE, "DEV", "netmask", p);
}


int set_gateway()
{

	char *p = NULL;
	p = get_cgi(dev_cfg_class[3].item);
	if(p==NULL)
		return SET_ERR;
	cgiLog("set_gatway :%s\n",p);
	return CFG_set_key(MAIN_CFG_FILE, "DEV", "gateway", p);
}





int set_time_config()
{

	
	char *p = NULL;
	p = get_cgi(time_cfg_class[0].item);
	if(p==NULL)
		return SET_ERR;
		time_jiffies= atoi(p);
	cgiLog("set_time_ns :%d\n",time_jiffies);
	return CFG_set_key(MAIN_CFG_FILE, "TIME", "interval", p);

	
}

int set_wifi_config()
{
	cgiLog("set_wifi_config : ......\n");
	char *pssid = NULL;
	char *pask = NULL;
	pssid = get_cgi(wifi_cfg_class[0].item);
	if(pssid==NULL)
		return SET_ERR;

	pask = get_cgi(wifi_cfg_class[1].item);
	if(pask==NULL)
		return SET_ERR;

	FILE*fp;　/*定义文件指针fp*/
	if((fp=fopen("/etc/wpa_supplicant/wpa_supplicant.conf","w"))==NULL)/*打开文件写模式*/
	{
		cgiLog("cannot open wifi config file.\n");/*判断文件是否正常打开*/
		return SET_ERR;
	}
	fputs(str,fp);/*将字符串写入文件*/
	fclose(fp);/*关闭文件*/

}


static tCfgCall configList[] =
{
	{"server_ip",	set_server_ip},
	{"device_ip",	set_dev_ip},
	{"server_port",	set_server_port},
	{"device_port",	set_dev_port},
	{"device_netmask",set_netmask},
	{"device_gateway",set_gateway},
	{"interval",	set_time_config},
	{"wifi",		set_wifi_config},

};



int set_config_call()
{

	
	int ret = 0;
	int i=0;
	int imber= sizeof(configList)/sizeof(tCfgCall);
		for( i = 0;i<imber;i++)
			if(get_cgi(configList[i].item) !=NULL)
			{
				ret += configList[i].cgi();
			}
			
	return ret;

}


int reboot_now()
{
	cgiLog("reboot_now ");
		return CFG_set_key(MAIN_CFG_FILE, "SYSTEM", "reboot", "true");
		
}

static tCfgCall actionlist[] = {
	{"config",		set_config_call},
	{"reboot",		reboot_now},	
};


int main(void)
{
    int icaller= sizeof(actionlist)/sizeof(tCfgCall);
	char pContent[2048];
	
	cgiLog("cgi start -------------\n");
	
    while(FCGI_Accept() >= 0){
    	char *method 	= NULL;
		char* pJsonId = NULL;
		char* pSid    = NULL;
		char *add = NULL;
		
		int strlenth = 0;
		int iRet = 0;
		int i = 0;
		char *pAction = NULL;
		char *pCfg = NULL;
		printf("Content-type:  text/html;charset=GBK\r\n""\r\n");
		memset(pContent,0,sizeof(pContent));
		
		check_cfg();
		add = getenv("REMOTE_ADDR");
		char *autor = NULL;
		autor = getenv("USER_AGENT");
	

		cgiLog("USER :%s\n",add);
		method = getenv("REQUEST_METHOD");
		if(strcmp(method,"POST") == 0){
			
			char *contentLength = NULL;
			char ch=0;
			contentLength =getenv("CONTENT_LENGTH");
			strlenth = strtol(contentLength, NULL, 10) ;
			if (contentLength == NULL || strlenth == 0) {
				cgi_set_return(SET_ERR);
				return -1;
			}
			for (i = 0; i < strlenth; i++) {
				if ((ch = getchar()) < 0) 
				{
					return -1;
				}
				pContent[i] = ch;
			}
			pContent[i] = 0;
		}
		else{
			char *p = getenv("QUERY_STRING");
			if(p == NULL)
			{
				cgi_set_return(SET_ERR);
				
				return -1;
			}	
			strcpy(pContent,p);

		}
		init_cgi(pContent,strlen(pContent));
		pAction = get_cgi("action");
		cgiLog("STRING:%s  *** action:%s\n",pContent,pAction);
		if(pAction == NULL)
				cgi_set_return(SET_ERR);
		for( i = 0;i<icaller; i++)
			if(strcmp(actionlist[i].item,pAction) == 0)
				break;
		if(i>=icaller){
			cgi_set_return(SET_ERR);
			return -1;
		}
		if(actionlist[i].cgi != NULL)
			iRet = actionlist[i].cgi();

		if(iRet<0)
			cgi_set_return(SET_ERR);
		else
			cgi_set_return(NO_ERR);
		cgiLog("cgi over -------------\n");
		fflush(stdout);
		fflush(stdin);
	
		destory_cgi();
	}//end while(FCGI_Accept() >= 0)
	
	
	return 0;
}




