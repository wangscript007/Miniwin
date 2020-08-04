#include "ngl_tuner.h"
#include <inttypes.h>
#include <ngl_os.h>
#include <ngl_log.h>

NGL_MODULE(NIM);

#define NB_TUNERS 1
#define MAX_LISTENERS 32

typedef struct{
    int tuneridx;
    NGLTunerCallBack Callback;
    INT locked;
    void *param;
}TUNERNOTIFY;

static TUNERNOTIFY sCallBacks[MAX_LISTENERS];
static NGLMutex nim_mutex=0;

typedef struct{
   int nim_id;
   INT locked;
}NGLTUNER;

static NGLTUNER sTuners[2];

static void TunerStateProc(void*p){
   while(1){
      int rc,i,j;
      for(i=0;i<NB_TUNERS;i++){
         INT locked=-2;
         INT timeout=1000;
         while( timeout>0 && locked!=1 ){
             rc=0;locked=1;//rc=aui_nim_is_lock(sTuners[i].hdl,&locked);
             nglSleep(50);timeout-=50;
         }
         NGLOG_DEBUG_IF(rc||(sTuners[i].locked!=locked),"aui_nim_is_lock=%d locked=%d/%d",rc,locked,sTuners[i].locked);
         nglLockMutex(nim_mutex);
         for(j=0;j<MAX_LISTENERS;j++){
              if(sCallBacks[j].tuneridx!=i&&sCallBacks[j].tuneridx!=-1)continue;
              if(NULL==sCallBacks[j].Callback)continue;
              if( sTuners[i].locked!=locked )
                  sCallBacks[j].Callback(i,(locked!=0),sCallBacks[j].param);
              sCallBacks[j].locked=locked;
         }
         sTuners[i].locked=locked;
         nglUnlockMutex(nim_mutex);
      }
   } 
}

DWORD nglTunerInit(){
    DWORD i,threadId;
    if(0!=nim_mutex)return NGL_OK;
    NGLOG_DEBUG("");
    nglCreateMutex(&nim_mutex);
    for(i=0;i<NB_TUNERS;i++){
        sTuners[i].locked=-1;
        sTuners[i].nim_id= i;
    }
    nglCreateThread(&threadId,0,1024,TunerStateProc,NULL);
    memset(sCallBacks,0,sizeof(sCallBacks));
    return 0;
}


DWORD  nglTunerLock(int tuneridx,NGLTunerParam*tp){
    int rc,TUNER_Polarity=2;
    int high_band = 0;
    nglLockMutex(nim_mutex);
    sTuners[tuneridx].locked=-1;//set to -1 ,make sure atleast 1 state callback will be called
    nglUnlockMutex(nim_mutex);
    NGLOG_DEBUG("aui_nim_connect(ul_freq=%d)=%d",tp->u.c.frequency,rc);
    return NGL_OK;
}

DWORD nglTunerRegisteCBK(INT tuneridx,NGLTunerCallBack cbk,void*param){
    int i;
    nglLockMutex(nim_mutex); 
    for(i=0;i<MAX_LISTENERS;i++){
        if(sCallBacks[i].Callback==NULL){
            sCallBacks[i].tuneridx=tuneridx;
            sCallBacks[i].Callback=cbk;
            sCallBacks[i].param=param;
            sCallBacks[i].locked=-1;
            nglUnlockMutex(nim_mutex);
            return NGL_OK;
        }
    }
    nglUnlockMutex(nim_mutex);
    return NGL_ERROR;
}

DWORD nglTunerUnRegisteCBK(INT tuneridx,NGLTunerCallBack cbk){
    int i;
    for(i=0;i<MAX_LISTENERS;i++){
        if(sCallBacks[i].Callback==cbk){
            sCallBacks[i].Callback=NULL;
            sCallBacks[i].param=NULL;
            sCallBacks[i].tuneridx=-1;
            return NGL_OK;
        }
    }
    return NGL_ERROR;
}

DWORD nglTunerGetState(INT tuneridx,NGLTunerState*state){
    int rc;
    char buf[256];//aui_nim_signal_info_get will caused crash ,add  var to avoid crash
    rc=0;//aui_nim_signal_info_get(sTuners[tuneridx].hdl,&info);
    //NGLOG_DEBUG_IF(rc,"rc=%d tuner %d hdl=%p,freq=%d",rc,tuneridx,sTuners[tuneridx].hdl,info.ul_freq);
    return rc;
}

//TUNER_Polarity 水平时为1，垂直时为2 off 时为0

DWORD nglTunerSetLNB(int tuneridx,int polarity){
    return NGL_OK;
}

DWORD nglTunerSet22K(int tuneridx,INT k22){
    int ret=0;//aui_nim_set22k_onoff(sTuners[tuneridx].hdl,k22);
    NGLOG_VERBOSE("ret=%d",ret);
    return ret;
}

DWORD nglSendDiseqcCommand(int tuneridx,BYTE*Command,BYTE cmdlen,BYTE diseqc_version){
    int rc=NGL_ERROR;
    NGLOG_VERBOSE("cmdlen=%d",cmdlen);
    return rc==0?NGL_OK:NGL_ERROR;
}

