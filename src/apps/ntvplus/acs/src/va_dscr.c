/*
  FILE : stub_dscr.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_dscr.h"
#include <ngl_dsc.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <ngl_log.h>

NGL_MODULE(ACSDSCR)

typedef struct{
    DWORD handle;
    DWORD pid;
}ACSDSC;

#define NUM_DSCS 8
int sVA_DSC_INITED=0;

static ACSDSC dscs[NUM_DSCS];

static ACSDSC*GetDSC(DWORD pid){
    int i;
    NGLOG_DEBUG("GetDSC sVA_DSC_INITED=%d\r\n",sVA_DSC_INITED);
    if(0==sVA_DSC_INITED){
        sVA_DSC_INITED++;
        //bzero(dscs,sizeof(dscs));
    }
    for(i=0;i<sizeof(dscs)/sizeof(ACSDSC);i++)
        if(dscs[i].pid==pid)
           return dscs+i;
    return NULL;
}
DWORD VA_DSCR_Init(){
    int i;
    memset(dscs,0,sizeof(dscs));
    nglDscInit();
    NGLOG_DEBUG("VA_DSCR_Init");
    return kVA_OK;
}

DWORD VA_DSCR_Open( DWORD dwStbStreamHandle )
{
    ACSDSC*dsc=NULL;
    if(dwStbStreamHandle>=0x1FFF)
        return  kVA_ILLEGAL_HANDLE;
    for(int i=0;i<sizeof(dscs)/sizeof(ACSDSC);i++){
        if(0==dscs[i].pid){
           dsc=dscs+i;
           break;
        }
    }
    if(dsc){
        USHORT pids[8],pid_count;
        USHORT pid=(dwStbStreamHandle&0x1FFF);
        pid_count=GetCurrentServiceEcmPids(pids);        
        NGLOG_DEBUG("pid_count=%d",pid_count,pids[0],pids[1]);
        if(pid_count==0){
             pids[0]=pid;
             pid_count=1;
        }else{
             for(int i=0;i<pid_count;i++){
                 if(pids[i]==pid){
                     pids[i]=pids[0];
                     pids[0]=pid;
                 }
             }
        }
        dsc->handle=nglDscOpen(pids,pid_count);
        NGLOG_DEBUG("dsc=%p idx=%d nglhandle=%p pid=%d pid_count=%d",dsc,(dsc?dsc-dscs:-1),(dsc?dsc->handle:0),dwStbStreamHandle,pid_count);
    }
    if(NULL==dsc||0==dsc->handle)return kVA_ILLEGAL_HANDLE;
    return (DWORD)dsc;
}

INT VA_DSCR_Close( DWORD dwStbDescrHandle )
{
    ACSDSC*dsc=(ACSDSC*)dwStbDescrHandle;
    NGLOG_DEBUG("\tdsc=%p",dsc);
    if( (NULL==dsc) || (dsc<dscs)||(dsc>=&dscs[NUM_DSCS]) )
        return kVA_INVALID_PARAMETER;
    if(dsc->handle!=0){
         dsc->pid=0;
         nglDscClose(dsc->handle);
         dsc->handle=0;
         return kVA_OK;
    }
    if(0==dsc->handle||0==dsc->pid)
         return kVA_INVALID_PARAMETER;
    return kVA_ERROR;
}

INT VA_DSCR_SetKeys( DWORD dwStbDescrHandle,
    UINT16 uiOddKeyLength, const BYTE  *pOddKey,
    UINT16 uiEvenKeyLength, const BYTE  *pEvenKey,
    void *pUserData )
{
    int ret,key_len=8;
    ACSDSC*dsc=(ACSDSC*)dwStbDescrHandle;
    NGLOG_DEBUG("dsc=%p OddKey=%p/%d EvenKey=%p/%d",dsc,pOddKey,uiOddKeyLength,pEvenKey,uiEvenKeyLength);
    if( (NULL==dsc) || (dsc<dscs) || (dsc>=&dscs[NUM_DSCS]) )
        return kVA_INVALID_PARAMETER;
    if(NULL==pOddKey)uiOddKeyLength=0;
    if(NULL==pEvenKey)uiEvenKeyLength=0;
    if ( 0==uiOddKeyLength+uiEvenKeyLength )
        return kVA_INVALID_PARAMETER;
    ret=nglDscSetKeys(dsc->handle,pOddKey,uiOddKeyLength,pEvenKey,uiEvenKeyLength);
    switch(ret){
    case NGL_INVALID_PARA:return kVA_INVALID_PARAMETER;
    case NGL_OK:return kVA_OK;
    default:return kVA_ERROR; 
    }
}

static const char*PRINTALGO(int a){
  switch(a){
  case eSCRAMBLING_ALGO_DVB_CSA:return "eSCRAMBLING_ALGO_DVB_CSA";
  case eSCRAMBLING_ALGO_AES_128_CBC:return "eSCRAMBLING_ALGO_AES_128_CBC";
  case eSCRAMBLING_ALGO_DVB_CSA3_STANDARD_MODE:return "eSCRAMBLING_ALGO_DVB_CSA3_STANDARD_MODE";
  case eSCRAMBLING_ALGO_DVB_CSA3_MINIMALLY_ENHANCED_MODE:return "eSCRAMBLING_ALGO_DVB_CSA3_MINIMALLY_ENHANCED_MODE";
  case eSCRAMBLING_ALGO_DVB_CSA3_FULLY_ENHANCED_MODE:return "eSCRAMBLING_ALGO_DVB_CSA3_FULLY_ENHANCED_MODE";
  default:return "Unknown Algo";
  }
}

INT VA_DSCR_SetParameters(
    DWORD dwStbStreamHandle,
    const tVA_DSCR_ConfigurationParameters *pstParameters )
{
    ACSDSC*dsc=(ACSDSC*)dwStbStreamHandle;
    NGLDSC_Param param;
    NGLOG_DEBUG("\tdsc=%p",dsc);
    if( (NULL==pstParameters) || (NULL==dsc) || (dsc<dscs) || (dsc>=&dscs[NUM_DSCS]) )
        return kVA_INVALID_PARAMETER;
    NGLOG_DEBUG("dsc=%p ACS.Algo=%s ivLength=%d",dsc,
          PRINTALGO(pstParameters->eContentScramblingAlgo),pstParameters->uiIVLength);
    switch(pstParameters->eContentScramblingAlgo){
    case eSCRAMBLING_ALGO_DVB_CSA:
           param.algo=eDSC_ALGO_DVB_CSA;//dsc->attr.uc_algo = AUI_DSC_ALGO_CSA;
           if(NULL!=pstParameters->pIV||pstParameters->uiIVLength)
               return kVA_INVALID_PARAMETER;
           break;
    case eSCRAMBLING_ALGO_AES_128_CBC:
           param.algo=eDSC_ALGO_AES_128_CBC;//dsc->attr.uc_algo = AUI_DSC_ALGO_AES;
           if(NULL==pstParameters->pIV||0==pstParameters->uiIVLength)
               return kVA_INVALID_PARAMETER;
           
           //memcpy(dsc->iv,pstParameters->pIV,pstParameters->uiIVLength);
           break;
    case eSCRAMBLING_ALGO_DVB_CSA3_STANDARD_MODE:
           param.algo=eDSC_ALGO_DVB_CSA3_STANDARD_MODE;break;
    case eSCRAMBLING_ALGO_DVB_CSA3_MINIMALLY_ENHANCED_MODE:
           param.algo=eDSC_ALGO_DVB_CSA3_MINIMALLY_ENHANCED_MODE;break;
    case eSCRAMBLING_ALGO_DVB_CSA3_FULLY_ENHANCED_MODE:
           param.algo=eDSC_ALGO_DVB_CSA3_FULLY_ENHANCED_MODE;break;
    default:break;
    }
    param.pIV=pstParameters->pIV;
    param.uiIVLength=pstParameters->uiIVLength;
    nglDscSetParameters(dsc->handle,&param);
    return kVA_OK;
}

/* End of File */
