#include <stdio.h>
#include <stdlib.h>
#include <linux/watchdog.h>
#include <pthread.h>
#include "libfahw.h"
#include <dirent.h>
#include <sys/types.h>
#include "version.h"

char CFG_ssl = '[', CFG_ssr = ']';
static char log_file[256] = "/var/log/matrix.log";
pthread_mutex_t ip_mutex;

char timeInterval[8];
char server_port[8];
static char server_ip[32];
static char dev_ip[32];
static int interval=5;
static int port=80;
int ip1,ip2,ip3,ip4;



int ServerLog(const char *format ,... )
{
	static FILE *fpLog = NULL;

	int result = 0;
	struct stat buf;

	if(fpLog == NULL)
	{
		result = stat( log_file, &buf );
		if(result != 0)  //²»´æÔÚ
    	{
    		fpLog = fopen(log_file,"w");
    	}
		else 
		{
			if( buf.st_size > (1*1024*1024))
				fpLog = fopen(log_file,"w");
			else
				fpLog = fopen(log_file,"a");	
	}
	}
	if(fpLog == NULL)
		return -1;	
	va_list args;
 
    char szDebugBuf[2048] = {0};
    int iPos = 0 ;
    int iLen;
	time_t timer;
    char asctimeBuf[1024];

	(void)time(&timer);
    ctime_r(&timer, asctimeBuf);
	
	va_start(args, format);
	iLen = vsprintf (szDebugBuf + iPos, format, args);
	if(iLen<0)
	{
        va_end (args);
		return -1;
	}
	va_end (args);

	iPos += iLen;
	iLen = sprintf(szDebugBuf+iPos, "%s\n",asctimeBuf);
	iPos += iLen;
	szDebugBuf[iPos] = '\0'	;

	if (fpLog != NULL)
	{
		fputs(szDebugBuf, fpLog);
		fflush(fpLog);
		//fclose(fpLog);
	}

    return (iLen + iPos);	
}

pid_t GetpidByName(char *name)
{
    pid_t           pid = -1;
    DIR *           dirHandle;  // handle of the directory.
    struct dirent * dirEntry;   // single directory item.
    int             fd;
    if ((dirHandle = opendir("/proc")) == NULL)
    {
        return -1;
    }
    chdir("/proc");
    while ((dirEntry = readdir(dirHandle)) != NULL)
    {
        if (dirEntry->d_name[0] != '.')
        {
            if ((fd = open(dirEntry->d_name, O_RDONLY)) != -1)
            {
                FILE *stream;
                char fname[128];
                
                sprintf(fname, "%s/cmdline", dirEntry->d_name);
                stream = fopen(fname, "r");
                if (stream)
                {
                    fgets(fname, 128, stream);
                    if (strstr(fname, name))
                    {
                        // get the pid we want.
                        pid = atoi(dirEntry->d_name);
                        if (pid > 0)
                        {
                            fclose(stream);
                            close(fd);
                            break;
                        }
                    }
                    fclose(stream);
                }
                
                close(fd);
            }
        }
    }  // end of while.
    closedir(dirHandle);

    return pid;
}

int GetExeName(char *pName)
{
    int iResult = 0;
    char pPath[256];

    memset(pPath,0,sizeof(pPath));
    iResult = readlink( "/proc/self/exe",pPath,256); 
    if ( iResult < 0 || iResult >= 256 ) 
        return FALSE;
    while(pPath[iResult] != '/' && iResult>= 0)
    {
        iResult--;
    }
    strcpy(pName,pPath+iResult+1);
    return TRUE;    
}

int IsAlreadyRunning()
{
    pid_t  pid;
    char   prog_name[256];
	char   curDir[256];
	int   bRet = FALSE;

	getcwd(curDir, 256);
    memset(prog_name,0,sizeof(prog_name));
	do
	{
		if(GetExeName(prog_name) == FALSE)
			break;
		pid = GetpidByName(prog_name);
		if(pid<=0 || pid == (pid_t)getpid())
			break;
		bRet = TRUE;
	}while(0);
    chdir(curDir);
    return bRet;
}

int getConfInfo(char *pParent,char *pChild, char *pBuf)
{
	int fd;
	char child[BUF_SIZE];
	char parent[BUF_SIZE];
	char strRed[MAX_CFG_BUF];
	char ch;
	
	int ret=0;
	int i=0,j=0;
	if((fd=open(MAIN_CFG_FILE,O_RDONLY)) <0)
		return -1;
	if((ret=read(fd,&ch,1))<=0)
	{
			close(fd);
			return 0;
	}
	while(1)
	{
		// ÂÔÈ¥×¢ÊÍ
		if(ch == '#' )
		{
			while(read(fd,&ch,1)>0)
			{
				if(ch == '\n')
					break;
			}
			i=0;
			j=0;
		}
		memset(parent,0,BUF_SIZE);
		if(ch == CFG_ssl)
		{
			while(read(fd,&ch,1)>0 && ch != '\n' )
			{
				if(ch !=CFG_ssr )
				{
					parent[i] =ch;
					i++;
				}
			}
	//		printf("%s=%s \n",parent,pParent);
			if(strcmp(parent,pParent) !=0)
				continue;
			while(1)
			{
				memset(child,0,BUF_SIZE);
				memset(strRed,0,BUF_SIZE);
				while(read(fd,&ch,1)>0 && ch != '\n' )
				{
					strRed[j] = ch;
					j++;
				}
				if(ch == CFG_ssl)
				{
					close(fd);
					return -1;
				}
				if(j==0)
					break;
				sscanf(strRed,"%[^=]=%s",child,pBuf);
				
				if(strcmp(child,pChild)==0)
				{
					close(fd);
					return 0;
				}
				j=0;		
			}
			i=0;j=0;
		}

		i=0;
		if((ret=read(fd,&ch,1))<=0)
		{
			close(fd);
			return 0;
		}
	}
}



void *check_state(void *para)
{

	para=NULL;
//	int fd = open("/dev/watchdog",O_RDWR);
//	if(fd<0)
//		return NULL;
	while(1)
	{
	//	write(fd,"feed",4);
		getConfInfo("SERVER","server_ip",server_ip);
		getConfInfo("DEV","address",dev_ip);
		sscanf(dev_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
		getConfInfo("TIME","interval",timeInterval);
		interval = atoi(timeInterval);
		getConfInfo("SERVER","server_port",server_port);
		port = atoi(server_port);
		sleep(3);
	}
	//close(fd);

}


int main(int argc, char ** argv)
{
    int ret = -1;
	int id=0;
    int dhtTemp=0, dhtHdty=0;
	char bufPost[512];
	pthread_t pid;
	unsigned long version = SERVER_VERSION;    
    char dht11FileTem[FILE_PATH_LENGTH];
	char dht11FileHum[FILE_PATH_LENGTH];

	if(argc == 2  ) 
	{
		if( (strcmp(argv[1],"-v") == 0) || (strcmp(argv[1],"-V") == 0))
		{
			printf("%lu.%lu.%d.%d\n",(version&0xFF00)>>8,(version&0xFF),VERSION_DATE_AS_INT,VERSION_TIME);
			return 0;
		}
	}
	if(IsAlreadyRunning())
	{
		ServerLog("IrServer is Already Running,exit\n");
		return 0;
	}
	strcpy(server_ip,"192.168.1.2");
	if( pthread_mutex_init(&ip_mutex, NULL) != 0 )
    	return -1;
	if(pthread_create(&pid, NULL, check_state,NULL) == 0)
	{
		pthread_detach(pid);
	}	
    sprintf(dht11FileHum, "%s/humidity1_input", SHT11_SYS_PATH);
    sprintf(dht11FileTem, "%s/temp1_input", SHT11_SYS_PATH);
	if ((access(dht11FileTem, F_OK) == -1) && access(dht11FileTem, F_OK) == -1) {
		ServerLog("file %s or %s no exist! ", dht11FileHum,dht11FileTem);
		return -1;
	}

	char buffTemp[255];
	char buffHum[255];
	int len = sizeof(buffHum)-1;
	FILE *fpTem = fopen(dht11FileTem,"r");
	if (fpTem == NULL) {
		ServerLog("Unable to open file %s",dht11FileTem);
		return -1;
	} 
	FILE *fpHum = fopen(dht11FileHum,"r");
	if (fpHum == NULL) {
		ServerLog("Unable to open file %s",dht11FileHum);
		return -1;
	}
	ServerLog("dht11FileHum: %s  dht11FileTem:%s ",dht11FileHum,dht11FileTem);

	while(1){
		if(id >100000)
			id = 0;
		id++;
		memset(buffTemp, 0, sizeof(buffTemp));
  		memset(buffHum, 0, sizeof(buffHum));	
		if (fread(buffTemp, sizeof(char), len, fpTem)>0) 
			dhtTemp = atoi(buffTemp)/100;
		else
			ServerLog("Faided to get temperature ");
		if (fread(buffHum, sizeof(char), len, fpHum)>0) 
			dhtHdty = atoi(buffHum)/100;
		else
			ServerLog("Faided to get humidity ");

			ServerLog("sdhtTemp:%d dhHdty:%d  ",dhtTemp,dhtHdty);
		fseek(fpTem,0,SEEK_SET);
		fseek(fpHum,0,SEEK_SET);
		sleep(1);	
		pthread_mutex_lock(&ip_mutex);	
		sprintf(bufPost,"wget http://%s:%d/collect.json?did=%03d%03d%03d%03d\\&pid=%d\\&tem=%d\\&hum=%d -O wget.json",server_ip,port,ip1,ip2,ip3,ip4,id,dhtTemp,dhtHdty);
		pthread_mutex_unlock(&ip_mutex);
		printf("%s\n",bufPost);
		system(bufPost);
		sleep(interval);
	}
	fclose(fpHum);
	fclose(fpTem);	
    return ret;
}























