/*
  FILE : stub_dmx.c
  PURPOSE: This file is a stub for linking tests.
*/
#include <stdio.h>
#include "ngl_dmx.h"
#include <strings.h>
#include <av/gxav_demux_propertytypes.h>
#include <av/gxav_common.h>
#include <gxcore.h>
#include <bus/gxavdev.h>
#include <sys/time.h>
#define MASK_LEN 16 
#define MAX_CHANNEL 64
#define MAX_FILTER  128 
#define UNUSED_PID 0xFFFF
#include <ngl_log.h>
#include <ngl_os.h>

NGL_MODULE(DMX);;

#define CHECKDMX(x) {if((x)!=AUI_RTN_SUCCESS)NGLOG_VERBOSE("%s:%d %s=%d\n",__FUNCTION__,__LINE__,#x,(x));}
typedef struct{
   int dev;
   int hdl;
}GXDMX;
typedef struct {
  unsigned short pid;
  int num_filt;
  int num_started;
  int started;
  int dmxid;
  GxDemuxProperty_Slot prop;
}DMXCHANNEL;

#define TSBUF_SIZE 188*7
typedef struct{
    DMXCHANNEL*ch;
    FILTER_NOTIFY CallBack;
    void*userdata; 
    int started;
    BYTE mask[MASK_LEN+2];
    BYTE value[MASK_LEN+2];
    BYTE reverse[MASK_LEN+2];
    BYTE*tsBuffer;
    GxDemuxProperty_Filter prop;
}NGLDMXFILTER;
static HANDLE mtx_dmx=0;
static GXDMX sDMX[2];
static DMXCHANNEL Channels[MAX_CHANNEL];
static NGLDMXFILTER  Filters[MAX_FILTER];
#define USE_OSMUTEX 
#ifndef USE_OSMUTEX
#define LOCK_DMX    GxCore_MutexLock(mtx_dmx)
#define UNLOCK_DMX  GxCore_MutexUnlock(mtx_dmx)
#else
#define LOCK_DMX    nglLockMutex(mtx_dmx) 
#define UNLOCK_DMX  nglUnlockMutex(mtx_dmx)
#endif
static DMXCHANNEL*GetChannel(int pid){
   int i;
   for(i=0;i<MAX_CHANNEL;i++){
       DMXCHANNEL*ch=Channels+i;
       if(ch->pid==pid){
            return Channels+i;
       }
   }
   return NULL;
}
static DMXCHANNEL*GetFreeChannel(){
  int i;
  for(i=0;i<MAX_CHANNEL;i++)
    if(UNUSED_PID==Channels[i].pid){
       return Channels+i;
    }
  return NULL;
}

static NGLDMXFILTER*GetFilter(DMXCHANNEL*ch){
    int i;
    for(i=0;i<MAX_FILTER;i++)
        if(Filters[i].ch==NULL){
            Filters[i].ch=ch;
            return Filters+i;
        }
    return NULL;
}
#define CHECKFILTER(flt) {if((flt<Filters)||(flt>=&Filters[MAX_FILTER]))return NGL_INVALID_PARA;}
#define SECLEN(sec) (((sec)[1]&0x0F)<<8|(sec)[2])
static void DMXReadProc(void*p){
#define BUFFER_SIZE (64*1024)
    char *buffer=malloc(BUFFER_SIZE);
    GxDemuxProperty_FilterRead filt_read={0};
    filt_read.buffer = buffer;
    filt_read.max_size = BUFFER_SIZE;
    while(1){
        int i,pos;
        int rc,read_bytes=0; 
        for(i=0;i<MAX_FILTER;i++){
            int64_t cur;
            LOCK_DMX;
            NGLDMXFILTER*flt=Filters+i;
            if(!flt->started){
                UNLOCK_DMX;
                continue;
            }
            filt_read.filter_id =flt->prop.filter_id;
            filt_read.buffer=buffer;
            filt_read.read_size=0;
            rc=GxAVGetProperty(sDMX[flt->ch->dmxid].dev,sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_FilterRead,
                         (void*)&filt_read, sizeof(GxDemuxProperty_FilterRead));
            read_bytes+=filt_read.read_size;
            for(pos=0;pos<filt_read.read_size;){
                int seclen=SECLEN(buffer+pos)+3;
                if(flt->CallBack) flt->CallBack(flt,buffer+pos,seclen,flt->userdata);
                pos+=seclen;
            }
            UNLOCK_DMX;
        }
        if(read_bytes==0)GxCore_ThreadDelay(10);
    }
}

DWORD nglDmxInit(){
    int i,rc,dev;
    HANDLE thid;
    if(mtx_dmx)return 0;
#ifndef USE_OSMUTEX
    GxCore_MutexCreate(&mtx_dmx);
#else
    nglCreateMutex(&mtx_dmx);
#endif
    dev=GxAvdev_CreateDevice(0);
    for(i=0;i<2;i++){
        GxDemuxProperty_ConfigDemux cfg;
        sDMX[i].dev=dev;
        sDMX[i].hdl=GxAvdev_OpenModule(dev, GXAV_MOD_DEMUX, i);
        cfg.source = i;
        cfg.ts_select = FRONTEND;
        cfg.stream_mode = DEMUX_PARALLEL;
        cfg.time_gate = 0xf;
        cfg.byt_cnt_err_gate = 0x03;
        cfg.sync_loss_gate = 0x03;
        cfg.sync_lock_gate = 0x03;
        rc=GxAVSetProperty(dev,sDMX[i].hdl,GxDemuxPropertyID_Config,&cfg,sizeof(GxDemuxProperty_ConfigDemux));
        NGLOG_VERBOSE("ConfigDMX %d =%d",sDMX[i].hdl,i);
    }
    memset(Filters,0,sizeof(NGLDMXFILTER)*MAX_FILTER);
    for(i=0;i<MAX_CHANNEL;i++){
        Channels[i].pid=UNUSED_PID;
        Channels[i].num_filt=0;
        Channels[i].num_started=0;
    }
    NGLOG_DEBUG("DMX[0]=%p DMX[1]=%p",sDMX[0].hdl,sDMX[1].hdl);
    nglCreateThread(&thid,0,0,DMXReadProc,NULL);
    return 0;
}

HANDLE nglAllocateSectionFilter(INT id,WORD  wPid,FILTER_NOTIFY cbk,void*userdata,DMX_TYPE dmxtype)
{
    int rc=0;
    DMXCHANNEL*ch=GetChannel(wPid);
    NGLDMXFILTER*flt=NULL;
    if(wPid>=0x1FFF||cbk==NULL)return NULL;
    LOCK_DMX;
    if(ch==NULL){
        ch=GetFreeChannel();
        memset(&ch->prop, 0, sizeof(GxDemuxProperty_Slot));
        
        ch->prop.pid=ch->pid=wPid;
        ch->num_filt=0;
        ch->started=0;
        switch(dmxtype){
        case DMX_SECTION:ch->prop.type = DEMUX_SLOT_PSI; break;
        case DMX_TS     :ch->prop.type = DEMUX_SLOT_TS ; break;
        case DMX_PES    :ch->prop.type = DEMUX_SLOT_PES; break;
        }
        rc=GxAVGetProperty(sDMX[id].dev,sDMX[id].hdl,GxDemuxPropertyID_SlotAlloc,(void*)&ch->prop, sizeof(GxDemuxProperty_Slot));
        NGLOG_VERBOSE_IF(rc,"GxDemux_SlotAlloc(pid %d)=%d",wPid,rc);

        if(rc<0)goto destroyslot; 
        ch->pid=wPid;
        ch->prop.flags= (DMX_REPEAT_MODE | DMX_AVOUT_EN);
        rc=GxAVSetProperty(sDMX[id].dev,sDMX[id].hdl, GxDemuxPropertyID_SlotConfig,(void*)&ch->prop, sizeof(GxDemuxProperty_Slot));
        NGLOG_VERBOSE_IF(rc,"GxDemux_SlotConfig=%d slotid=%d",rc,ch->prop.slot_id);
    }
    flt=GetFilter(ch);
    flt->started=0;
    memset(&flt->prop, 0, sizeof(GxDemuxProperty_Filter));
    
    if(rc>=0){
        flt->prop.slot_id = ch->prop.slot_id;
        rc=GxAVGetProperty(sDMX[id].dev,sDMX[id].hdl,GxDemuxPropertyID_FilterAlloc,(void*)&flt->prop, sizeof(GxDemuxProperty_Filter));
        NGLOG_VERBOSE_IF(rc,"GxDemux_FilterAlloc=%d slotid=%d",rc,flt->prop.slot_id); 
    }
    flt->ch=ch;
    ch->num_filt++;
    NGLOG_VERBOSE("Alloced flt:%p  pid:%d slot_id=%d filter_id=%d",flt,wPid,ch->prop.slot_id,flt->prop.filter_id);
    flt->CallBack=cbk;
    flt->userdata=userdata;
    UNLOCK_DMX;
    return flt;

 destroyfilt:
    flt->ch=NULL;//recycle filter handle resource
    
    GxAVSetProperty(sDMX[id].dev,sDMX[id].hdl,GxDemuxPropertyID_FilterFree,(void*)&flt->prop, sizeof(GxDemuxProperty_Filter));
 destroyslot:
    if(ch->num_filt==0){
       GxAVSetProperty(sDMX[id].dev,sDMX[id].hdl, GxDemuxPropertyID_SlotFree,(void*)&ch->prop, sizeof(GxDemuxProperty_Slot));
       NGLOG_VERBOSE_IF(rc,"GxDemux_SlotFree(%p)=%d ",flt,rc);
    }
    UNLOCK_DMX;
    return NULL;
}

INT nglGetFilterPid( HANDLE dwStbFilterHandle){
    NGLDMXFILTER*flt=(NGLDMXFILTER*)dwStbFilterHandle;
    return (flt&&flt->ch)?flt->ch->pid:-1;
}

INT nglFreeSectionFilter( HANDLE dwStbFilterHandle )
{
    int rc;
    NGLDMXFILTER*flt=(NGLDMXFILTER*)dwStbFilterHandle; 

    CHECKFILTER(flt);

    nglStopSectionFilter(dwStbFilterHandle);
    LOCK_DMX;
    if(flt->ch){
        rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev, sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_FilterFree,
               (void*)&flt->prop, sizeof(GxDemuxProperty_Filter));
        NGLOG_VERBOSE("GxDemux_FilterFree(%p)=%d",flt,rc);
        flt->ch->num_filt--;
        if(flt->ch->num_filt==0){
            rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev,sDMX[flt->ch->dmxid].hdl,GxDemuxPropertyID_SlotFree,
                    (void*)&flt->ch->prop, sizeof(GxDemuxProperty_Slot));
            NGLOG_VERBOSE("GxDemux_SlotFree(%p)=%d slotid:%d filtid:%d",flt,rc,flt->ch->prop.slot_id,flt->prop.filter_id);
            flt->ch->pid=UNUSED_PID;
        }
        flt->ch=NULL;
    }
    UNLOCK_DMX;
    return NGL_OK;
}

INT nglSetSectionFilterParameters( HANDLE dwStbFilterHandle, BYTE *pMask, BYTE *pValue,UINT uiLength)
{
    int i,rc;
    NGLDMXFILTER*flt=(NGLDMXFILTER*)dwStbFilterHandle;
    CHECKFILTER(flt);
    if((uiLength&&(pMask==NULL||pValue==NULL))||(uiLength>MAX_FILTER_DEPTH))
       return NGL_INVALID_PARA;
    LOCK_DMX;
    flt->prop.depth=uiLength+(uiLength>1?2:0);
    flt->prop.key[1].mask=flt->prop.key[1].value=0;
    flt->prop.key[2].mask=flt->prop.key[2].value=0;
    for(i=0;i<uiLength;i++){
        flt->prop.key[i==0?i:i+2].value=pValue[i];
        flt->prop.key[i==0?i:i+2].mask=pMask[i];
    }
    flt->prop.flags|=DMX_EQ;/*no DMX_EQ is onshot mode*/
    rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev, sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_FilterConfig,
         (void*)&flt->prop, sizeof(GxDemuxProperty_Filter)); 
    NGLOG_VERBOSE("GxDemux_FilterConfig(%p)=%d mask_len=%d",flt,rc,uiLength);
    UNLOCK_DMX;
    return NGL_OK;
}

INT nglFilterSetCRC(HANDLE hfilter,BOOL enable){
    int rc;
    NGLDMXFILTER*flt=(NGLDMXFILTER*)hfilter;
    CHECKFILTER(flt);
    LOCK_DMX;
    if(enable)
        flt->prop.flags |= (DMX_CRC_IRQ);
    else
        flt->prop.flags &= (~DMX_CRC_IRQ);
    rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev, sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_FilterConfig,
         (void*)&flt->prop, sizeof(GxDemuxProperty_Filter));
    UNLOCK_DMX;
    return NGL_OK;
}

INT nglStartSectionFilter(HANDLE  dwStbFilterHandle)
{
    int rc;
    NGLDMXFILTER*flt=(NGLDMXFILTER*)dwStbFilterHandle;
    CHECKFILTER(flt);

    LOCK_DMX;    
    if(!flt->started){
        if(flt->ch->started==0){
            rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev, sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_SlotEnable,
                  (void*)&flt->ch->prop,sizeof(GxDemuxProperty_Slot));
            NGLOG_VERBOSE("GxDemux_SlotEnable(%p)=%d",flt,rc);
            flt->ch->started++;
        }
        rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev,sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_FilterEnable,
                            (void*)&flt->prop, sizeof(GxDemuxProperty_Filter));
        NGLOG_VERBOSE("GxDemux_FilterEnable(%p)=%d",flt,rc);
        flt->started=1;
        flt->ch->num_started++;
    }
    UNLOCK_DMX;
    return NGL_OK;
}

/**AllocSectionFilter without FreeSectionFilter ?*/
INT nglStopSectionFilter(HANDLE dwStbFilterHandle)
{
    int rc;
    NGLDMXFILTER*flt=(NGLDMXFILTER*)dwStbFilterHandle;
    CHECKFILTER(flt);
    LOCK_DMX;
    if(flt->started){
        rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev,sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_FilterDisable,
                            (void*)&flt->prop, sizeof(GxDemuxProperty_Filter));
        NGLOG_VERBOSE("GxDemux_FilterDisable(%p)=%d",flt,rc);
        flt->started--;
        flt->ch->num_started--;
        if(flt->ch->num_started==0){
            flt->ch->started--;
            rc=GxAVSetProperty(sDMX[flt->ch->dmxid].dev, sDMX[flt->ch->dmxid].hdl, GxDemuxPropertyID_SlotDisable,
                (void*)&flt->ch->prop,sizeof(GxDemuxProperty_Slot));
            NGLOG_VERBOSE("GxDemux_SlotDisable(%p)=%d",flt,rc);
        }
    }
    UNLOCK_DMX;
    return NGL_OK;
}

/* End of File */
