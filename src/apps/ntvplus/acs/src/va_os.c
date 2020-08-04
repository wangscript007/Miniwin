/*
  FILE : stub_os.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_os.h"
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <aui_os.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <ngl_types.h>
#include <ngl_log.h>

NGL_MODULE(ACSOS)

//typedef pthread_mutex_t tVA_OS_Mutex;
INT VA_OS_InitializeSemaphore(tVA_OS_Semaphore * const pSemaphore, UINT32 uiValue )
{
  NGLOG_VERBOSE("%s: pSemaphore=%p\r\n",__FUNCTION__,pSemaphore);
  if(pSemaphore!=NULL){
      sem_t *sem=malloc(sizeof(sem_t));
      if(0!=sem_init(sem,1,uiValue))
          return kVA_INVALID_PARAMETER;//SET_SEMFLAG(pSemaphore,SEM_INITED);
      else{ 
          *pSemaphore[0] = sem;
		  return kVA_OK;
      }
  }
  return kVA_INVALID_PARAMETER;
}

INT VA_OS_DeleteSemaphore(tVA_OS_Semaphore * const pSemaphore)
{
  NGLOG_VERBOSE("%s: pSemaphore=%p  \r\n",__FUNCTION__,pSemaphore);
  if(pSemaphore!=NULL &&(*pSemaphore[0]!=0)){
      int ret;
      sem_t*sem=*pSemaphore[0];
	  if(0!=sem_destroy(sem))
          return kVA_INVALID_PARAMETER;
      else{
	      *pSemaphore[0]=0;
		  return kVA_OK;
	  } 
  }else
   return kVA_INVALID_PARAMETER;

}
#define VA_INVALID_DURATION 0x87654321
INT VA_OS_AcquireSemaphore(tVA_OS_Semaphore * const pSemaphore, UINT32 uiDuration )
{
  //NGLOG_VERBOSE("%s: pSemaphore=%p duration=0x%x\r\n",__FUNCTION__,pSemaphore,uiDuration);
  if((pSemaphore!=NULL)&&(*pSemaphore[0]!=0)){
      sem_t*sem=*pSemaphore[0];
      if(kVA_OS_WAIT_FOREVER==uiDuration){
          if(sem!=0xFFFFFFFF){
              return (0==sem_wait(sem))?kVA_OK:kVA_INVALID_PARAMETER;
          }else
              return kVA_INVALID_PARAMETER;
	  }else{
	       return kVA_INVALID_PARAMETER;
	  } 
	  return kVA_INVALID_PARAMETER;
  }
  return kVA_INVALID_PARAMETER;
}


INT VA_OS_ReleaseSemaphore(tVA_OS_Semaphore * const pSemaphore )
{
  //NGLOG_VERBOSE("%s: pSemaphore=%p\r\n",__FUNCTION__,pSemaphore);
  if((pSemaphore!=NULL)&&(*pSemaphore[0]!=0)){
      sem_t*sem=*pSemaphore[0];
      if(0!=sem_post(sem))return kVA_INVALID_PARAMETER;
      else return kVA_OK;
  }
  return kVA_INVALID_PARAMETER;
}

 VA_OS_InitializeMutex(tVA_OS_Mutex * const pMutex)
{
  //NGLOG_VERBOSE("%s:pMutex=%p\r\n",__FUNCTION__,pMutex);
  if(pMutex){
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      //PTHREAD_PROCESS_PRIVATE;
      pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);//PTHREAD_PROCESS_PRIVATE PTHREAD_PROCESS_SHARED
      pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);//PTHREAD_MUTEX_RECURSIVE);//PTHREAD_MUTEX_ERRORCHECK
      pthread_mutex_init((pthread_mutex_t*)pMutex,&attr);
      pthread_mutexattr_destroy(&attr);
      //SET_MTXFLAG(pMutex,MTX_INITED);
  }
  return pMutex?kVA_OK:kVA_INVALID_PARAMETER;
}


INT VA_OS_DeleteMutex(tVA_OS_Mutex * const pMutex)
{
  //NGLOG_VERBOSE("%s:pMutex=%p\r\n",__FUNCTION__,pMutex);
  if(pMutex){
      int ret=pthread_mutex_destroy((pthread_mutex_t*)pMutex);
      //SET_MTXFLAG(pMutex,MTX_DELETED);
      return ret==0?kVA_OK:kVA_INVALID_PARAMETER;
  }
  return pMutex?kVA_OK:kVA_INVALID_PARAMETER;
}


INT VA_OS_LockMutex(tVA_OS_Mutex * const pMutex)
{
  //NGLOG_VERBOSE("%s:pMutex=%p\r\n",__FUNCTION__,pMutex);
  if(pMutex){
      int ret=pthread_mutex_lock((pthread_mutex_t*)pMutex);
      return ret==0?kVA_OK:kVA_INVALID_PARAMETER;
      //SET_MTXFLAG(pMutex,MTX_LOCKED);
   }
  return pMutex?kVA_OK:kVA_INVALID_PARAMETER;
}


INT VA_OS_UnlockMutex(tVA_OS_Mutex * const pMutex)
{
  //NGLOG_VERBOSE("%s:pMutex=%p\r\n",__FUNCTION__,pMutex);
  if(pMutex){
      int ret;
      //if(GET_MTXFLAG(pMutex)==MTX_LOCKED){
           ret=pthread_mutex_unlock((pthread_mutex_t*)pMutex);
           //SET_MTXFLAG(pMutex,MTX_INITED);
           return ret==0?kVA_OK:kVA_INVALID_PARAMETER;
      //}
      return kVA_INVALID_PARAMETER;
  }
  return pMutex?kVA_OK:kVA_INVALID_PARAMETER;
}


void* VA_OS_Alloc ( UINT32 uiSize)
{
  return (void*)calloc(uiSize,1);
}


void VA_OS_Free(void * ptr)
{
  if(ptr!=NULL)
     free(ptr);
  return;
}


INT VA_OS_Printf(const char *pFormat, ...)
{
  va_list args;
  va_start(args, pFormat);
  vprintf(pFormat,args);
  va_end(args);
  return 0;
}

extern  void ProcessCommand(char*cmd);
INT VA_OS_Getc(UINT32 uiDuration)
{   //reads the characters from a unique external communication port (preferably a serial port), if available
    static char cmdbuffer[64];
    static int index=0;
    int ret,c = kVA_TIMEOUT;
    struct timeval to={0,0};
    struct timeval *pt=&to;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO,&rfds);
    if(kVA_OS_WAIT_FOREVER==uiDuration){
        pt=NULL; 
    }else if(kVA_OS_NO_WAIT!=uiDuration){
         to.tv_sec=uiDuration/1000;
         to.tv_usec=(uiDuration%1000)*1000;
    }
    ret=select(STDIN_FILENO+1,&rfds,NULL,NULL,pt);
    if(ret>0){
         if(-1==read(STDIN_FILENO, &c, 1))
            c=kVA_TIMEOUT;
    }else if(ret==-1){
         NGLOG_VERBOSE("VA_OS_Getc not reached\r\n");exit(0);
    }
    if(0!=uiDuration)
        NGLOG_VERBOSE("VA_OS_Getc(0x%x) =0x%x\r\n",uiDuration,c);
    cmdbuffer[index]=c;
    if(index>=sizeof(cmdbuffer))index=0;
    if(c=='\r'||c=='\n'){
        cmdbuffer[index]=0;
        //ProcessCommand(cmdbuffer);bzero(cmdbuffer,sizeof(cmdbuffer));index=0;
    }else index++;
    return c;//return getchar();
}


void VA_OS_Sleep( UINT32 uiDuration )
{
  usleep(uiDuration*1000);
  return;
}


typedef struct{
   tVA_OS_RunTime starttime;
   UINT32 duration;
   tVA_OS_PeriodicCallFct callback;
   DWORD data;
}PERIODCALL;

#define MAX_PERIODCALL 128 

static PERIODCALL sPeriodCalls[MAX_PERIODCALL];
static  pthread_t sPeriodThread=0;

void*PeriodProc(void*p){
    while(1){
        int i;
        INT32 sleepms=100;//10s
        tVA_OS_RunTime tnow;
        VA_OS_GetRunTime(&tnow);
        for(i=0;i<MAX_PERIODCALL;i++){
             PERIODCALL*pc=sPeriodCalls+i;
             if(pc->callback==NULL)continue;
             if(pc->starttime.uiMilliSec+pc->duration<=tnow.uiMilliSec){
                if(pc->duration>1000)
                    NGLOG_VERBOSE("PERIOD=0x%x start=0x%x now=0x%x  dur=0x%x proc=%p data=0x%x",pc->duration,pc->starttime.uiMilliSec,
                     tnow.uiMilliSec,(tnow.uiMilliSec-pc->starttime.uiMilliSec),pc->callback,pc->data);
                sPeriodCalls[i].callback(sPeriodCalls[i].data);
                sPeriodCalls[i].starttime=tnow;
             }else{
                INT32 ms= sPeriodCalls[i].starttime.uiMilliSec+ sPeriodCalls[i].duration-tnow.uiMilliSec;
                if(sleepms>ms)sleepms=ms;
             }
        }
        if(sleepms>0){
            struct timeval tempval;
            tempval.tv_sec = sleepms/1000;
            tempval.tv_usec =(sleepms%1000)*1000;
            select(0, NULL, NULL, NULL, &tempval);
        }
    }
}

DWORD VA_OS_StartPeriodicCall( UINT32 uiDurationPeriod,tVA_OS_PeriodicCallFct pfPeriodicCallFunction,
                               DWORD dwViaccessData )
{
    int i;
    if(sPeriodThread==0){
        NGLOG_INFO("VA_OS_StartPeriodicCall init PeriodCall Thread");
        pthread_create(&sPeriodThread,NULL,PeriodProc,NULL);
        bzero(sPeriodCalls,sizeof(PERIODCALL)*MAX_PERIODCALL);
    }
    NGLOG_DEBUG("uiDurationPeriod=%x pfPeriodicCallFunction=%p",uiDurationPeriod,pfPeriodicCallFunction);
    if(uiDurationPeriod==0||uiDurationPeriod<100||pfPeriodicCallFunction==NULL)
           return kVA_ILLEGAL_HANDLE;
    for(i=0;i<MAX_PERIODCALL;i++){
       if(sPeriodCalls[i].callback==NULL){
           tVA_OS_RunTime tnow;
           VA_OS_GetRunTime(&tnow);
           NGLOG_VERBOSE("VA_OS_StartPeriodicCall starttime=0x%x dur=0x%x",tnow.uiMilliSec,uiDurationPeriod);
           sPeriodCalls[i].callback=pfPeriodicCallFunction;
           sPeriodCalls[i].duration=uiDurationPeriod;
           sPeriodCalls[i].data=dwViaccessData;
           sPeriodCalls[i].starttime=tnow;
           return (DWORD)(sPeriodCalls+i);
       }
    }
    return kVA_ILLEGAL_HANDLE;
}


INT VA_OS_StopPeriodicCall( DWORD dwHandle )
{
  PERIODCALL*p=(PERIODCALL*)dwHandle;
  NGLOG_DEBUG("dwHandle=0x%x",dwHandle);
  if(dwHandle==kVA_ILLEGAL_HANDLE)
      return kVA_INVALID_PARAMETER;
  if( (p>=sPeriodCalls) && (p<sPeriodCalls+MAX_PERIODCALL) && p->callback){
      p->callback=NULL;
      return kVA_OK;
  }
  return kVA_INVALID_PARAMETER;
}

void VA_OS_GetTime(tVA_OS_Time *pTime)
{
  if(pTime)*pTime=time(NULL);
}


void VA_OS_TimeToTm(const tVA_OS_Time  *pTime, tVA_OS_Tm  *pTm )
{
  struct tm *t=gmtime((time_t*)pTime);
  if(pTm&&pTime){
    pTm->uiSec=t->tm_sec;
    pTm->uiMin=t->tm_min;
    pTm->uiHour=t->tm_hour;
    pTm->uiMonthDay=t->tm_mday;
    pTm->uiMonth=t->tm_mon;
    pTm->uiYear=t->tm_year;
    pTm->uiWeekDay=t->tm_wday;
    pTm->uiYearDay=t->tm_yday;
  }
  return;
}

void VA_OS_TmToTime(const tVA_OS_Tm *pTm, tVA_OS_Time *pTime)
{
  if(pTime&&pTm){
    struct tm t;
    t.tm_sec=pTm->uiSec;
    t.tm_min=pTm->uiMin;
    t.tm_hour=pTm->uiHour;
    t.tm_mday=pTm->uiMonthDay;
    t.tm_mon=pTm->uiMonth;
    t.tm_year=pTm->uiYear;
    t.tm_wday=pTm->uiWeekDay;
    t.tm_yday=pTm->uiYearDay;
    if(t.tm_sec<0||t.tm_sec>59||t.tm_min<0||t.tm_min>59||t.tm_hour<0||t.tm_hour>23||t.tm_mday<1||t.tm_mday>31
       ||t.tm_mon<0||t.tm_mon>11)//yday wday must be ignore
       *pTime= (tVA_OS_Time)(-1);
    else
      *pTime=mktime(&t);
  }
  return;
}


void VA_OS_GetRunTime(tVA_OS_RunTime *pRunTime)
{
  struct timespec ts;
  //clock_gettime(CLOCK_REALTIME,&ts);
  clock_gettime(CLOCK_MONOTONIC,&ts);
  if(pRunTime){
     pRunTime->uiMilliSec=ts.tv_sec*1000+ts.tv_nsec/1000000 ;
     pRunTime->uiMicroSec=(ts.tv_nsec%1000000)/1000;
  }
  return;
}

/* End of File */
