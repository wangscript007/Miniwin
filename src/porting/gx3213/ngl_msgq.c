#include <ngl_msgq.h>
#include <ngl_os.h>
#include <pthread.h>
#include <ngl_log.h>
NGL_MODULE(MSGQ);

typedef struct{
   pthread_mutex_t mutex;
   pthread_cond_t cget;
   pthread_cond_t cput;
   int msgSize;//max size of message item
   int queSize;
   int rdidx;
   int wridx;
   int msgCount;
   unsigned char*msgs;
}MSGQUEUE;

HANDLE nglMsgQCreate(int howmany, int sizepermag)
{
     if(howmany*sizepermag==0)return 0;
     MSGQUEUE*q=(MSGQUEUE*)nglMalloc(sizeof(MSGQUEUE));
     pthread_mutex_init(&q->mutex,NULL);
     pthread_condattr_t attr;
     pthread_condattr_init(&attr);
     pthread_condattr_setclock(&attr,CLOCK_MONOTONIC);
     pthread_cond_init(&q->cget,&attr);
     pthread_cond_init(&q->cput,&attr);
     pthread_condattr_destroy(&attr);
     q->queSize=howmany;
     q->msgSize=sizepermag;
     q->rdidx=q->wridx=q->msgCount=0;
     q->msgs=(unsigned char*)nglMalloc(sizepermag*howmany);
     NGLOG_VERBOSE("msgq=%p",q);
     return q;
}

DWORD nglMsgQDestroy(HANDLE msgid)
{
     MSGQUEUE*q=(MSGQUEUE*)msgid;
     pthread_cond_destroy(&q->cput);
     pthread_cond_destroy(&q->cget);
     pthread_mutex_destroy(&q->mutex);
     nglFree(q->msgs);
     nglFree(q);
     return NGL_OK;
}

DWORD nglMsgQSend(HANDLE msgid, const void* pvmsg, int msgsize, DWORD timeout)
{
    MSGQUEUE*q=(MSGQUEUE*)msgid;
    struct timespec ts;
    int rc=0;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    ts.tv_sec += timeout/1000;
    ts.tv_nsec+= (timeout%1000)*1000000l;
    if(ts.tv_nsec>=1000000000l){
         ts.tv_sec+=ts.tv_nsec/1000000000l;
         ts.tv_nsec%=1000000000l;
    }
    if(0==timeout){ts.tv_sec=ts.tv_nsec=0;}
    pthread_mutex_lock(&q->mutex);
    NGLOG_VERBOSE("rdidx=%d wridx=%d msgsize=%d/%d",q->rdidx,q->wridx,q->msgCount,q->queSize);
    if((q->wridx==q->rdidx)&&(q->queSize==q->msgCount)){
        NGLOG_VERBOSE("queue is full");
        rc=pthread_cond_timedwait(&q->cput,&q->mutex,&ts);
    }
    if(0==rc){
        memcpy(q->msgs+q->wridx*q->msgSize,pvmsg,((msgsize<=0)?q->msgSize:msgsize));
        q->wridx=(q->wridx+1)%q->queSize;
        q->msgCount++;
        pthread_cond_signal(&q->cget);
    }
    pthread_mutex_unlock(&q->mutex);
    return rc==0?NGL_OK:NGL_ERROR;
}

DWORD nglMsgQReceive(HANDLE msgid, const void* pvmsg, DWORD msgsize, DWORD timeout)
{
    MSGQUEUE*q=(MSGQUEUE*)msgid;
    struct timespec ts;
    int rc=0;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    ts.tv_sec += timeout/1000;
    ts.tv_nsec+= (timeout%1000)*1000000l;
    if(ts.tv_nsec>=1000000000l){
         ts.tv_sec+=ts.tv_nsec/1000000000l;
         ts.tv_nsec%=1000000000l;
    }
    if(0==timeout){ts.tv_sec=ts.tv_nsec=0;}
    pthread_mutex_lock(&q->mutex);
    NGLOG_VERBOSE("rdidx=%d wridx=%d msgsize=%d/%d",q->rdidx,q->wridx,q->msgCount,q->queSize);
    if((q->wridx==q->rdidx)&&(q->msgCount==0)){
        NGLOG_VERBOSE("queue is empty waiting...");
        rc=pthread_cond_timedwait(&q->cget,&q->mutex,&ts);
    }
    if(0==rc){
        memcpy(pvmsg,q->msgs+q->rdidx*q->msgSize,((msgsize<=0)?q->msgSize:msgsize));
        q->rdidx=(q->rdidx+1)%q->queSize;
        q->msgCount--;
        pthread_cond_signal(&q->cput);
    }
    pthread_mutex_unlock(&q->mutex);
    return rc==0?NGL_OK:NGL_ERROR;
}

DWORD nglMsgQGetCount(HANDLE msgid,UINT*count){
    MSGQUEUE*q=(MSGQUEUE*)msgid;
    pthread_mutex_lock(&q->mutex);
    *count=q->msgCount;
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

