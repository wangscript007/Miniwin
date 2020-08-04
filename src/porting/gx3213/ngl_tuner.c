#include "ngl_tuner.h"
#include <inttypes.h>
#include <ngl_os.h>
#include <ngl_log.h>
#include <bus/module/frontend/gxfrontend_module.h>
#include <frontend/demod.h>
#include <frontend/tuner.h>

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
   GxFrontend fe;
}NGLTUNER;

static NGLTUNER sTuners[2];
static int mv_TunerFroDevHdl = 0;
static int mv_TunerDmxModHdl = 0;

static void TunerStateProc(void*p){
   while(1){
      int rc,i,j;
      for(i=0;i<NB_TUNERS;i++){
         INT locked=-2;
         INT timeout=1000;
         while( timeout>0 && locked!=1 ){
             locked=(1==GxFrontend_QueryFrontendStatus(0))&& (1 == GxFrontend_QueryStatus(0));
             nglSleep(50);timeout-=50;
         }
         NGLOG_DEBUG_IF(rc||(sTuners[i].locked!=locked),"locked=%d/%d",locked,sTuners[i].locked);
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
    HANDLE threadId;
    int rc,i;
    int fe_handle;
    struct dev_attach dev;
    if(0!=nim_mutex)return NGL_OK;
    NGLOG_DEBUG("");
    nglCreateMutex(&nim_mutex);
    mv_TunerFroDevHdl = GxAvdev_CreateDevice(0);
    mv_TunerDmxModHdl = GxAvdev_OpenModule(mv_TunerFroDevHdl, GXAV_MOD_DEMUX, 0);

    NGLOG_DEBUG("mv_TunerFroDevHdl=%d,mv_TunerDmxModHdl=%d",mv_TunerFroDevHdl,mv_TunerDmxModHdl);
    rc=GxFrontend_Init(0);
    NGLOG_DEBUG("GxFrontend_Init=%d",rc);

    dev.demod_dev.id = 0;
    dev.demod_dev.addr = 0xa4;
    dev.demod_attach = Sirius_DVBC_ATTACH;

    dev.tuner_dev.id = 0;
    dev.tuner_dev.addr = 0xc0;
    dev.tuner_attach = MXL608_C_ATTACH;

    fe_handle = GxFrontend_IdToHandle(0);
    ioctl(fe_handle, FE_SET_DEVICE, &dev);

    NGLOG_DEBUG("fe_handle=%d",fe_handle);    
    for(i=0;i<NB_TUNERS;i++){
        sTuners[i].locked=-1;
        sTuners[i].nim_id= i;
        memset(&sTuners[i].fe,0,sizeof(GxFrontend));
        sTuners[i].fe.dev=mv_TunerFroDevHdl;
        sTuners[i].fe.demux=mv_TunerDmxModHdl;
    }

    nglCreateThread(&threadId,0,1024,TunerStateProc,NULL);
    memset(sCallBacks,0,sizeof(sCallBacks));
    return 0;
}

static int QAM2GX(NGLQamMode qam){
   switch(qam){
   case NGL_NIM_QAM16 :return DVBC_16QAM;
   case NGL_NIM_QAM32 :return DVBC_32QAM;
   case NGL_NIM_QAM64 :return DVBC_64QAM;
   case NGL_NIM_QAM128:return DVBC_128QAM;
   case NGL_NIM_QAM256:return DVBC_256QAM;
   }
}
DWORD  nglTunerLock(int tuneridx,NGLTunerParam*tp){
    int rc,TUNER_Polarity=2;
    int high_band = 0;
    GxFrontend *fe=&sTuners[tuneridx].fe;

    nglLockMutex(nim_mutex);
    sTuners[tuneridx].locked=-1;//set to -1 ,make sure atleast 1 state callback will be called
    fe->fre=tp->u.c.frequency;
    fe->symb=tp->u.c.symbol_rate;
    fe->type=FRONTEND_DVB_C;
    fe->qam=QAM2GX(tp->u.c.modulation);
    rc=GxFrontend_SetTp(fe);
    nglUnlockMutex(nim_mutex);
   
     
    NGLOG_DEBUG("GxFrontend_SetTp(%d)=%d",tp->u.c.frequency,rc);
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
    int quality=0;
    int rc=0;//aui_nim_signal_info_get(sTuners[tuneridx].hdl,&info);
    GxFrontendSignalQuality sq;
    if(state)memset(state,0,sizeof(NGLTunerState));
    if(0==GxFrontend_GetQuality(0,&sq)){
        quality=sq.snr;
        if(quality>0xFF)quality=100L*quality/0xFFFF;
        state->quality=quality;
        state->ber=sq.error_rate;
    }
    state->strength=GxFrontend_GetStrength(0);
    state->locked=(1==GxFrontend_QueryFrontendStatus(0))&& (1 == GxFrontend_QueryStatus(0));
    NGLOG_VERBOSE("locked=%d quality=%d ber=%d strength=%d",state->locked,state->quality,state->ber,state->strength);
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

