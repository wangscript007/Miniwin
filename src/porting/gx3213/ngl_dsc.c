#include <ngl_dsc.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <gxos/gxcore_os_filesys.h>
#include <downloader/LoaderCore_Types.h>
#include <gxsecure/CA/gxifcp_api.h>
#include <gxsecure/gxsecure.h>
#include <dvbhal/gxdescrambler_hal.h>
//#include "gxsecure/gxfuse_api.h"
#include <gxsecure/gxsecure_api.h>

NGL_MODULE(DSCR);

#define INVALID_PID 0x1FFFF
typedef struct{
   int key_len;
}NGLDSC;
#define NUM_DSCS 8
static NGLDSC nglDSCS[NUM_DSCS];
#define CHECK(p) {if(p<nglDSCS||p>=&nglDSCS[NUM_DSCS])return NGL_INVALID_PARA;}
DWORD nglDscInit(){
   int rc;
   GxDescInitParam param;
   NGLOG_DEBUG("");
   bzero(nglDSCS,sizeof(nglDSCS));
   memset(&param, 0, sizeof(param));
   param.klm = GXDESC_KLM_NONE|GXDESC_KLM_ACPU_SOFT_DES|GXDESC_KLM_SCPU_DYNAMIC|GXDESC_KLM_ACPU_MSR2|GXDESC_KLM_SCPU_MSR3;
   rc= GxDescInit(&param);
   return rc;
}

DWORD nglDscOpen(USHORT *pids ,UINT cnt)
{
    int i;
    return (DWORD)0;
}

DWORD nglDscClose(DWORD dwDescrambleID )
{
    handle_t dschandle;
    GxDescClose(dschandle);
}

DWORD nglDscSetParameters(DWORD dwStbStreamHandle,const NGLDSC_Param *param )
{
    GxDescAlg alg = GXDESC_ALG_CAS_CSA2;
    handle_t dschandle=GxDescOpen(GXDESC_MOD_TS, 0, alg);
    return NGL_OK;
}


DWORD nglDscSetKeys(DWORD dwStbDescrHandle,const BYTE  *pOddKey,UINT32 uiOddKeyLength,
         const BYTE  *pEvenKey,UINT32 uiEvenKeyLength)
{
    return NGL_OK;
}

#define CHIPSET_OTP_ADDR (0)
#define KEY_OTP_ADDR	(0x03 * 4)
#define KL_KEY_OTP_SET	(1 << 23)

DWORD nglGetCipherMode(NGLCipherMode*md){
    return NGL_OK;
}

DWORD nglSetCipherMode(NGLCipherMode md){
    return NGL_OK;
}

DWORD nglSetCipherSessionKey(const BYTE*pSessionKey,UINT uiSessionKeyLength){
    return NGL_OK;
}

DWORD nglGetChipID(){
     DWORD chipid=0000;
     return NGL_OK;
}

