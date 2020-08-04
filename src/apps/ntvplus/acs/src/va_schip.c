/*
  FILE : stub_schip.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_schip.h"
#include "aui_misc.h"
#include <stdio.h>
#include <aui_dsc.h>
#include <aui_kl.h>
#include <ngl_os.h>
#include <ngl_dsc.h>
#include <ngl_log.h>

NGL_MODULE(SCHIP)

//pls refto aui_test_stream_nim_dsc_kl.c

static const char*ModeName(tVA_SCHIP_CipheringMode md){
  switch(md){
     case eINACTIVE:return "eINACTIVE";
     case eSESSION: return "eSESSION";
     case eLOCKED:  return "eLOCKED";
     default:return "Unknown.CipheringMode";
  }
}
INT   VA_SCHIP_SetChipsetMode ( tVA_SCHIP_CipheringMode eChipsetMode )
{
     switch(eChipsetMode){
     case eINACTIVE:
     case eSESSION: 
     case eLOCKED: break ;
     default:return kVA_INVALID_PARAMETER;
     }
     int rc=nglSetCipherMode((NGLCipherMode)eChipsetMode);
     NGLOG_DEBUG("rc=%d eChipsetMode=%s",rc,ModeName(eChipsetMode));
     return rc==NGL_OK?kVA_OK:kVA_ERROR;
}

INT VA_SCHIP_GetChipsetMode ( tVA_SCHIP_CipheringMode* eChipsetMode )
{
     if(NULL==eChipsetMode)
         return kVA_INVALID_PARAMETER;
     int rc=nglGetCipherMode((NGLCipherMode*)eChipsetMode);
     NGLOG_DEBUG("rc=%d ChipsetMode=%s",rc,ModeName(*eChipsetMode));
     return rc==NGL_OK?kVA_OK:kVA_ERROR;
}


INT   VA_SCHIP_SetSessionKey ( UINT32 uiSessionKeyLength, const BYTE *pSessionKey )
{
    NGLOG_DEBUG("pSessionKey=%p,len=%d",pSessionKey,uiSessionKeyLength);
    NGLOG_DUMP("uiSessionKey",pSessionKey,uiSessionKeyLength);
    if(NULL==pSessionKey||(0==uiSessionKeyLength)||(uiSessionKeyLength%8))
        return kVA_INVALID_PARAMETER; 
    int rc=nglSetCipherSessionKey(pSessionKey,uiSessionKeyLength);
    return rc==NGL_OK?kVA_OK:kVA_ERROR;
}

DWORD VA_SCHIP_GetChipsetId ( void )
{
     DWORD id=nglGetChipID();
     NGLOG_DEBUG("chipid=%x",id);
     return id;
}

/*void STChip_Register   (DWORD dwStbStreamHandle)
{
     NGLOG_DEBUG("%s\r\n",__FUNCTION__);
     return;
}
    
void STChip_Unregister (DWORD dwStbStreamHandle)
{
     NGLOG_DEBUG("%s\r\n",__FUNCTION__);
     return;
}

void STChip_SetKeys (DWORD dwStbStreamHandle,
                        UINT32 uiOddKeyLength, BYTE *pOddKey,  
                        UINT32 uiEvenKeyLength, BYTE *pEvenKey)
{
     NGLOG_DEBUG("%s\r\n",__FUNCTION__);
     return;
}*/

/* End of File */
