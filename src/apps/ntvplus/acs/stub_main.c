/*
  FILE : stub_main
  .c
  PURPOSE: This file is a stub for linking tests.
*/

#include "va_dscr.h"
#include "va_errors.h"
#include "va_init.h"
#include "va_os.h"
#include "va_pvr.h"
#include "va_sc.h"
#include "va_schip.h"
//#include "va_sec.h"
#include "va_stdby.h"
#include "va_http.h"

#define VAOPT_ENABLE_SC  1

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31)
#include "va_nvm.h"
#else
#include "va_xnvm.h"
#endif
#if !defined VAOPT_DISABLE_VA_CTRL && !defined(__STB_SIMULATOR__)
#include "va_ctrl.h"
#else
#include "va_pi.h"
#endif

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40)

#define kVA_DMX_MAX_FILTER_SIZE    10
DWORD VA_DMX_AllocateSectionFilter(DWORD dwAcsId, DWORD dwVaFilterHandle,WORD  wPid, UINT16 uiNbBuffers);
INT VA_DMX_FreeSectionFilter( DWORD dwStbFilterHandle );
INT VA_DMX_SetSectionFilterParameters(DWORD dwStbFilterHandle, UINT32 uiLength, BYTE *pValue, BYTE *pMask);
INT VA_DMX_StartSectionFilter(DWORD  dwStbFilterHandle);
INT VA_DMX_StopSectionFilter(DWORD  dwStbFilterHandle);
INT VA_DMX_UnlockSectionFilter(DWORD dwStbFilterHandle);
INT VA_DMX_NotifyFilteredSection(DWORD dwVaFilterHandle,UINT32 uiBufferLength, BYTE *pBuffer, void *pUserData);

DWORD VA_NET_UnicastOpen( DWORD dwVaNetHandle, 
                         const char *pIPAddress, UINT16 uiPort, UINT32 uiTimeout);
INT VA_NET_UnicastReceive( DWORD dwStbNetHandle, UINT32 uiLength ,UINT32 uiTimeout );
INT VA_NET_UnicastReceiveRelease( DWORD dwStbNetHandle );
INT VA_NET_UnicastSend ( DWORD dwStbNetHandle, 
                        UINT32 uiDataLength ,BYTE* pData, UINT32 uiTimeout );
INT VA_NET_UnicastClose( DWORD dwStbNetHandle );

DWORD VA_NET_MulticastOpen( DWORD dwVaNetHandle, 
                           const char *pIPAddress, UINT16 uiPort, UINT32 uiBufferSize );
INT VA_NET_MulticastNotifyRelease( DWORD dwStbNetHandle );
INT VA_NET_MulticastClose( DWORD dwStbNetHandle );

INT VA_NET_UnicastReceiveDone( DWORD dwVaNetHandle , UINT32 uiDataLength, BYTE * pData);
INT VA_NET_UnicastReceiveFailed( DWORD dwVaNetHandle );
INT VA_NET_UnicastSendDone( DWORD dwVaNetHandle );
INT VA_NET_UnicastSendFailed( DWORD dwVaNetHandle ,UINT32 uiBytesSent );
INT VA_NET_UnicastOpenDone( DWORD dwVaNetHandle );
INT VA_NET_UnicastOpenFailed( DWORD dwVaNetHandle );

INT VA_NET_MulticastOpenDone( DWORD dwVaNetHandle );
INT VA_NET_MulticastOpenFailed( DWORD dwVaNetHandle );
INT VA_NET_MulticastNotify ( DWORD dwVaNetHandle ,UINT32 uiDataLength, BYTE * pData);

#else
#include "va_crypt.h"
#include "va_fs.h"

#if defined VAOPT_ENABLE_ACS41

#define kVA_DMX_MAX_FILTER_SIZE    10
typedef INT (*tVA_DMX_NotifyFilteredSection)(DWORD dwVaFilterHandle , 
                                             UINT32 uiBufferLength  , 
                                             BYTE *pBuffer          ,
                                             void *pUserData        );
DWORD VA_DMX_AllocateSectionFilter(DWORD dwAcsId, DWORD dwVaFilterHandle,
        WORD  wPid, UINT16 uiNbBuffers,tVA_DMX_NotifyFilteredSection pfVA_DMX_NotifyFilteredSection);
INT VA_DMX_FreeSectionFilter( DWORD dwStbFilterHandle );
INT VA_DMX_SetSectionFilterParameters(DWORD dwStbFilterHandle, UINT32 uiLength, BYTE *pValue, BYTE *pMask);
INT VA_DMX_StartSectionFilter(DWORD  dwStbFilterHandle);
INT VA_DMX_StopSectionFilter(DWORD  dwStbFilterHandle);
INT VA_DMX_UnlockSectionFilter(DWORD dwStbFilterHandle);

typedef enum 
{
    eVA_NOT_CONNECTED,
    eVA_OPEN_DONE,
    eVA_OPEN_FAILED,
    eVA_RECEIVE_DONE,
    eVA_RECEIVE_FAILED,
    eVA_SEND_DONE,
    eVA_SEND_FAILED
} tVA_NET_Status;
typedef INT (*tVA_NET_Callback)(DWORD dwVaNetHandle, tVA_NET_Status eStatus, 
                                UINT32 uiDataLength ,BYTE *pData);
DWORD VA_NET_UnicastOpen( DWORD dwVaNetHandle, 
                         const char *pIPAddress, UINT16 uiPort, UINT32 uiTimeout,
                         tVA_NET_Callback pfVaUnicastCallback);
INT VA_NET_UnicastReceive( DWORD dwStbNetHandle, UINT32 uiLength ,UINT32 uiTimeout );
INT VA_NET_UnicastReceiveRelease( DWORD dwStbNetHandle ) ;
INT VA_NET_UnicastSend ( DWORD dwStbNetHandle, 
                        UINT32 uiDataLength ,BYTE* pData, UINT32 uiTimeout ) ;
INT VA_NET_UnicastClose( DWORD dwStbNetHandle ) ;
DWORD VA_NET_MulticastOpen( DWORD dwVaNetHandle, 
                           const char *pIPAddress, UINT16 uiPort, UINT32 uiBufferSize,
                           tVA_NET_Callback  pfVaMulticastCallback ) ;
INT VA_NET_MulticastNotifyRelease( DWORD dwStbNetHandle ) ;
INT VA_NET_MulticastClose( DWORD dwStbNetHandle ) ;
#define kVA_RELEASE_RESOURCES             9

#else

/* ACS > 4.1 */
#include "va_dmx.h"
#include "va_net.h"

#endif /* VAOPT_ENABLE_ACS41 */
#endif

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31)

INT VA_PROT_StartMacrovision(void);
INT VA_PROT_StopMacrovision (void);
INT VA_PROT_SetCopyProtection(BYTE byCopyProtection);
INT VA_PROT_StartFingerprinting(char *pFingerprintStr);
INT VA_PROT_StopFingerprinting(void);
INT VA_PROT_SetDigitalOutputProtection(WORD wOutputProtection);
INT VA_PROT_SuspendAnalogChannels(void);
INT VA_PROT_ResumeAnalogChannels(void);
INT VA_PROT_SuspendFreeChannels(void);
INT VA_PROT_ResumeFreeChannels(void);
#elif (defined VAOPT_ENABLE_ACS40) || (defined VAOPT_ENABLE_ACS41)
typedef enum
{
    eCiPlusCertificateRevocationList,   /* CI + Certificate Revocation List    */
    eCiPlusCertificateWhiteList,        /* CI + Certificate White List         */
    eCiPlusRevocationSignallingData,    /* CI + RevocationSignallingData       */
    eDtcpIpCertificateRevocationList,   /* DTCP-IP Certificate Revocation List */

    eNbCertificateLists

} tVA_PROT_CertificateListType;

typedef struct
{
    BYTE aURI[8];                 /* CI+ Usage Rules Information */
    BOOL bAesMandatoryForContent; /* FALSE if the DES scrambling is authorized for HD content */
} tVA_PROT_CiPlusInfos;

INT VA_PROT_GetCapabilities(WORD *pOutputProtection, WORD *pCopyProtection);
INT VA_PROT_SetCopyProtection(DWORD dwAcsId, WORD wCopyProtection);
INT VA_PROT_StartFingerprinting(DWORD dwAcsId, const char *pszFingerprintStr);
INT VA_PROT_StopFingerprinting(DWORD dwAcsId);
INT VA_PROT_SetDigitalOutputProtection(WORD wOutputProtection);
INT VA_PROT_SuspendAnalogChannels(void);
INT VA_PROT_ResumeAnalogChannels(void);
INT VA_PROT_SuspendFreeChannels(void);
INT VA_PROT_ResumeFreeChannels(void);
INT VA_PROT_SetCertificateList(tVA_PROT_CertificateListType eCertificateListType, 
                               UINT32 uiLength,
                               const BYTE *pCertificateList);
INT VA_PROT_SetCiPlusContentInfos(DWORD dwAcsId,
                                  BOOL bActivateCiPlus,
                                  const tVA_PROT_CiPlusInfos *pstCiPlusInfos);
INT VA_PROT_SetCiPlusKeyLifetime(UINT32 uiKeyLifetime);

#else /* ACS > 5.x */
#include "va_prot.h"
#endif
/**
 * Calls misc ACS functions to force linking
 */
int testAPI(void)
{
    //VA_INIT_StartAcs();

#if !defined VAOPT_DISABLE_VA_CTRL && !defined(__STB_SIMULATOR__)
    VA_CTRL_OpenAcsInstance(0,0);
    VA_CTRL_SwitchOnProgram(0, 0, NULL);
    VA_CTRL_AddStream(0,0,0, NULL);
    VA_CTRL_RemoveStream(0);
#else
    VA_PI_OpenAcs(0,0);
    VA_PI_SwitchOnProgram(0);
    VA_PI_AddStream(0,0);
    VA_PI_RemoveStream(0);
#endif
    //VA_INIT_StopAcs();

    //VA_SEC_Request(NULL);
    VA_UI_AddEventListener(NULL, NULL);
    VA_UI_RemoveEventListener(0);

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40)
    VA_DMX_AllocateSectionFilter(0, 0, 0, 0);
    VA_DMX_NotifyFilteredSection(0, 0, NULL, NULL);
#elif defined VAOPT_ENABLE_ACS41
    VA_DMX_AllocateSectionFilter(0, 0, 0, 0, NULL);
#else
    VA_DMX_AllocateSectionFilter(0, 0, 0, NULL);
#endif
    VA_DMX_FreeSectionFilter( 0 );
#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40 || defined VAOPT_ENABLE_ACS41)
    VA_DMX_SetSectionFilterParameters(0, 0, NULL, NULL);
#else
    VA_DMX_SetSectionFilterParameters(0, 0, NULL, NULL, eVA_DMX_OneShot);
#endif
    VA_DMX_StartSectionFilter( 0 );
    VA_DMX_StopSectionFilter(0);

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40 || defined VAOPT_ENABLE_ACS41)
    VA_DMX_UnlockSectionFilter(0);
#endif

    /* Call dscr functions */
    VA_DSCR_Open(0);
    VA_DSCR_Close(0);
    VA_DSCR_SetKeys(0,0, NULL, 0, NULL, NULL);
#if (!defined VAOPT_ENABLE_ACS30 && !defined VAOPT_ENABLE_ACS31 && !defined VAOPT_ENABLE_ACS40)
    VA_DSCR_SetParameters(0, NULL);
#endif

    /* Call init functions */
    //VA_INIT_InitializeAcs(NULL) ;
    //VA_INIT_StartAcs();
    //VA_INIT_StopAcs();

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31)
    /* Call nvm functions */
    VA_NVM_Read(0, 0,  NULL);
    VA_NVM_Write(0, 0, NULL);
    VA_NVM_RequestDone();
    VA_NVM_RequestFailed();
#else
    /* Call xnvm functions */
    VA_XNVM_Open(0, 0);
    VA_XNVM_Close(0);
    VA_XNVM_Read (0, 0, 0, NULL);
    VA_XNVM_Write(0, 0, 0, NULL);
    VA_XNVM_RequestDone(0);
    VA_XNVM_RequestFailed(0);
#endif

    /* Call OS functions */
    VA_OS_InitializeSemaphore(NULL, 0);
    VA_OS_DeleteSemaphore(0);
    VA_OS_AcquireSemaphore(NULL, 0);
    VA_OS_ReleaseSemaphore(NULL );
    VA_OS_InitializeMutex(NULL);
    VA_OS_DeleteMutex(NULL);
    VA_OS_LockMutex(NULL);
    VA_OS_UnlockMutex(NULL);
    VA_OS_Alloc (0);
    VA_OS_Free(0);
    VA_OS_Printf("...");
    VA_OS_Getc(0);
    VA_OS_Sleep(0);
    VA_OS_StartPeriodicCall(0, NULL,0);
    VA_OS_StopPeriodicCall(0);
    VA_OS_GetTime(NULL);
    VA_OS_TimeToTm(NULL, NULL);
    VA_OS_TmToTime(NULL, NULL);
    VA_OS_GetRunTime(NULL);

#if !defined VAOPT_DISABLE_VA_CTRL && !defined(__STB_SIMULATOR__)
    /* Call CTRL functions */
    VA_CTRL_OpenAcsInstance( 0, eLIVE );
    VA_CTRL_CloseAcsInstance( 0 );
    VA_CTRL_TsChanged(0 );
    VA_CTRL_CatUpdated(0, 0, NULL);
    VA_CTRL_SwitchOnProgram(0, 0, NULL) ; 
    VA_CTRL_SwitchOffProgram(0);
    VA_CTRL_PmtUpdated(0, 0, NULL);
    VA_CTRL_GetProgramInformation(0, NULL);
    VA_CTRL_AddStream(0, 0, 0, NULL);
    VA_CTRL_QueryStream(0, 0, 0, NULL);
    VA_CTRL_UpdateStream (0, 0);
    VA_CTRL_RemoveStream (0);
    //VA_CTRL_FileTransferRegister (0, 0);
    //VA_CTRL_FileTransferUnregister (0);
#else
    /* Call PI functions */
    VA_PI_OpenAcs( 0, eLIVE );
    VA_PI_ChangeAcsMode( 0, eLIVE );
    VA_PI_CloseAcs( 0 );
    VA_PI_TsChanged(0 );
    VA_PI_CatUpdated(0);
    VA_PI_ParseCatDescriptors (0);
    VA_PI_CatDescParsingFct(0, NULL);
    VA_PI_SwitchOnProgram(0) ; 
    VA_PI_SwitchOffProgram(0);
    VA_PI_PmtUpdated(0);
    VA_PI_ParsePmtProgramDescriptors(0);
    VA_PI_PmtProgramDescParsingFct(0, NULL);
    VA_PI_GetProgramInformation(0, NULL);
    VA_PI_AddStream(0, 0);
    VA_PI_QueryStream(0, 0);
    VA_PI_UpdateStream (0);
    VA_PI_RemoveStream (0);
    VA_PI_NotifyStreamEvent(0, eCLEAR);
    VA_PI_NotifyStreamRecordPolicy (0, eRECORD_SCRAMBLED);
    VA_PI_ParsePmtStreamDescriptors(0);
    VA_PI_PmtStreamDescParsingFct(0, NULL);
    VA_PI_GetStreamInformation(0, NULL);
#endif
    /* Call PROT functions */
    //VA_PROT_SetDigitalOutputProtection(0);
    VA_PROT_SuspendFreeChannels();
    VA_PROT_ResumeFreeChannels();
    #if defined (VAOPT_ENABLE_ACS30)
    VA_PROT_SuspendAnalogChannels();
    VA_PROT_ResumeAnalogChannels();
    VA_PROT_StartMacrovision();
    VA_PROT_StopMacrovision();
    VA_PROT_StartFingerprinting (NULL);
    VA_PROT_StopFingerprinting ();
    #elif defined (VAOPT_ENABLE_ACS31)
    VA_PROT_SuspendAnalogChannels();
    VA_PROT_ResumeAnalogChannels();
    VA_PROT_SetCopyProtection(0);
    VA_PROT_StartFingerprinting (NULL);
    VA_PROT_StopFingerprinting ();
    #elif defined (VAOPT_ENABLE_ACS40) || defined (VAOPT_ENABLE_ACS41)
    VA_PROT_SuspendAnalogChannels();
    VA_PROT_ResumeAnalogChannels();
    VA_PROT_GetCapabilities(NULL,NULL);
    VA_PROT_SetCopyProtection(0,0);
    VA_PROT_StartFingerprinting(0,NULL);
    VA_PROT_StopFingerprinting(0);
    VA_PROT_SetCertificateList(0,0,NULL); 
    VA_PROT_SetCiPlusContentInfos(0,0,NULL);
    VA_PROT_SetCiPlusKeyLifetime(0);
    #else /* ACS 5.x*/
    {
    tVA_PROT_Color stColor = {0};
    VA_PROT_GetCapabilities(NULL,NULL);
    VA_PROT_SetCopyProtection(0,0);
    VA_PROT_StartFingerprintingSession(0,NULL,0,stColor,stColor);
    VA_PROT_DisplayFingerprint(0,0,0,0);
    VA_PROT_StopFingerprintingSession(0);
    VA_PROT_SetCertificateList(0,0,NULL); 
    VA_PROT_SetCiPlusContentInfos(0,NULL);
    VA_PROT_SetCiPlusKeyLifetime(0);
    }
    #endif


    /* Eventually call PVR functions */
     #if 1//defined (VAOPT_ENABLE_PVR)
      VA_PVR_OpenEcmChannel ( 0, ePERSISTENT, 0 );
      VA_PVR_CloseEcmChannel ( 0 );
      VA_PVR_RecordEcm (  0, 0, NULL, NULL );
      VA_PVR_WriteMetadata (  0, 0, NULL );
      VA_PVR_ReadMetadata (   0, 0, NULL );
    #endif
    
    /* Eventually call standby functions */
    #if (defined VAOPT_ENABLE_DATACOLLECT && defined VAOPT_ENABLE_IPPV)
      VA_STDBY_StandbyProcessDone();
      VA_STDBY_SetAlarm(NULL);
      VA_STDBY_AlarmProcessDone();
      VA_STDBY_EnterStandby();
      VA_STDBY_NotifyAlarm();
      VA_STDBY_ExitStandby();
    #endif

    /* Eventually call sc functions */
    #if defined (VAOPT_ENABLE_SC)
      VA_SC_Reset(0, 0);
      VA_SC_ResetDone(0, 0, NULL, 0);
      VA_SC_ResetFailed(0);
      VA_SC_GetState(0, NULL);
      VA_SC_CardInserted(0);
      VA_SC_CardExtracted(0);
      VA_SC_SendCommand(0, 0, NULL);
      VA_SC_CommandDone(0, 0, NULL);
      VA_SC_CommandFailed(0);
      VA_SC_Activate(0);
      VA_SC_Deactivate(0);
      VA_SC_Lock(0);
      VA_SC_Unlock(0);
    #endif

    /* Eventually call net functions */
    #if defined (VAOPT_ENABLE_MODEM)
      VA_NET_UnicastModemOpen( 0, 0, NULL, 0, NULL, 0);
    #endif
    #if (defined __COMMON_ENABLE_UNICAST_DRIVER__)|| (defined VAOPT_ENABLE_DATACOLLECT)
    #if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40)
      VA_NET_UnicastOpen( 0, NULL, 0, 0);
      VA_NET_UnicastOpenDone(0) ;
      VA_NET_UnicastReceiveDone(0 , 0, NULL) ;
      VA_NET_UnicastReceiveFailed(0) ;
      VA_NET_UnicastSendDone(0) ;
      VA_NET_UnicastSendFailed(0, 0) ;
      VA_NET_UnicastOpenDone(0) ;
      VA_NET_UnicastOpenFailed(0) ;
    #else
      VA_NET_UnicastOpen( 0, NULL, 0, 0, NULL);
    #endif
      VA_NET_UnicastReceive( 0, 0 ,0);
    #if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40 || defined VAOPT_ENABLE_ACS41)
      VA_NET_UnicastReceiveRelease( 0) ;
    #endif
      VA_NET_UnicastSend ( 0, 0, NULL, 0) ;
      VA_NET_UnicastClose(0) ;
    #endif /* (defined __COMMON_ENABLE_UNICAST_DRIVER__)|| (defined VAOPT_ENABLE_DATACOLLECT) */

    #if defined (VAOPT_ENABLE_IPCAS)
    #if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40)
      VA_NET_MulticastOpen( 0, NULL, 0, 0) ;
      VA_NET_MulticastOpenDone(0) ;
      VA_NET_MulticastOpenFailed(0) ;
      VA_NET_MulticastNotify (0, 0, NULL) ;
    #else
      VA_NET_MulticastOpen( 0, NULL, 0, 0, NULL) ;
    #endif
    #if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40 || defined VAOPT_ENABLE_ACS41)
      VA_NET_MulticastNotifyRelease(0) ;
    #endif
      VA_NET_MulticastClose(0) ;
    #endif


    #if defined (VAOPT_ENABLE_SCHIP)
      VA_SCHIP_SetChipsetMode ( 0 ) ;
      VA_SCHIP_SetSessionKey ( 0 , NULL ) ;
      VA_SCHIP_GetChipsetId ( ) ;
    #endif

    #if (!defined VAOPT_ENABLE_ACS30 && !defined VAOPT_ENABLE_ACS31 && !defined VAOPT_ENABLE_ACS40)
    #if defined (VAOPT_ENABLE_FILESYSTEM)
        VA_FS_Open(NULL, 0);
        VA_FS_Close(0);
        VA_FS_Read (0, NULL, NULL);
        VA_FS_Write (0, NULL, NULL);
        VA_FS_Seek (0, 0, 0, NULL);
        VA_FS_GetSize (NULL, NULL);
        VA_FS_Remove (NULL);
        VA_FS_GetFilesCount();
        VA_FS_GetFiles(NULL, NULL);
    #endif

    #if defined VAOPT_ENABLE_SECURE_CRYPT
        VA_CRYPT_GetCapabilities(NULL);
        VA_CRYPT_ImportKey(NULL,0,0,NULL);
        VA_CRYPT_ExportKey(NULL,NULL,0,0);
        VA_CRYPT_ReleaseKey(0);
        VA_CRYPT_CipherOpen(NULL,NULL);
        VA_CRYPT_CipherProcess(0,NULL,NULL,0);
        VA_CRYPT_CipherClose(0);
        VA_CRYPT_GetRandom();
    #endif
    #endif /* (!defined VAOPT_ENABLE_ACS30 && !defined VAOPT_ENABLE_ACS31 && !defined VAOPT_ENABLE_ACS40) */
    VA_HTTP_Init(NULL);
    VA_HTTP_Terminate();
    VA_HTTP_CreateRequest();
    VA_HTTP_DeleteRequest(NULL);
    VA_HTTP_SetHeader(NULL,NULL);
    VA_HTTP_ResetHeaders(NULL);
    VA_HTTP_SetCookiePersistence(NULL);
    VA_HTTP_SetCookieList(NULL,NULL);
    VA_HTTP_SetMethod(NULL,0);
    VA_HTTP_SetPostData(NULL,NULL,0,0);
    VA_HTTP_SetTimeout(NULL,0);
    VA_HTTP_SetTcpKeepAlive(NULL,0);
    VA_HTTP_DoRequest(NULL,NULL,NULL,NULL);
    VA_HTTP_GetResponseData(NULL,NULL,NULL);
    VA_HTTP_ReleaseBuffer(NULL);
    VA_HTTP_GetResponseCode(NULL,NULL);
    VA_HTTP_GetCookieList(NULL,NULL);
    VA_HTTP_ReleaseStrList(NULL);
    return 0;
}


/* End of File */
