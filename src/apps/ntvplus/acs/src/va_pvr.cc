/*
  FILE : stub_pvr.c
  PURPOSE: This file is a stub for linking tests.
*/
#include <stdio.h>
extern "C"{
#include <va_pvr.h>
}
#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_pvr.h>
#include <va_os.h>
#include <aui_common.h>
#include <dvbepg.h>
NGL_MODULE(ACSPVR)
#define TAG_CA_DESCRIPTOR 9
#define MAX_PVR 8
#define MAX_ECM 8
typedef struct{
   WORD ecmpid;
   DWORD ecmhandler;
   FILE*ecmfile;
}ECMPVR;
typedef struct{
   tVA_PVR_RecordType recordType;//ePERSISTENT, eTIMESHIFT 
   WORD num_ecm;
   WORD ecm_opened;
   ECMPVR ecms[MAX_ECM];
   DWORD nglpvr;
}PVR,SVCPVR;
static PVR sPvrs[MAX_PVR];

static PVR*GetPVRByECMPID(WORD pid){
   for(int i=0;i<MAX_PVR;i++){
       PVR*p=sPvrs+i;
       for(int j=0;j<p->num_ecm;j++){
           if(p->ecms[j].ecmpid==pid)return p;
       }
   }
   return NULL;
}
static ECMPVR*GetECMPVR(PVR*pvr,WORD pid){
   for(int i=0;i<pvr->num_ecm;i++)
      if(pvr->ecms[i].ecmpid==pid)
         return pvr->ecms+i;
   return NULL;
}
static int ETYPE2AUI(int tp){
   switch(tp){
   case 1:
   case 2:return AUI_DECV_FORMAT_MPEG;
   case 3:return AUI_DECA_STREAM_TYPE_MPEG1;
   case 4:return AUI_DECA_STREAM_TYPE_MPEG2;
   }
}


INT GetPvrParamByEcmPID(NGLPVR_RECORD_PARAM*param){
    int i,audidx,rc;
    WORD pcr,num_ecm=0,ecmpids[16]; 
    BYTE pmtbuffer[1024];
    SERVICELOCATOR cur;    
    ELEMENTSTREAM es[16];
    DtvGetCurrentService(&cur);
    DtvGetServicePmt(&cur,pmtbuffer);
    PMT pmt(pmtbuffer,false);
    rc=pmt.getElements(es,false);
    memset(param,0,sizeof(NGLPVR_RECORD_PARAM));
    num_ecm+=pmt.getCAECM(NULL,ecmpids+num_ecm);;
    param->pcr_pid=pmt.pcrPid();
    for(i=0,audidx=0;i<rc;i++){
        num_ecm+=es[i].getCAECM(NULL,ecmpids+num_ecm);
        switch(es[i].stream_type){
        case 1:
        case 2: param->video_pid=es[i].pid;
                param->video_type=0;//ETYPE2AUI(es[i].type);
                break;
        case 3:
        case 4: param->audio_pids[audidx]=es[i].pid;
                param->audio_types[audidx++]=ETYPE2AUI(es[i].stream_type);
                break;
        }
    }
    memcpy(param->ecm_pids,ecmpids,num_ecm*sizeof(USHORT));
    return num_ecm;
}

DWORD VA_PVR_Start( DWORD  dwAcsId,int eRecordType,WORD sid){
    SERVICELOCATOR cur;
    BYTE pmtbuffer[1024];
    DtvGetCurrentService(&cur);
    DtvGetServicePmt(&cur,pmtbuffer);
    PMT pmt(pmtbuffer,false);
    WORD pids[8]={0x1FFF};
    int rc=pmt.getCAECM(NULL,pids);
    NGLOG_DEBUG("service=%d.%d.%d ecmpid=%d",cur.netid,cur.tsid,cur.sid,pids[0]);
    if(rc>0)
         return VA_PVR_OpenEcmChannel(dwAcsId,(tVA_PVR_RecordType)eRecordType,pids[0]);
    return 0;
}

DWORD VA_PVR_OpenEcmChannel ( DWORD  dwAcsId,tVA_PVR_RecordType eRecordType,WORD wEcmChannelPid )
{//ecm is from PMT's CA_Descriptor
    int i,rc;
    static int indexed=0;
    char fname[256];
    PVR*pvr=NULL;
    if(dwAcsId>= kVA_SETUP_NBMAX_ACS || wEcmChannelPid>=0x1FFF)
        return kVA_ILLEGAL_HANDLE;
    if(eRecordType!=ePERSISTENT&&eRecordType!=eTIMESHIFT)
        return kVA_ILLEGAL_HANDLE;
    if((pvr=GetPVRByECMPID(wEcmChannelPid))==NULL){
        for(i=0;i<MAX_PVR;i++){
            if(sPvrs[i].ecm_opened==0){
                pvr=sPvrs+i;
                break;
            }
        }
    
        tVA_OS_Time tnow;
        tVA_OS_Tm tm;
        VA_OS_GetTime(&tnow);
        VA_OS_TimeToTm(&tnow,&tm);
        sprintf(fname,"VA,PVR-%d,%d-%d-%d,%02d,%02d.trp",tm.uiHour,tm.uiYear,tm.uiMonth,tm.uiMonthDay,tm.uiMin,tm.uiSec);
        NGLPVR_RECORD_PARAM param;
        memset(&param,0,sizeof(NGLPVR_RECORD_PARAM));
        pvr->num_ecm=GetPvrParamByEcmPID(&param);
        if(0==pvr->num_ecm){
             NGLOG_ERROR("invalid ECMPID 0x%x/%d",wEcmChannelPid,wEcmChannelPid);
             return kVA_ILLEGAL_HANDLE;
        }
        for(int i=0;i<pvr->num_ecm;i++)
            pvr->ecms[i].ecmpid=param.ecm_pids[i];
        pvr->nglpvr=nglPvrRecordOpen(fname,&param);
        if(pvr->nglpvr!=0){
            param.recordMode=eRecordType;
            pvr->ecm_opened++;
            NGLOG_DEBUG("pvr=%p acsid=%d eRecordType=%d wEcmChannelPid=%d nglhandle=%p",pvr,dwAcsId,eRecordType,wEcmChannelPid,pvr->nglpvr);
            return wEcmChannelPid;
        }
    }else{
        for(int i=0;i<pvr->num_ecm;i++){
           if(pvr->ecms[i].ecmpid==wEcmChannelPid){
               pvr->ecm_opened++;
               return wEcmChannelPid;
           }
        }
    }
    return  kVA_ILLEGAL_HANDLE;
}


INT VA_PVR_CloseEcmChannel ( DWORD   dwStbEcmChannelHandle )
{
    PVR*pvr=(PVR*)GetPVRByECMPID(dwStbEcmChannelHandle);
    if(pvr<sPvrs||pvr>=&sPvrs[MAX_PVR]){
        return kVA_INVALID_PARAMETER;
    }
    if(0==pvr->nglpvr){
        return kVA_INVALID_PARAMETER;
    }
    ECMPVR*ecm=GetECMPVR(pvr,dwStbEcmChannelHandle);
    if(ecm){
        if(ecm->ecmfile){
            fclose(ecm->ecmfile);
            ecm->ecmfile=NULL;
        }
        ecm->ecmpid=0;
        pvr->ecm_opened--;
    }
    NGLOG_DEBUG(" pvr=%p nglhandle=%p",pvr,pvr->nglpvr);
    if(0>= pvr->ecm_opened){
        nglPvrRecordClose(pvr->nglpvr);
        pvr->nglpvr=0;
        pvr->ecm_opened=0;
        pvr->num_ecm=0;
    }
    return 0;
}

extern "C" void nglGetPvrPath(DWORD handler,char*path);

INT VA_PVR_RecordEcm (  DWORD        dwStbEcmChannelHandle,
                        UINT32       uiEcmLength,
                        BYTE *       pEcm,
                        void *       pUserData )
{
    PVR*pvr=(PVR*)GetPVRByECMPID(dwStbEcmChannelHandle);
    char path[512],pvrpath[256];

    NGLOG_DEBUG("pvr=%p pEcm=%p uiEcmLength=%d",pvr,pEcm,uiEcmLength);
    
    if(pvr<sPvrs||pvr>=&sPvrs[MAX_PVR] ||NULL==pEcm||0==uiEcmLength||kVA_PVR_ECM_MAX_SIZE<uiEcmLength)
        return kVA_INVALID_PARAMETER;

    NGLOG_DUMP("==ECMData",pEcm,uiEcmLength/8);
    ECMPVR*ecm=GetECMPVR(pvr,dwStbEcmChannelHandle);
    nglGetPvrPath(pvr->nglpvr,pvrpath);
    sprintf(path,"%s/acs_ecm_%d.dat",pvrpath,dwStbEcmChannelHandle);
    if(NULL==ecm->ecmfile)ecm->ecmfile=fopen(path,"wb");
    NGLOG_DEBUG("ecmpath=%s ecm=%p file=%p",path,ecm,ecm->ecmfile);
    if(ecm->ecmfile){
        fwrite(pEcm,1,uiEcmLength,ecm->ecmfile);
    }
    return NGL_OK;
}

static BYTE PVR_MetaData[kVA_PVR_METADATA_MAX_SIZE];
INT VA_PVR_WriteMetadata (  DWORD    dwAcsId,
                            UINT32   uiMetadataLength,
                            BYTE *   pMetadata )
{//metadata<--->service ?
    char path[512];
    NGLOG_DEBUG("acsid=%d pMetadata=%p uiMetadataLength=%d",dwAcsId,pMetadata,uiMetadataLength);
    if(dwAcsId>= kVA_SETUP_NBMAX_ACS||kVA_PVR_METADATA_MAX_SIZE<uiMetadataLength||0==uiMetadataLength|| NULL==pMetadata)
        return kVA_INVALID_PARAMETER;
    sprintf(path,"/tmp/meta%d.dat",dwAcsId);
    FILE*f=fopen(path,"wb");
    if(f){
        fwrite(pMetadata,1,uiMetadataLength,f);
        fclose(f);
    }
    NGLOG_DEBUG("metapath=%s file=%p",path,f);
    memcpy(PVR_MetaData,pMetadata,uiMetadataLength);
    NGLOG_DUMP("MetaData",pMetadata,uiMetadataLength);
    return 0;
}


INT VA_PVR_ReadMetadata (   DWORD    dwAcsId,
                            UINT32   uiMetadataLength,
                            BYTE *   pMetadata )
{
    char path[512];
    NGLOG_DEBUG("acsid=%d pMetadata=%p uiMetadataLength=%d kVA_PVR_METADATA_MAX_SIZE=%d kVA_SETUP_NBMAX_ACS=%d",dwAcsId,pMetadata,
            uiMetadataLength,kVA_PVR_METADATA_MAX_SIZE,kVA_SETUP_NBMAX_ACS);
    if((dwAcsId>= kVA_SETUP_NBMAX_ACS)||(kVA_PVR_METADATA_MAX_SIZE<uiMetadataLength)||(0==uiMetadataLength|| NULL==pMetadata))
        return kVA_INVALID_PARAMETER;
    sprintf(path,"/tmp/meta%d.dat",dwAcsId);
    FILE*f=fopen(path,"rb");
    if(f){
        fread(pMetadata,1,uiMetadataLength,f);    
        fclose(f);
    }
    NGLOG_DEBUG("metapath=%s file=%p",path,f);
    memcpy(pMetadata,PVR_MetaData,uiMetadataLength);
    return 0;
}


/* End of File */
