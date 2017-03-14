#include "fcgi_config.h"

#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>  
#include <sys/stat.h>

#include <time.h>
#include "fcgi_stdio.h"

#include "util.h"

#define __USE_GNU
#include <search.h>

/* CGI hash table */
static struct hsearch_data htab;
static int htab_count;

int CFG_section_line_no;
int CFG_key_line_no ;
int CFG_key_lines;


char CFG_ssl = '[', CFG_ssr = ']';    /* .CFG/.INI file section symbol */ 



int isspace(int c)
{	
	return c == ' ' || (unsigned)(c-'\t') < 5;
}



char *ctime1(const time_t *t, char *buf)
{	
	struct tm tm;	
	localtime_r(t, &tm);	
	return asctime_r(&tm, buf);

}

long long atoll (const char *str)
{    
	return strtoll (str, NULL, 10);
}


char *rtrim(char *s   )   
{   
    register   int   l;   
    for(l=strlen(s); l>0 && (isspace((u_char)s[l-1]) || s[l-1] == '\r' || s[l-1] == '\n' ); l--)   
		s[l-1]='\0';   
    return(s);   
}

char *ltrim(char *s   )   
{   
	register   char   *p;	
	for(p=s; isspace((u_char)*p);p++ );	
	if(p!=s)	  
		strcpy(s, p);	 
	return(s);	 
}





char *inet_htoa(unsigned int ipaddr)
{
    static int  buf_index=0;
    unsigned char  t1;
    char   bFlag = 0;
    static char buf[10][20];
    char   *pbuf = NULL;

    buf_index = buf_index%10;
	pbuf      = buf[buf_index];

    t1 = (ipaddr >> 24) & 0xff;
    *pbuf = (t1 / 100);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
        bFlag = 1;
    }
    *pbuf = ((t1 / 10) % 10);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    else if (bFlag)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    *pbuf++ = (t1 % 10) + 0x30;
    *pbuf++ = '.';

    /******************************/
    bFlag = 0;
    t1 = (ipaddr >> 16) & 0xff;
    *pbuf = (t1 / 100);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
        bFlag = 1;
    }
    *pbuf = ((t1 / 10) % 10);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    else if (bFlag)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    *pbuf++ = (t1 % 10) + 0x30;
    *pbuf++ = '.';

    /******************************/
    bFlag = 0;
    t1 = (ipaddr >> 8) & 0xff;
    *pbuf = (t1 / 100);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
        bFlag = 1;
    }
    *pbuf = ((t1 / 10) % 10);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    else if (bFlag)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    *pbuf++ = (t1 % 10) + 0x30;
    *pbuf++ = '.';

    /******************************/
    bFlag = 0;
    t1 = ipaddr & 0xff;
    *pbuf = (t1 / 100);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
        bFlag = 1;
    }
    *pbuf = ((t1 / 10) % 10);
    if (*pbuf != 0)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    else if (bFlag)
    {
        *pbuf += 0x30;
        pbuf++;
    }
    *pbuf++ = (t1 % 10) + 0x30;
    *pbuf = '\0';

    pbuf   = buf[buf_index++];
    return pbuf;
}


/*获取CPU占用率*/  
int getCpuUsage(float *pfCpuUsage)
{  
	char read_buf[256];
	long long  tmpused,tmptotal,tmpused_pre=0,tmptotal_pre=0;
	char  tmp1[32],tmp2[32],tmp3[32],tmp4[32],tmp5[32],tmp6[32],tmp7[32];

    FILE* Fd = fopen("/proc/stat", "r"); 
	if(Fd == NULL)
		return -1;

	if(fgets(read_buf, 256, Fd) == NULL)
		return -1;

   tmpused  = 0;
   tmptotal = 0;

   sscanf(read_buf,"%*[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^ ]",tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7);
   tmptotal += atoll(tmp1); //user
   tmptotal += atoll(tmp2); //nice
   tmptotal += atoll(tmp3); //system
   tmptotal += atoll(tmp4); //idle
   tmptotal += atoll(tmp5); //system
   tmptotal += atoll(tmp6); //system
   tmptotal += atoll(tmp7); //system tmptotal instand total cpu time

   tmpused = atoll(tmp4);   //tmp4 instand idle time

   if(tmptotal ==0 )
      tmptotal = 1;

   tmptotal_pre = tmptotal;
   tmpused_pre  = tmpused;

   fclose(Fd);

   usleep(2000000);  

   Fd = fopen("/proc/stat", "r"); 
   if(Fd == NULL)
		return -1;

  if(fgets(read_buf, 256, Fd) == NULL)
		return -1;
   tmpused  = 0;
   tmptotal = 0;

   sscanf(read_buf,"%*[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^ ]",tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7);
   tmptotal += atoll(tmp1); //user
   tmptotal += atoll(tmp2); //nice
   tmptotal += atoll(tmp3); //system
   tmptotal += atoll(tmp4); //idle
   tmptotal += atoll(tmp5); //system
   tmptotal += atoll(tmp6); //system
   tmptotal += atoll(tmp7); //system tmptotal instand total cpu time

   tmpused = atoll(tmp4);   //tmp4 instand idle time
   //*pfCpuUsage =100.0- (tmpused*100)/(tmptotal*1.0);
   *pfCpuUsage = 100.0-((tmpused-tmpused_pre)*100)/((tmptotal-tmptotal_pre)*1.0);
  
    return 0;  
}  





static void unescape(char *s)
{
  unsigned int c;

  while ((s = strpbrk(s, "%+"))) 
  {
    /* Parse %xx */
    if (*s == '%') 
	{
      sscanf(s + 1, "%02x", &c);
      *s++ = (char) c;
      strncpy(s, s + 2, strlen(s) + 1);
    }
    /* Space is special */
    else if (*s == '+')
      *s++ = ' ';
  }
}

/*子字符串分割，会破坏掉原始字符串*/
void split_str(char* pStrSource,const char *pSeps,char **pSubStr,int *pNum)
{
    char *token = NULL;
    int  i = 0;
    token = strtok( pStrSource, pSeps);
    while(token != NULL)
    {
        pSubStr[i++] = token;
        token = strtok( NULL, pSeps);
    }
    *pNum = i;
}


char * get_cgi(char *name)
{
  ENTRY e, *ep=NULL;

  if (!htab.table)
    return NULL;
  e.key = name;
  hsearch_r(e, FIND, &ep, &htab);
  return ep ? ep->data : NULL;
}

void set_cgi(char *name, char *value)
{
  ENTRY e, *ep=NULL;

  if (!htab.table)
    return;

  e.key = name;
  hsearch_r(e, FIND, &ep, &htab);
  if (ep)
  {
    ep->data = value;
  }
  else 
  {
    e.data = value;
    hsearch_r(e, ENTER, &ep, &htab);
    htab_count++;
  }
}

void init_cgi(char *query,int len)
{
  long nel=0;
  char *q, *name, *value;
  char *tmp;
	
  htab_count = 0;

  if (!query) {
    hdestroy_r(&htab);
    return;
  }

  q = query;
  tmp = query;

  while(*tmp)
  {
		if(*tmp++ == '&')
			nel++;
  }
  if(nel!=0)
  	nel++;
   hcreate_r(nel, &htab);

  while ((value = strsep(&q, "&")))
  {
	 name = strsep(&value, "=");
    if (value&& name)
      set_cgi(name, value);
  }

 }



int count_cgi()
{
  return htab_count;
}

void destory_cgi()
{
	if(htab_count != 0)
	{
		hdestroy_r(&htab);
		htab_count = 0;
	}
}


#define CGI_LOG_FILE "/var/log/cgi.log"
int cgiLog(const char *format ,... )
{
	static FILE *fpLog = NULL;
	struct stat buf;
	if(fpLog == NULL)
	{
	//	char fileName[256];
	//	sprintf(fileName,CGI_LOG_FILE);
	//	fpLog = fopen(fileName,"w+");
	//	printf("fileName=%s\n",fileName);


	int result = 0;
	result = stat( CGI_LOG_FILE, &buf );
	if(result != 0)  //不存在
	{
		fpLog = fopen(CGI_LOG_FILE,"w+");
	}
	else 
	{
		if( buf.st_size > (5*1024*1024))
			fpLog = fopen(CGI_LOG_FILE,"w");
		else
			fpLog = fopen(CGI_LOG_FILE,"a");	
	}


	
	}
	if(fpLog == NULL)
		return -1;
	
	va_list args;
 
    char szDebugBuf[2048] = {0};
    int iPos = 0 ;
    int iLen;
	time_t timer;
    char asctimeBuf[BUFSIZ];

	(void)time(&timer);
    ctime1(&timer, asctimeBuf);
	
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
	}
    return (iLen + iPos);	
}


/*
	将十六进制数转换为对应十进制整数
*/
static char HexToInt(char hex)
{
	if((hex>='0')&&(hex<='9'))
		return (hex - '0');
	if((hex>='a')&&(hex<='z'))
		return (hex -'a'+10);
	if((hex>='A')&&(hex<='Z'))
		return (hex -'A'+10);
	
	return -1;
}

/*
	将十六进制数字（ascii码）转换为对应的字符
*/
static char HexToChar(const char *str)
{
	char digit;
	digit = (HexToInt(str[0]));
	digit = digit <<4;
	digit += HexToInt(str[1]);
	return (digit);
}

/*
	将'+'替换为空格
*/
static char * PlusToSpace(char *str)
{
	int i=0;
	if(str==NULL)
		return NULL;
	while(str[i])
	{
		if(str[i]=='+')str[i]=' ';
		i++;
	}
	return (str);
}
char *UrlDecode(char *in_str,int out_len,char *out_buf)
{
	int x,y;
	char *buf = out_buf;
	char *str = in_str;
	if(!(in_str && out_buf))
		return NULL;

	//size_t length  = strlen(str)-2;
	for(x = 0,y=0;str[y];++x,++y)
	{
		if((buf[x]=str[y])=='%')
		{
			buf[x] = HexToChar(&str[y+1]);
			y+=2;
		}
		
	}
	buf[x] = '\0';
	
	PlusToSpace(buf);
//	printf("buf:%s \n",buf);
	return buf;
}




int CreateAllDirectory(char *dir)
{
	char *pTemp 	= NULL;
	char *pTemp1	= NULL;
	char ch;
	char path[128];
	
	strcpy(path,dir);
	if(access(path,0) == 0)
	{
		return 0;
	}

	pTemp = path;
	/*去掉第一个跟路径*/
	while(*pTemp == '\\' || *pTemp == '/')
		pTemp++;
	while(1)
	{
		pTemp1 = pTemp;
		pTemp = strchr(pTemp1,'\\');
		if(pTemp == NULL)
		{
			pTemp = strchr(pTemp1,'/');
			if(pTemp == NULL)
				break;
		}
		ch = *pTemp;
		if(*(pTemp-1) == ':')
		{
			pTemp += 1;
			continue;
		}
		*pTemp = 0;
		if(access(path,0) != 0)
		{
			if(mkdir(path,0777) != 0)
			{
				return -1;
			}
		}
		*pTemp = ch;
		pTemp++;
	}
	return 0;
}



int  fgetline(FILE *fp, char *buffer, int maxlen) 
{ 
    int  i, j; 
    char ch1; 
    
    for(i = 0, j = 0; i < maxlen; j++) 
    { 
        if(fread(&ch1, sizeof(char), 1, fp) != 1) 
        { 
            if(feof(fp) != 0) 
            { 
                if(j == 0) return -1;               /* EOF */ 
                else break; 
            } 
            if(ferror(fp) != 0) return -2;        /* error */ 
            return -2; 
        } 
        else 
        { 
            if(ch1 == '\n' || ch1 == 0x00) break; /* end of line */ 
            if(ch1 == '\f' || ch1 == 0x1A)        /* '\f': Form Feed */ 
            { 
                buffer[i++] = ch1; 
                break; 
            } 
            if(ch1 != '\r') buffer[i++] = ch1;    /* ignore CR */ 
        } 
    } 
    buffer[i] = '\0'; 
    return i; 
} 



int  split_key_val(char *buf, char **key, char **val) 
{ 
    int  i, k1, k2, n; 
    
    if((n = strlen((char *)buf)) < 1) return 0; 
    for(i = 0; i < n; i++) 
        if(buf[i] != ' ' && buf[i] != '\t') break; 
        if(i >= n) return 0; 
        if(buf[i] == '=') return -1; 
        k1 = i; 
        for(i++; i < n; i++) 
            if(buf[i] == '=') break; 
            if(i >= n) return -2; 
            k2 = i; 
            for(i++; i < n; i++) 
                if(buf[i] != ' ' && buf[i] != '\t') break; 
                buf[k2] = '\0'; 
                *key = buf + k1; 
                *val = buf + i; 
                return 1; 
} 


int  copy_txt_file(char *source_file, char *dest_file) 
{ 
    FILE *fp1, *fp2; 
    char buf[1024+1]; 
    int  rc; 
    
    if((fp1 = fopen((char *)source_file, "r")) == NULL) 
        return COPYF_ERR_OPEN_FILE; 
    rc = COPYF_ERR_CREATE_FILE; 
    if((fp2 = fopen((char *)dest_file, "w")) == NULL) goto copy_end; 
    while(1) 
    { 
        rc = COPYF_ERR_READ_FILE; 
        memset(buf, 0x00, 1024+1); 
        if(fgets((char *)buf, 1024, fp1) == NULL) 
        { 
            if(strlen(buf) == 0) 
            { 
                if(ferror(fp1) != 0) goto copy_end; 
                break;                                   /* EOF */ 
            } 
        } 
        rc = COPYF_ERR_WRITE_FILE; 
        if(fputs((char *)buf, fp2) == EOF) goto copy_end; 
    } 
    rc = COPYF_OK; 
copy_end: 
    if(fp2 != NULL) fclose(fp2); 
    if(fp1 != NULL) fclose(fp1); 
    return rc; 
} 



int  CFG_get_key(char *CFG_file, const char *section, const char *key, char *buf) 
{ 
	FILE *fp; 
	char buf1[MAX_CFG_BUF + 1], buf2[MAX_CFG_BUF + 1]; 
	char *key_ptr, *val_ptr; 
	int  line_no, n, rc; 
	
	line_no = 0; 
	CFG_section_line_no = 0; 
	CFG_key_line_no = 0; 
	CFG_key_lines = 0; 
	if((fp = fopen((char *)CFG_file, "rb")) == NULL) 
		return CFG_ERR_OPEN_FILE; 
	while(1)									   /* seek section */ 
	{ 
		rc = CFG_ERR_READ_FILE; 
		n = fgetline(fp, buf1, MAX_CFG_BUF); 
		if(n < -1) goto r_cfg_end; 
		rc = CFG_SECTION_NOT_FOUND; 
		if(n < 0) goto r_cfg_end;					 /* EOF, not found */ 
		line_no++; 
		n = strlen(buf1); 
		if(n == 0 || buf1[0] == ';') continue;		 /* blank/remarks line */ 
		rc = CFG_ERR_FILE_FORMAT; 
		if(n > 2 && ((buf1[0] == '[' && buf1[n-1] != ']') || 
			(buf1[0] == '{' && buf1[n-1] != '}'))) 
			goto r_cfg_end; 
		if(buf1[0] == '[' || buf1[0] == '{') 
		{ 
			buf1[n-1] = 0x00; 
			if(strcmp(buf1+1, section) == 0) 
				break;									 /* section found */ 
		} 
	} 
	CFG_section_line_no = line_no; 
	while(1)									   /* seek key */ 
	{ 
		rc = CFG_ERR_READ_FILE; 
		n = fgetline(fp, buf1, MAX_CFG_BUF); 
		if(n < -1) goto r_cfg_end; 
		rc = CFG_KEY_NOT_FOUND; 
		if(n < 0) goto r_cfg_end;					 /* EOF, key not found */ 
		line_no++; 
		CFG_key_line_no = line_no; 
		CFG_key_lines = 1; 
		n = strlen(buf1); 
		if(n == 0 || buf1[0] == ';') continue;		 /* blank/remarks line */ 
		rc = CFG_KEY_NOT_FOUND; 
		if(buf1[0] == '[' || buf1[0] == '{') goto r_cfg_end; 
		if(buf1[n-1] == '+')						 /* to be continued */ 
		{ 
			buf1[n-1] = 0x00; 
			while(1) 
			{ 
				rc = CFG_ERR_READ_FILE; 
				n = fgetline(fp, buf2, MAX_CFG_BUF); 
				if(n < -1) goto r_cfg_end; 
				if(n < 0) break;						 /* EOF */ 
				line_no++; 
				CFG_key_lines++; 
				n = strlen(buf2); 
				rc = CFG_ERR_EXCEED_BUF_SIZE; 
				if(n > 0 && buf2[n-1] == '+')			 /* to be continued */ 
				{ 
					buf2[n-1] = 0x00; 
					if(strlen(buf1) + strlen(buf2) > MAX_CFG_BUF) 
						goto r_cfg_end; 
					strcat(buf1, buf2); 
					continue; 
				} 
				if(strlen(buf1) + strlen(buf2) > MAX_CFG_BUF) 
					goto r_cfg_end; 
				strcat(buf1, buf2); 
				break; 
			} 
		} 
		rc = CFG_ERR_FILE_FORMAT; 
		if(split_key_val(buf1, &key_ptr, &val_ptr) != 1) 
			goto r_cfg_end; 
		if(strcmp(key_ptr, key) != 0) 
			continue;								   /* not same key */ 
		strcpy(buf, val_ptr); 
		break; 
	} 
	rc = CFG_OK; 
r_cfg_end: 
	if(fp != NULL) fclose(fp); 
	return rc; 
}


int  CFG_set_key(char *CFG_file, char *section, char *key, char *buf) 
{ 
	FILE *fp1, *fp2; 
	char buf1[MAX_CFG_BUF + 1]; 
	int  line_no, line_no1, n, rc, rc2; 
	char *tmpfname; 
	
	rc = CFG_get_key(CFG_file, section, key, buf1); 
	if(rc <= CFG_ERR && rc != CFG_ERR_OPEN_FILE) return rc; 
	if(rc == CFG_ERR_OPEN_FILE || rc == CFG_SECTION_NOT_FOUND) 
	{ 
		if((fp1 = fopen((char *)CFG_file, "a")) == NULL) 
			return CFG_ERR_CREATE_FILE; 
		CFG_ssl = '['; 
		CFG_ssr = ']';	/* .CFG/.INI file section symbol */ 
		if(fprintf(fp1, "%c%s%c\n", CFG_ssl, (char*)section, CFG_ssr) == EOF) 
		{ 
			fclose(fp1); 
			return CFG_ERR_WRITE_FILE; 
		} 
		if(fprintf(fp1, "%s=%s\n", (char*)key, (char*)buf) == EOF) 
		{ 
			fclose(fp1); 
			return CFG_ERR_WRITE_FILE; 
		} 
		fclose(fp1); 
		return CFG_OK; 
	} 
	if((tmpfname = tmpnam(NULL)) == NULL) 
		return CFG_ERR_CREATE_FILE; 
	if((fp2 = fopen(tmpfname, "w")) == NULL) 
		return CFG_ERR_CREATE_FILE; 
	rc2 = CFG_ERR_OPEN_FILE; 
	if((fp1 = fopen((char *)CFG_file, "rb")) == NULL) goto w_cfg_end; 
	if(rc == CFG_KEY_NOT_FOUND) 
		line_no1 = CFG_section_line_no; 
	else /* rc = CFG_OK */ 
		line_no1 = CFG_key_line_no - 1; 
	for(line_no = 0; line_no < line_no1; line_no++) 
	{ 
		rc2 = CFG_ERR_READ_FILE; 
		n = fgetline(fp1, buf1, MAX_CFG_BUF); 
		if(n < 0) goto w_cfg_end; 
		rc2 = CFG_ERR_WRITE_FILE; 
		if(fprintf(fp2, "%s\n", buf1) == EOF) goto w_cfg_end; 
	} 
	if(rc != CFG_KEY_NOT_FOUND) 
		for( ; line_no < line_no1+CFG_key_lines; line_no++) 
		{ 
			rc2 = CFG_ERR_READ_FILE; 
			n = fgetline(fp1, buf1, MAX_CFG_BUF); 
			if(n < 0) goto w_cfg_end; 
		} 
		rc2 = CFG_ERR_WRITE_FILE; 
		if(fprintf(fp2, "%s=%s\n", (char*)key, (char*)buf) == EOF) 
			goto w_cfg_end; 
		while(1) 
		{ 
			rc2 = CFG_ERR_READ_FILE; 
			n = fgetline(fp1, buf1, MAX_CFG_BUF); 
			if(n < -1) goto w_cfg_end; 
			if(n < 0) break; 
			rc2 = CFG_ERR_WRITE_FILE; 
			if(fprintf(fp2, "%s\n", buf1) == EOF) goto w_cfg_end; 
		} 
		rc2 = CFG_OK; 
w_cfg_end: 
		if(fp1 != NULL) fclose(fp1); 
		if(fp2 != NULL) fclose(fp2); 
		if(rc2 == CFG_OK) 
		{ 
			rc = copy_txt_file(tmpfname, CFG_file); 
			if(rc != 0) return CFG_ERR_CREATE_FILE; 
		} 
		remove(tmpfname); 
		return rc2; 
}




