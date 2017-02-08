#ifndef UTIL_INCLUDE_H_
#define UTIL_INCLUDE_H_


#define MAX_TXT_LINE_FLD                         128 
#define MAX_CFG_BUF                              512 

#define CFG_OK                                   0 
#define CFG_SECTION_NOT_FOUND                    -1 
#define CFG_KEY_NOT_FOUND                        -2 
#define CFG_ERR                                  -10 
#define CFG_ERR_FILE                             -10 
#define CFG_ERR_OPEN_FILE                        -10 
#define CFG_ERR_CREATE_FILE                      -11 
#define CFG_ERR_READ_FILE                        -12 
#define CFG_ERR_WRITE_FILE                       -13 
#define CFG_ERR_FILE_FORMAT                      -14 
#define CFG_ERR_SYSTEM                           -20 
#define CFG_ERR_SYSTEM_CALL                      -20 
#define CFG_ERR_INTERNAL                         -21 
#define CFG_ERR_EXCEED_BUF_SIZE                  -22

#define COPYF_OK                                 0
#define COPYF_ERR_OPEN_FILE                      -10 
#define COPYF_ERR_CREATE_FILE                    -11 
#define COPYF_ERR_READ_FILE                      -12 
#define COPYF_ERR_WRITE_FILE                     -13 


char *rtrim(char *s );
char *ltrim(char *s );   
char *inet_htoa(unsigned int ipaddr);

void split_str(char* pStrSource,const char *pSeps,char **pSubStr,int *pNum);

extern void init_cgi(char *query,int len);
int count_cgi();
char *get_cgi(char *name);
void destory_cgi();

int cgiLog(const char *format ,... );
char *UrlDecode(char *in_str,int out_len,char *out_buf);


void *attach_shm(char *path, int shmsize);
int CreateAllDirectory(char *dir);
int  CFG_get_key(char *CFG_file, const char *section, const char *key, char *buf);
int  CFG_set_key(char *CFG_file, char *section, char *key, char *buf);


#endif

