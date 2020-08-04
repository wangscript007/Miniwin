/*
  FILE : stub_sc.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "ngl_smc.h"
#include <stdio.h>
#include <string.h>
#include <ngl_log.h>

NGL_MODULE(SMC)

typedef struct{
  BYTE pps;
  INT locked;
  INT state;
  INT busy;//for reset read write
}SMCCARD;
typedef struct{
   INT slot;
   NGL_SMCSTATE_NOTIFY fn;
   void*userdata;
}SMCNOTIFY;

#define NB_NOTIFIERS 16
static SMCCARD CARDS[1]={NULL,0,0};
static SMCNOTIFY sNotifiers[NB_NOTIFIERS]={0,NULL};
static DWORD smc_msgq=0;
#define CHECKSLOT(s) {if(s<0||s>=sizeof(CARDS)/sizeof(SMCCARD))return NGL_INVALID_PARA;}

typedef struct{
   DWORD slot;
   char*cmd;
   UINT size; 
   void* CBK;
   void*userdata;
}SMCMSG;

static void CardStateProc(int slot,unsigned int state)//state :plugged in or plugged out
{
    int i;
    CARDS[slot].state=(state?eSMCCARD_INSERTED:eSMCCARD_EXTRACTED);
    for(i=0;i<NB_NOTIFIERS;i++){
        if(NULL==sNotifiers[i].fn)continue;
        if((sNotifiers[i].slot==slot)||(sNotifiers[i].slot<0))
            sNotifiers[i].fn(slot, CARDS[slot].state,sNotifiers[i].userdata);
    }
    NGLOG_DEBUG("Card %d state=%d",slot,state); 
}

INT nglSmcInit(){
    return NGL_OK;
}

INT nglSmcRegisterNotify(DWORD dwScSlot,NGL_SMCSTATE_NOTIFY fn,void*userdata){
    return NGL_OK;
}

INT nglSmcUnRegister(NGL_SMCSTATE_NOTIFY fn){
    return NGL_ERROR;
}
#define MAX_ATR_LN 33

INT nglSmcReset( DWORD dwScSlot, UINT8 uiPPSNegotiationValue,NGL_SMCRESET_NOTIFY fn )
{ 
   return NGL_OK;
}

INT nglSmcSendCommand(DWORD dwScSlot,BYTE*command,UINT cmdsize,NGL_SMCCOMMAND_NOTIFY fn,void*userdata){
   return 0;
}

INT nglSmcTransfer( DWORD dwScSlot,BYTE*command,UINT cmdsize,BYTE*response,UINT *responseSize)
{
    return NGL_OK;
}

INT nglSmcGetState(DWORD dwScSlot, tNGL_SMC_State *pScState)
{ 
   return NGL_OK;
}




INT nglSmcRead(DWORD dwScSlot, BYTE *pCommand,UINT uiCommandSize)
{ 
   return 0;
}
     
INT nglSmcActivate(DWORD dwScSlot)
{ 
   return NGL_OK;
}


INT nglSmcDeactivate(DWORD dwScSlot)
{ 
   return NGL_OK;
}


INT nglSmcLock(DWORD dwScSlot)
{ 
    return NGL_OK;
}


INT nglSmcUnlock(DWORD dwScSlot)
{ 
   return NGL_OK;
}

/* End of File */
