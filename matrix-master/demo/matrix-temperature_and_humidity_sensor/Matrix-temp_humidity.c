#include <stdio.h>
#include <stdlib.h>
#include "libfahw.h"

#define BUF_SIZE            (64)
#define MAX_CFG_BUF  		(512)
#define DRIVER_MODULE       "dht11"
#define MAIN_CFG_FILE "/mnt/mmc1/server.conf"
char CFG_ssl = '[', CFG_ssr = ']';


static char server_ip[32];



int getConfInfo(char *pParent,char *pChild, char *pBuf)
{
	int fd;
	char child[BUF_SIZE];
	char parent[BUF_SIZE];
	char info[BUF_SIZE];
	char strRed[MAX_CFG_BUF];
	char ch;
	
	int ret=0;
	int i=0,j=0,k=0;
	if((fd=open(MAIN_CFG_FILE,O_RDONLY)) <0)
		return -1;

	if((ret=read(fd,&ch,1))<=0)
			return 0;
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
			printf("%s=%s\n",parent,pParent);
			if(strcmp(parent,pParent) !=0)
				continue;
			while(1)
			{
				memset(child,0,BUF_SIZE);
				memset(strRed,0,BUF_SIZE);
				while(read(fd,&ch,1)>0 && ch != '\n')
				{
					strRed[j] = ch;
					j++;
				}
			
				if(j==0)
					break;
				
				sscanf(strRed,"%[^=]=%s",child,pBuf);
				printf("[%s]\n%s=%s\n",parent,child,pBuf);
				if(strcmp(child,pChild)==0)
					return 0;
				j=0;
				
				
			}
				
			i=0;
			j=0;
		}
		
		if((ret=read(fd,&ch,1))<=0)
				return 0;
		

			
	
		
	}
		

}


int main(int argc, char ** argv)
{
    int ret = -1;
    int dhtTemp=0, dhtHdty=0, board;
    char modStr[BUF_SIZE];
	char bufPost[BUF_SIZE];	
	char buf[BUF_SIZE];
	char serip[BUF_SIZE];
    int pin = GPIO_PIN(7);
	strcpy(server_ip,"192.168.1.2");

	getConfInfo("SERVER","server_ip",server_ip);
		

    if ((board = boardInit()) < 0) {
        printf("Fail to init board\n");
        return -1;
    }    
    if (board == BOARD_NANOPC_T2 || board == BOARD_NANOPC_T3)
        pin = GPIO_PIN(15);
	
	while(1){
	    sprintf(modStr, "modprobe %s gpio=%d", DRIVER_MODULE, pintoGPIO(pin));
	    system(modStr);
	    if ((ret = dht11Read(DHT_HUMIDITY, &dhtHdty)) != -1) {
	        printf("The humidity is %d\n", dhtHdty);
	    } else {
	        printf("Faided to get humidity\n");
	    }
	    if ((ret = dht11Read(DHT_TEMP, &dhtTemp)) != -1) {
	        printf("The temperature is %d\n", dhtTemp);
	    } else {
	        printf("Faided to get temperature\n");
	    }
		sprintf(buf,"echo dhHdty:%d  dhTemp:%d  time:%lu \n",dhtHdty,dhtTemp,time(NULL));		
system(buf);
sprintf(bufPost,"wget http://%s/collect.json?did=112\\&pid=1\\&tem=%d\\&hum=%d -O wget.json",server_ip,dhtTemp/100,dhtHdty/100);
system(bufPost);
sleep(5);
	}




	
    return ret;
}
