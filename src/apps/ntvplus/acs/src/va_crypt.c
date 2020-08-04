/*
  FILE : stub_crypt.c
  PURPOSE: This file is a stub for linking tests.
*/


#ifdef VAOPT_ENABLE_SECURE_CRYPT
#include "va_crypt.h"
#include "aui_dsc.h"
#include <stdio.h>
#include <aui_kl.h>
//only used for SECURE CRYPT ,coment by zhhou

typedef struct{
    aui_hdl klhdl;
    BOOL crypt;
}CIPHER;
static CIPHER sCipher={NULL,0};
static aui_hdl GetHDL(int dev_idx){
    aui_hdl hdl;
    aui_attr_kl attr;
    bzero(&attr,sizeof(attr));
    attr.uc_dev_idx = dev_idx;
    attr.en_level=AUI_KL_KEY_TWO_LEVEL;
    attr.en_key_pattern=AUI_KL_OUTPUT_KEY_PATTERN_128_SINGLE;
    attr.en_root_key_idx=AUI_KL_ROOT_KEY_0_0;
    attr.en_key_ladder_type=AUI_KL_TYPE_ALI;
    if(aui_find_dev_by_idx(AUI_MODULE_KL, attr.uc_dev_idx, &hdl))
        if(!aui_kl_open(&attr,&hdl))return -1;
    return hdl;
}
INT VA_CRYPT_GetCapabilities ( tVA_CRYPT_Capabilities * pCapabilities )
{
     aui_attr_kl attr;
     printf("%s\r\n",__FUNCTION__);
     if(NULL==pCapabilities)
         return kVA_INVALID_PARAMETER;
     bzero(pCapabilities,sizeof(tVA_CRYPT_Capabilities));
     pCapabilities->uiNbKeySlots=kVA_CRYPT_NB_SLOT;//10
     pCapabilities->bAllowImportClearKey=FALSE;
     pCapabilities->bAllowExportKey=FALSE;
     pCapabilities->bAllowRandomGeneration=FALSE;
     pCapabilities->uiMinKeyLevelForCipher=2;
     pCapabilities->uiMaxKeyLevelForCipher=2;
     if(NULL==sCipher.klhdl){
         bzero(&attr,sizeof(attr));
         attr.en_level=AUI_KL_KEY_TWO_LEVEL;
         attr.en_key_pattern=AUI_KL_OUTPUT_KEY_PATTERN_128_SINGLE;
         attr.en_root_key_idx=AUI_KL_ROOT_KEY_0_0;
         attr.en_key_ladder_type=AUI_KL_TYPE_ALI;
         aui_kl_open(&attr,&sCipher.klhdl);
     }
     return kVA_OK;
}

INT VA_CRYPT_ImportKey ( const BYTE * pWrappedKey , UINT32 uiKeySize , DWORD dwWrappingKeyId , DWORD * pImportedKeyId )
{//uiKeySize mustbe kVA_CRYPT_KEY_SIZE(16)
     aui_cfg_kl cfg;
     printf("%s\r\n",__FUNCTION__);
     if( (NULL==pImportedKeyId) || (NULL==pWrappedKey ||  kVA_CRYPT_KEY_SIZE!=uiKeySize) 
        ||( (kVA_CRYPT_ROOT_KEY_LVL2_ID1!=dwWrappingKeyId)&&( kVA_CRYPT_CLEAR_KEY !=dwWrappingKeyId) ) )
         return kVA_INVALID_PARAMETER;
     bzero(&cfg,sizeof(cfg));
     cfg.run_level_mode = AUI_KL_RUN_LEVEL_MODE_LEVEL_ALL;
     cfg.en_kl_algo = AUI_KL_ALGO_AES;	//algo_pvr2kl(p_key->algo);
     cfg.en_crypt_mode = AUI_KL_DECRYPT;
     
     aui_kl_gen_key_by_cfg(sCipher.klhdl,&cfg,NULL);
}

INT VA_CRYPT_ExportKey ( BYTE * pWrappedKey , UINT32 * puiKeySize , DWORD dwWrappingKeyId , DWORD dwExportKeyId )
{
     printf("%s\r\n",__FUNCTION__);
}

INT VA_CRYPT_ReleaseKey ( DWORD dwKeyId )
{
     printf("%s\r\n",__FUNCTION__);
}

INT VA_CRYPT_CipherOpen ( const tVA_CRYPT_Config * pConfig , DWORD * pHandle )
{
    printf("%s\r\n",__FUNCTION__);
    aui_hdl hdl;
    algo_t algo;
    aui_attr_kl attr;
    if(NULL==pConfig || NULL==pHandle)
        return kVA_INVALID_PARAMETER;
    if(NULL!=sCipher.klhdl)
        return kVA_RESOURCE_BUSY;
    /*aui_hdl hdl;
    aui_attr_dsc attr;
    bzero(&attr,sizeof(attr));
    attr.dsc_data_type = AUI_DSC_DATA_TS;
    switch(pConfig->eAlgorithm){
    case eVA_CRYPT_AES_ECB:  
         attr.uc_algo=AUI_DSC_ALGO_AES;
         attr.uc_mode=AUI_DSC_WORK_MODE_IS_ECB;
         break;
    case eVA_CRYPT_AES_CBC:  
         attr.uc_algo=AUI_DSC_ALGO_AES;
         attr.uc_mode=AUI_DSC_WORK_MODE_IS_CBC;
         break;
    case eVA_CRYPT_TDES_ECB: 
         attr.uc_algo=AUI_DSC_ALGO_TDES; 
         attr.uc_mode=AUI_DSC_WORK_MODE_IS_ECB;
         break;
    case eVA_CRYPT_TDES_CBC: 
         attr.uc_algo=AUI_DSC_ALGO_TDES;
         attr.uc_mode=AUI_DSC_WORK_MODE_IS_CBC;
         break;
    }
    // attr.uc_mode=pConfig->bEncryptNotDecrypt;// Encrypt if true, decrypt if false
    aui_dsc_open(&attr,&hdl);*/
    switch(pConfig->eAlgorithm){
    case eVA_CRYPT_AES_ECB:  algo=AES; break;
    case eVA_CRYPT_AES_CBC:  algo=AES; break;
    case eVA_CRYPT_TDES_ECB: algo=TDES;break;
    case eVA_CRYPT_TDES_CBC: algo=TDES;break;
    }
    pConfig->pIV;
    pConfig->uiSizeIV;//must be kVA_CRYPT_AES_IV_SIZE(8) for AES and kVA_CRYPT_TDES_IV_SIZE(16) for TDES). It is ignored if pIV is NULL
    bzero(&attr,sizeof(attr));
    attr.en_level=AUI_KL_KEY_TWO_LEVEL;
    attr.en_key_pattern=AUI_KL_OUTPUT_KEY_PATTERN_128_SINGLE;
    attr.en_root_key_idx=AUI_KL_ROOT_KEY_0_0;
    attr.en_key_ladder_type=AUI_KL_TYPE_ALI;
    //alisldsc_algo_open(&hdl,algo,0);
    //alisldsc_algo_ioctrl(hdl,
    aui_kl_open(&attr,&sCipher.klhdl);
    *pHandle=(DWORD*)&sCipher;
    return kVA_OK;
}
INT VA_CRYPT_CipherProcess ( DWORD dwHandle , const BYTE * pSrc , BYTE * pDst , UINT32 uiSize ) 
{//use openssl??
    printf("%s\r\n",__FUNCTION__);
    if(NULL==pSrc || NULL==pDst || (dwHandle!=&sCipher))//( kVA_CRYPT_AES_BLOCK_SIZE!=uiSize &&  kVA_CRYPT_DES_BLOCK_SIZE!=uiSize))
        return kVA_INVALID_PARAMETER;
    aui_dsc_encrypt(sCipher.klhdl,pSrc,pDst,uiSize);
}
INT VA_CRYPT_CipherClose ( DWORD dwHandle ) 
{
    printf("%s\r\n",__FUNCTION__);
    if(dwHandle!=&sCipher)
        return  kVA_INVALID_PARAMETER;
    aui_dsc_close((aui_hdl)dwHandle);
}

UINT32 VA_CRYPT_GetRandom ( void ) 
{
    printf("%s\r\n",__FUNCTION__);
}
#endif /* VAOPT_ENABLE_SECURE_CRYPT */

/* End of File */
