#include <ngl_types.h>
#include <ngl_log.h>
#include <stdarg.h> 
#include <ngl_os.h>
#include <stdio.h>
#include <string.h>

#define HASH_MODULE_SIZE 23

static NGLLogLevel sLogLevel=NGL_LOG_DEBUG;
typedef struct HashNode{
  const char *name;
  int level;
  struct HashNode*next;
}HASHNODE;

typedef struct{
  HASHNODE **nodes;
  int size;
}HASHTABLE;


static HASHTABLE*Create_Hash(int size){
    HASHTABLE*tbl=(HASHTABLE*)nglMalloc(sizeof(HASHTABLE));
    tbl->size=size;
    tbl->nodes=(HASHNODE**)nglMalloc(size*sizeof(HASHNODE*));
    memset(tbl->nodes,0,size*sizeof(HASHNODE*));
    return tbl;
}
unsigned int SDBMHash(const char *str){
    unsigned int hash = 0;
    while (*str){// equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }
    return (hash & 0x7FFFFFFF);
}

static int Insert_Hash(HASHTABLE*tbl,const char*name,int level)
{
    int idx=SDBMHash(name);
    printf("Insert_Hash %s hash=%d/%d\r\n",name,idx,idx%tbl->size);
    idx%=tbl->size;
    HASHNODE*nd=(HASHNODE*)nglMalloc(sizeof(HASHNODE));
    nd->name=strdup(name);
    nd->level=level;
    nd->next=tbl->nodes[idx];
    tbl->nodes[idx]=nd;
    return idx;
}

static HASHNODE* Find_Hash(HASHTABLE*tbl,const char*name){
    int idx=SDBMHash(name);
    if(tbl==NULL)
       return NULL;
    idx%=tbl->size;
    HASHNODE*nd=tbl->nodes[idx];
    while(nd){
       if(0==strcmp(nd->name,name))
           return nd;
       nd=nd->next;
    } 
    return NULL;
}

static HASHTABLE*sModules=NULL;
const char*colors[]={"\033[0m","\033[1m","\033[0;32m","\033[0;36m","\033[1;31m","\033[5;31m"};

void nglLogPrintf(const char*tag,int level,const char*func,int line,const char*format,...)
{
    char str[256]={0};
    va_list args={0};
    HASHNODE*nd=Find_Hash(sModules,tag);
    int module_loglevel=nd?nd->level:sLogLevel;
    if(level<module_loglevel)
        return;
    snprintf(str,sizeof(str)," \033[0;32m[%s]\033[0;34m \%s:%d ",tag,func,line);
    if(level>=0&&level<=NGL_LOG_ALERT)
         strcat(str,colors[level]);
    printf(str);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\033[0m\r\n");
}

void nglLogDump(const char*tag,int level,const char*func,int line,const char*label,const BYTE*data,int len)
{
    static int buflen=0;
    static char *buff=NULL;
    int i,j,taglen=0;
    if(len>buflen||NULL==buff){
       buff=nglRealloc(buff,len*4+128);
       buflen=len;
    }
    if(NULL==data||len<0){
        printf("%s:%p:%d\n",label,data,len);
        return ;
    }
    if(len<32)taglen=sprintf(buff,"%s",label);
    else taglen=sprintf(buff,"%s[%d]\r\n",label,len);
    for(i=0,j=taglen;i<len;i++)
       j+=sprintf(buff+j,"%02X%s",data[i],(i%32==0)&&(i>0)?"\r\n":" ");
    buff[j]=0;
    nglLogPrintf(tag,level,func,line,"%s",buff);
}

void nglLogSetModuleLevel(const char*module,int level){
    if(module==NULL){
        int i;
        sLogLevel=level;
        for(i=0;sModules && i<sModules->size;i++){
            HASHNODE*nd=sModules->nodes[i];
            while(nd){
                nd->level=level;
                nd=nd->next;
            }
        }
    }else {
        HASHNODE*nd;
        if(NULL==sModules)
            sModules=Create_Hash(HASH_MODULE_SIZE);
        nd=Find_Hash(sModules,module);
        if(NULL==nd)
            Insert_Hash(sModules,module,level);
        else
            nd->level=level;
    }
}

void nglLogParseModule(const char*log){
   char module[128];
   char*p=strchr(log,':');
   if(p==NULL)return;
   strncpy(module,log,p-log);
   module[p-log]=0;
   NGLLogLevel l=NGL_LOG_DEBUG;
   switch(p[1]){
   case 'v':
   case 'V':l=NGL_LOG_VERBOSE;break;
   case 'd':
   case 'D':l=NGL_LOG_DEBUG;break;
   case 'i':
   case 'I':l=NGL_LOG_INFO;break;
   case 'w':
   case 'W':l=NGL_LOG_WARN;break;
   case 'e':
   case 'E':l=NGL_LOG_ERROR;break;
   }
   nglLogSetModuleLevel( ((strcmp(module,"*")==0)?NULL:module),l);
}

void nglLogParseModules(int argc,const char*argv[]){
   int i;
   for ( i = 1; i <argc; i++) {
       if(strchr(argv[i],':')){
          nglLogParseModule(argv[i]);
       }
   }
}
