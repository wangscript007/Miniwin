/*
  FILE : stub_net.c
  PURPOSE: This file is a stub for linking tests.
*/
#include <va_types.h>
#include "va_net.h"

/* Unicast functions available for broadcast with mode, IP no option and purplebox*/
#if (defined __COMMON_ENABLE_UNICAST_DRIVER__ || defined VAOPT_ENABLE_DATACOLLECT)

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40)
DWORD VA_NET_UnicastOpen(DWORD dwVaNetHandle, const char * pIPAddress,
                         UINT16 uiPort, UINT32 uiTimeout)
{
#else
DWORD VA_NET_UnicastOpen(DWORD dwVaNetHandle, const char * pIPAddress,
                         UINT16 uiPort, UINT32 uiTimeout,
                         tVA_NET_Callback pfVaUnicastCallback)
{
#endif
      printf("%s\r\n",__FUNCTION__);
      return 0;
}

INT VA_NET_UnicastReceive( DWORD dwStbNetHandle, UINT32 uiMaximumLength ,UINT32 uiTimeout )
{
      printf("%s\r\n",__FUNCTION__);
  return 0;
}

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40 || defined VAOPT_ENABLE_ACS41)
INT VA_NET_UnicastReceiveRelease( DWORD dwStbNetHandle )
{
      printf("%s\r\n",__FUNCTION__);
  return 0;
}
#endif

INT VA_NET_UnicastSend ( DWORD dwStbNetHandle, UINT32 uiDataLength ,BYTE* pData, UINT32 uiTimeout )
{
      printf("%s\r\n",__FUNCTION__);
  return 0;
}


INT VA_NET_UnicastClose( DWORD dwStbNetHandle )
{
      printf("%s\r\n",__FUNCTION__);
  return 0;
}
#endif

#if defined VAOPT_ENABLE_IPCAS
#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40)
DWORD VA_NET_MulticastOpen(DWORD dwVaNetHandle ,
                           const char *pIPAddress, UINT16 uiPort,
                           UINT32 uiBufferSize)
{
#else
DWORD VA_NET_MulticastOpen(DWORD dwVaNetHandle ,
                           const char *pIPAddress, UINT16 uiPort,
                           UINT32 uiBufferSize,
                         tVA_NET_Callback pfVaMulticastCallback)
{
#endif
      printf("%s\r\n",__FUNCTION__);
  return 0;
}

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40 || defined VAOPT_ENABLE_ACS41)
INT VA_NET_MulticastNotifyRelease( DWORD dwStbNetHandle )
{
      printf("%s\r\n",__FUNCTION__);
  return 0;
}
#endif

INT VA_NET_MulticastClose( DWORD dwStbNetHandle )
{
      printf("%s\r\n",__FUNCTION__);
  return 0;
}
#endif

/* End of File */
