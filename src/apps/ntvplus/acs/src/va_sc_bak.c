/*
  FILE : stub_sc.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_sc.h"
#include "ngl_smc.h"
#include <stdio.h>
#include <sys/time.h>
#include <ngl_msgq.h>
#include <va_os.h>
#include <ngl_log.h>

NGL_MODULE(ACSSMC)

typedef struct{
  DWORD hdl;
  INT locked;
  INT state;
  INT slot;
}SMCCARD;

SMCCARD CARDS[1]={NULL,0,0};

#define CHECKSLOT(s) {if(s<0||s>=sizeof(CARDS)/sizeof(SMCCARD))return kVA_INVALID_PARAMETER;}

static const char*stateNames[]={"Extracted","Inserted","Ready"};
static void CardStateProc(int slot,tNGL_SMC_State state,void*userdata)//state :plugged in or plugged out
{
   CARDS[slot].state=(state==0?eCARD_EXTRACTED:eCARD_INSERTED);
   NGLOG_DEBUG("slot =%d state=%s",slot,stateNames[state]);
   if(CARDS[slot].state==eCARD_INSERTED)
      VA_SC_CardInserted(slot);
   else
      VA_SC_CardExtracted(slot);
}

static void ResetCBK(DWORD slot,const BYTE*atr,INT len,BYTE pps){
   NGLOG_DEBUG(">>slot=%d atr=%p len=%d",slot,atr,len);
   if(atr&&len>0){
       NGLOG_DEBUG(">>>>>>before VA_SC_ResetDone");
       VA_SC_ResetDone(slot,atr,len,pps);
       NGLOG_DEBUG("<<<<<<<");
       CARDS[slot].state=eCARD_READY;
   }else
       VA_SC_ResetFailed(slot);
   NGLOG_DEBUG("end.");
}

static void va_smcinit(){
    static int va_smc_inited=0;
    if(0==va_smc_inited){
        nglSmcInit();
        nglSmcRegisterNotify(-1,CardStateProc,NULL); 
        va_smc_inited++;
        NGLOG_DEBUG("va_smcinit=====");
    }
}

INT VA_SC_Reset( DWORD dwScSlot, UINT8 uiPPSNegotiationValue )
{ 
   va_smcinit();
   NGLOG_DEBUG("dwScSlot =%d pps=%d",dwScSlot,uiPPSNegotiationValue);
   CHECKSLOT(dwScSlot);
   if(eCARD_EXTRACTED==CARDS[dwScSlot].state){
        VA_SC_ResetFailed(dwScSlot);
        NGLOG_DEBUG("kVA_INSERT_CARD_FIRST");
        return kVA_INSERT_CARD_FIRST;
   }
   if(eCARD_READY==CARDS[dwScSlot].state){
        VA_SC_ResetFailed(dwScSlot);
        NGLOG_DEBUG("kVA_RESOURCE_BUSY");
        return kVA_RESOURCE_BUSY;
   }
   return nglSmcReset(dwScSlot,uiPPSNegotiationValue,ResetCBK);
}

INT VA_SC_GetState(DWORD dwScSlot, tVA_SC_State *pScState)
{ 
   va_smcinit(); 
   CHECKSLOT(dwScSlot);
   if(NULL==pScState)
       return kVA_INVALID_PARAMETER;
   int rc=nglSmcGetState(dwScSlot,pScState);
   NGLOG_DEBUG("rc=%d ScState=%s",rc,(rc==0?stateNames[*pScState]:"??"));
   switch(rc){
   case NGL_INVALID_PARA:return kVA_INVALID_PARAMETER;
   case NGL_INSERT_CARD_FIRST:return kVA_INSERT_CARD_FIRST;
   case NGL_OK:
      CARDS[dwScSlot].state=*pScState;
      return kVA_OK;
   }
   return kVA_OK;
}

static void SendCBK(DWORD dwScSlot,BYTE*response,UINT size,void*data){
   if(NULL==response)
       VA_SC_CommandFailed(dwScSlot);
   else
       VA_SC_CommandDone(dwScSlot,size,response);
}
INT VA_SC_SendCommand(DWORD dwScSlot, UINT32 uiCommandSize, BYTE *pCommand)
{ 
   INT rc,responseSize,writed;
   CHECKSLOT(dwScSlot);
   if(NULL==pCommand||uiCommandSize==0)
       return kVA_INVALID_PARAMETER;
   switch(CARDS[dwScSlot].state){
   case eCARD_EXTRACTED:return kVA_INSERT_CARD_FIRST;
   case eCARD_INSERTED:return kVA_RESET_CARD_FIRST;
   case eCARD_READY://path through to default
   default:break;
   }
#if 0
   BYTE *response=(BYTE*)malloc(1024);
   NGLOG_DUMP("VA_SC_SendCommand",pCommand,uiCommandSize);
   rc=nglSmcTransfer(dwScSlot,pCommand,uiCommandSize,response,(UINT)&responseSize);
   if(rc==0)
       VA_SC_CommandDone(dwScSlot,responseSize,response);
   else
       VA_SC_CommandFailed(dwScSlot);
   free(response);
   return rc==0?kVA_OK:kVA_ERROR;
#else
   rc=nglSmcSendCommand(dwScSlot,pCommand,uiCommandSize,SendCBK,NULL);
   return  rc==0?kVA_OK:kVA_ERROR;
#endif
}
     

INT VA_SC_Activate(DWORD dwScSlot)
{ 
   NGLOG_DEBUG("slot=%d\r\n",dwScSlot);
   CHECKSLOT(dwScSlot);
   if(eCARD_EXTRACTED==CARDS[dwScSlot].state)
        return kVA_INSERT_CARD_FIRST; 
   return nglSmcActivate(dwScSlot);
}


INT VA_SC_Deactivate(DWORD dwScSlot)
{ 
   NGLOG_DEBUG("slot=%d\r\n",dwScSlot);
   CHECKSLOT(dwScSlot);
   if(eCARD_EXTRACTED==CARDS[dwScSlot].state)
        return kVA_INSERT_CARD_FIRST; 
   return nglSmcDeactivate(dwScSlot);
}


INT VA_SC_Lock(DWORD dwScSlot)
{ 
    CHECKSLOT(dwScSlot);
    if(eCARD_EXTRACTED==CARDS[dwScSlot].state)
        return kVA_INSERT_CARD_FIRST; 
    if(CARDS[dwScSlot].locked>0)
        return kVA_RESOURCE_ALREADY_LOCKED;
    CARDS[dwScSlot].locked++;
    return kVA_OK;
}


INT VA_SC_Unlock(DWORD dwScSlot)
{ 
   CHECKSLOT(dwScSlot);
   if(eCARD_EXTRACTED==CARDS[dwScSlot].state)
        return kVA_INSERT_CARD_FIRST; 
   if(CARDS[dwScSlot].locked==0)
       return kVA_RESOURCE_NOT_LOCKED;
   CARDS[dwScSlot].locked--;
   return kVA_OK;
}

/* End of File */
