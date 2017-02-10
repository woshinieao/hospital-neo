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





int cgi_set_return(int err,char *info)
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
	if(info != NULL)
		json_AddStringToObject(root, "INFO",info);
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
	
	
	return NO_ERR;	
	
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
	char ssid[256];
	char ask[256];
	pssid = get_cgi(wifi_cfg_class[0].item);
	if(pssid==NULL)
		return SET_ERR;

	pask = get_cgi(wifi_cfg_class[1].item);
	if(pask==NULL)
		return SET_ERR;

	FILE*fp;/*定义文件指针fp*/
	if((fp=fopen("/etc/wpa_supplicant/wpa_supplicant.conf","w"))==NULL)
	{
		cgiLog("cannot open wifi config file.\n");
		return SET_ERR;
	}
	
	memset(ssid,0,256);
	memset(ask,0,256);

	cgiLog("ssid:%s  ask:%s \n",pssid,pask);
	sprintf(ssid,"ssid=\"%s\"",pssid);
	sprintf(ask,"\r\npsk=\"%s\"",pask);

	fputs("network={\r\n",fp);
	fputs(ssid,fp);
	fputs(ask,fp);

	fputs("\r\n}\r\n",fp);
	fclose(fp);

//	CFG_set_key(MAIN_CFG_FILE, "SYSTEM", "reboot", "true");

	return NO_ERR;
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
			cgiLog("ret %d ",ret);
				ret += configList[i].cgi();
			}
	if(ret>=0)
		cgi_set_return(NO_ERR,NULL);
	else
		cgi_set_return(SET_ERR,"config error !");
	return ret;

}


int reboot_now()
{
	int ret=0;
	cgiLog("reboot_now ");
		 
			ret=CFG_set_key(MAIN_CFG_FILE, "SYSTEM", "reboot", "true");
	if(ret>0)
		cgi_set_return(NO_ERR,NULL);
	else
		cgi_set_return(SET_ERR,"reboot error !");
	return ret;
		
}

int get_version()
{
	unsigned long version = SERVER_VERSION;
	char buff[256];
	sprintf(buff,"verson:%lu.%lu.%lu.%lu built at: %8s,on %12s",(version&0xFF000000)>>24,(version&0xFF0000)>>16,(version&0xFF00)>>8,(version&0xFF),__TIME__,__DATE__);
	cgi_set_return(NO_ERR,buff);
	return NO_ERR;

}
static tCfgCall actionlist[] = {
	{"config",		set_config_call},
	{"reboot",		reboot_now},
	{"version",		get_version}
};


int main(int argc, char ** argv)
{
    int icaller= sizeof(actionlist)/sizeof(tCfgCall);
	char pContent[2048];
	unsigned long version = SERVER_VERSION; 
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

		if(argc == 2  ) 
		{
			if( (strcmp(argv[1],"-v") == 0) || (strcmp(argv[1],"-V") == 0))
			{
				printf("%lu.%lu.%lu.%lu\n",(version&0xFF00)>>8,(version&0xFF),VERSION_DATE_AS_INT,VERSION_TIME);
				return 0;
			}
		}

		
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
				cgi_set_return(SET_ERR,"post string is null!");
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
				cgi_set_return(SET_ERR,"get string is null!");
				return -1;
			}	
			strcpy(pContent,p);

		}
		init_cgi(pContent,strlen(pContent));
		pAction = get_cgi("action");
		cgiLog("STRING:%s  *** action:%s\n",pContent,pAction);
		if(pAction == NULL)
				cgi_set_return(SET_ERR,"action is null !");
		for( i = 0;i<icaller; i++)
			if(strcmp(actionlist[i].item,pAction) == 0)
				break;
		if(i>=icaller){
			cgi_set_return(SET_ERR,"action match null!");
			return -1;
		}
		if(actionlist[i].cgi != NULL)
			iRet = actionlist[i].cgi();

/*
		if(iRet<0)
			cgi_set_return(SET_ERR,"");
		else if(iRet>0)
			cgi_set_return(NO_ERR);
*/		
		cgiLog("cgi over -------------\n");
		fflush(stdout);
		fflush(stdin);
	
		destory_cgi();
	}//end while(FCGI_Accept() >= 0)
	
	
	return NO_ERR;
}




