/*
  FILE : stub_prot.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_prot.h"
#include "va_errors.h"
#include <stdio.h>
#include <aui_dis.h>
/* Functions communes aux Acs 3x and 4x */
static int sSuspended=0,sResumed=0;

INT VA_PROT_SuspendFreeChannels()
{
    if(sSuspended)
       return kVA_ALREADY_DONE;
    sSuspended++;
    printf("%s\r\n",__FUNCTION__);
    return 0;
}

INT VA_PROT_ResumeFreeChannels()
{
    if(sResumed)return kVA_ALREADY_DONE;
    sResumed++;
    printf("%s\r\n",__FUNCTION__);
    return 0;
}

INT VA_PROT_GetCapabilities(WORD *pOutputProtection, WORD *pCopyProtection)
{
    printf("%s\r\n",__FUNCTION__);
    if(NULL==pOutputProtection||NULL==pCopyProtection)
        return kVA_INVALID_PARAMETER;
    *pOutputProtection=kVA_PROT_BIT_DVBCI|kVA_PROT_BIT_CI_PLUS_1_2|kVA_PROT_BIT_CI_PLUS_1_3;
            /*kVA_PROT_BIT_SPDIF = 0x0001     kVA_PROT_BIT_ETHERNET = 0x0002
            kVA_PROT_BIT_USB = 0x0004       kVA_PROT_BIT_ILINK = 0x0008
            kVA_PROT_BIT_DVBCI = 0x0010     kVA_PROT_BIT_CLEAR_RECORDING= 0x0020
            kVA_PROT_BIT_DVD = 0x0040       kVA_PROT_BIT_CI_PLUS_1_2= 0x0080
            kVA_PROT_BIT_CI_PLUS_1_3= 0x0100*/
    *pCopyProtection=kVA_PROT_BIT_CP_MECHANISM_FP|kVA_PROT_BIT_CP_MECHANISM_DTCP_IP;
    /*kVA_PROT_BIT_CP_MECHANISM_MACROVISION = 0x01
      kVA_PROT_BIT_CP_MECHANISM_FP = 0x02  Fingerprinting
      kVA_PROT_BIT_CP_MECHANISM_CGMSA = 0x04
      kVA_PROT_BIT_CP_MECHANISM_DTCP_IP = 0x08*/
    return 0;
}

INT VA_PROT_SetCopyProtection(DWORD dwAcsId, WORD wCopyProtection)
{
     //printf("%s\r\n",__FUNCTION__);
     if(dwAcsId>=1)
         return kVA_INVALID_PARAMETER;
     //if(wCopyProtection&kVA_PROT_BIT_CP_MECHANISM_FP)
     //aui_dis_
     return kVA_OK;
}

INT VA_PROT_StartFingerprintingSession(DWORD dwAcsId,
                                       const char *pszFingerprintStr,
                                       UINT8 uiFontSize,
                                       tVA_PROT_Color stForegroundColor,
                                       tVA_PROT_Color stBackgroundColor)
{
    printf("%s\r\n",__FUNCTION__);
    if(NULL==pszFingerprintStr||strlen(pszFingerprintStr)>kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE)
        return kVA_INVALID_PARAMETER;
    return 0;
}

INT VA_PROT_DisplayFingerprint(DWORD dwAcsId,
                               UINT8 uiHorizontalPercent,
                               UINT8 uiVerticalPercent,
                               UINT16 uiDuration)
{
    printf("%s\r\n",__FUNCTION__);
    if(uiHorizontalPercent>100||uiVerticalPercent>100)
        return kVA_INVALID_PARAMETER;
    return 0;
}

INT VA_PROT_StopFingerprintingSession(DWORD dwAcsId)
{
    printf("%s\r\n",__FUNCTION__);
    if(dwAcsId>=1)
       return kVA_INVALID_PARAMETER;
    return 0;
}

INT VA_PROT_SetCertificateList(tVA_PROT_CertificateListType eCertificateListType,
                               UINT32 uiLength,
                               const BYTE *pCertificateList)
{
    printf("%s\r\n",__FUNCTION__);
    if(NULL==pCertificateList||0==uiLength)
        return kVA_INVALID_PARAMETER;
    return 0;
}

INT VA_PROT_SetCiPlusContentInfos(DWORD dwAcsId,                                  
                                  const tVA_PROT_CiPlusInfos *pstCiPlusInfos)
{
    printf("%s\r\n",__FUNCTION__);
    if(dwAcsId>=1||NULL==pstCiPlusInfos)
       return kVA_INVALID_PARAMETER;
    return 0;
}

INT VA_PROT_SetCiPlusKeyLifetime(UINT32 uiKeyLifetime)
{
    printf("%s\r\n",__FUNCTION__);
    return 0;
}

INT VA_PROT_SetDigitalOutputProtection(WORD wOutputProtection)
{
    //printf("%s\r\n",__FUNCTION__);
    aui_hdl hdl_dishd=NULL;
    aui_find_dev_by_idx(AUI_MODULE_DIS,AUI_DIS_HD,&hdl_dishd);
    WORD bits=wOutputProtection&1;
    //aui_dis_macrovision_set(hdl_dishd,
    bits=(wOutputProtection>>1)&3;
    aui_dis_set(hdl_dishd,AUI_DIS_SET_CGMS,bits);
    
    return 0;
}
INT VA_PROT_SetCiPlusHostRevocationParameters(){
   return 0;
}
/* End of File */

