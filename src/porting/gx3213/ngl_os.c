/*
  FILE : stub_os.c
  PURPOSE: This file is a stub for linking tests.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ngl_os.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include <ngl_log.h>

NGL_MODULE(OS);

typedef struct{
   pthread_mutex_t mtx;
   pthread_cond_t cond;
   UINT nsem;
}SEM;

INT nglCreateSemaphore(NGLSemaphore * const pSemaphore, UINT uiValue )
{
  if(pSemaphore){
      SEM*s=(SEM*)nglMalloc(sizeof(SEM));
      pthread_mutex_init(&s->mtx,NULL);
      pthread_condattr_t attr; 
      pthread_condattr_init(&attr);
      pthread_condattr_setclock(&attr,CLOCK_MONOTONIC);
      pthread_cond_init(&s->cond,&attr);
      pthread_condattr_destroy(&attr);
      s->nsem=uiValue;
      *pSemaphore=s;
  }
  return NULL==pSemaphore?NGL_ERROR:NGL_OK;
}

INT nglDeleteSemaphore(NGLSemaphore pSemaphore)
{
  SEM*s=(SEM*)pSemaphore;
  if(NULL==s)
     return NGL_ERROR;
  pthread_mutex_destroy(&s->mtx);
  pthread_cond_destroy(&s->cond);
  nglFree(s);
  return NGL_OK;
}

INT nglAcquireSemaphore(NGLSemaphore pSemaphore, UINT uiDuration )
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC,&ts);
  ts.tv_sec+=uiDuration/1000;
  ts.tv_nsec+=(uiDuration%1000)*1000000l;

  if(ts.tv_nsec>=1000000000l){
      ts.tv_sec+=ts.tv_nsec/1000000000l;
      ts.tv_nsec%=1000000000l;
  }

  SEM*s=(SEM*)pSemaphore;
  if(NULL==s)
      return NGL_ERROR;
  if(0==uiDuration){ts.tv_sec=ts.tv_nsec=0;}
  pthread_mutex_lock(&s->mtx);
  if(s->nsem==0)
     pthread_cond_timedwait(&s->cond,&s->mtx,&ts);
  s->nsem--;
  pthread_mutex_unlock(&s->mtx);
  return NGL_OK;
}


INT nglReleaseSemaphore(NGLSemaphore pSemaphore )
{
    SEM*s=(SEM*)pSemaphore;
    NGLOG_DEBUG("%p",pSemaphore);
    if(NULL==s)
       return NGL_ERROR;
    pthread_mutex_lock(&s->mtx);
    if(s->nsem==0)
        pthread_cond_broadcast(&s->cond);
    s->nsem++;
    pthread_mutex_unlock(&s->mtx);
    return NGL_OK;
}


INT nglCreateMutex(NGLMutex * const pMutex)
{
    if(NULL==pMutex)return NGL_INVALID_PARA;
    pthread_mutexattr_t attr;
    pthread_mutex_t *mtx=(pthread_mutex_t*)nglMalloc(sizeof(pthread_mutex_t));;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);//PTHREAD_PROCESS_PRIVATE PTHREAD_PROCESS_SHARED
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);//PTHREAD_MUTEX_RECURSIVE);//PTHREAD_MUTEX_ERRORCHECK
    pthread_mutex_init(mtx,&attr);
    pthread_mutexattr_destroy(&attr);
    *pMutex=(NGLMutex*)mtx;
    return NGL_OK;
}


INT nglDeleteMutex(NGLMutex pMutex)
{
    if(NULL==pMutex)
       return NGL_INVALID_PARA;
    int rc=pthread_mutex_destroy((pthread_mutex_t*)pMutex);
    nglFree((void*)pMutex);
    return NGL_OK;
}


INT nglLockMutex(NGLMutex pMutex)
{
   if(NULL==pMutex)
       return NGL_INVALID_PARA;
   return 0==pthread_mutex_lock((pthread_mutex_t*)pMutex)?NGL_OK:NGL_ERROR;
}


INT nglUnlockMutex(NGLMutex pMutex)
{
    if(NULL==pMutex)
       return NGL_INVALID_PARA;
    return 0==pthread_mutex_unlock((pthread_mutex_t*)pMutex)?NGL_OK:NGL_ERROR;
}

typedef struct{
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    pthread_condattr_t attr;
    BOOL triggered;
    BOOL autoreset;
}EVENT;

HANDLE nglCreateEvent(BOOL state,BOOL autoreset )
{
    EVENT*e=(EVENT*)nglMalloc(sizeof(EVENT));
    pthread_mutex_init(&e->mtx,NULL);
    pthread_condattr_init(&e->attr);
    pthread_condattr_setclock(&e->attr,CLOCK_MONOTONIC);
    pthread_cond_init(&e->cond,&e->attr);
    e->triggered=state;
    e->autoreset=autoreset;
    return (HANDLE)e;
}


DWORD nglDestroyEvent(HANDLE eventid)
{
    EVENT*e=(EVENT*)eventid;
    if(NULL==e)return NGL_INVALID_PARA;
    pthread_cond_destroy(&e->cond);
    pthread_mutex_destroy(&e->mtx);
    pthread_condattr_destroy(&e->attr);
    nglFree(e);
    return NGL_OK;
}

DWORD nglResetEvent(HANDLE eventid)
{
     EVENT*e=(EVENT*)eventid;
     if(NULL==e)return NGL_INVALID_PARA;
     pthread_mutex_lock(&e->mtx);
     e->triggered=FALSE;
     pthread_mutex_unlock(&e->mtx);
     return 0;
}

DWORD nglSetEvent(HANDLE eventid)
{
    EVENT*e=(EVENT*)eventid;
    pthread_mutex_lock(&e->mtx);
    pthread_cond_broadcast(&e->cond);
    e->triggered=TRUE;
    pthread_mutex_unlock(&e->mtx);
}

DWORD nglWaitEvent(HANDLE eventid, DWORD timeout)
{
    EVENT*e=(EVENT*)eventid;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    
    ts.tv_sec+=timeout/1000;
    ts.tv_nsec+=(timeout%1000)*1000000l;

    if(ts.tv_nsec>=1000000000l){
         ts.tv_sec+=ts.tv_nsec/1000000000l;
         ts.tv_nsec%=1000000000l;
    }
    else if(timeout==0)ts.tv_sec=ts.tv_nsec=0;
    
    pthread_mutex_lock(&e->mtx); 
    while(!e->triggered){
         if(pthread_cond_timedwait(&e->cond,&e->mtx,&ts)){
               pthread_mutex_unlock(&e->mtx);
               return NGL_ERROR;
         }
    }
    if(e->autoreset)e->triggered=FALSE;
    pthread_mutex_unlock(&e->mtx);
    return NGL_OK;
}

void nglCreateThread(HANDLE *threadid,int p,int stacksize,THREAD_PROC proc,void*param){
    pthread_t thid;
    pthread_create(&thid,NULL,(void * (*)(void *))proc,param);
    *threadid=thid;
}

void nglDeleteThread(HANDLE threadid){
    
}

void nglJoinThread(HANDLE threadid){
    pthread_join((pthread_t)threadid,NULL);
}

void *nglMalloc(UINT uiSize){
    return uiSize==0?NULL:malloc(uiSize);
}

void* nglAlloc ( UINT uiSize){
    return (void*)calloc(uiSize,1);
}

void* nglRealloc(void*p,UINT size){
    return (void*)realloc(p,size);
}

void nglFree(void * ptr){
    if(ptr!=NULL)
       free(ptr);
    return;
}


INT nglPrintf(const char *pFormat, ...){
    va_list args;
    va_start(args, pFormat);
    vprintf(pFormat,args);
    va_end(args);
    return 0;
}

void nglSleep( UINT uiDuration ){
    usleep(uiDuration*1000);
    return;
}

/* End of File */
