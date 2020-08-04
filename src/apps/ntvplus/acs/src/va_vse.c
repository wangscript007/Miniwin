/*
  FILE : stub_vse.c
  PURPOSE: This file is a stub for linking tests.
*/

#if defined VAOPT_ENABLE_VSE

#define VO_TYPES_DEFINED /* to avoid types redefinitions */
#include "vse_common.h"
#include "host_vse_syst.h"
#include "host_vse_srpc.h"

UINT32 HOST_VSE_SYST_InitVSEAppliContext(tVSE_APPLI_ID appliID)
{
  printf("%s\r\n",__FUNCTION__);
  return 0;
}

UINT32 HOST_VSE_SYST_ReleaseVSEAppliContext(tVSE_APPLI_ID appliID)
{
  printf("%s\r\n",__FUNCTION__);
  return 0;
}

UINT32 HOST_VSE_SRPC_Call(tVSE_APPLI_ID appliID, UINT32 uiCommandID)
{
  printf("%s\r\n",__FUNCTION__);
  return 0;
}

UINT32 HOST_VSE_SRPC_GetBuffer(tVSE_APPLI_ID appliID, BYTE** pBuffer, UINT32 *puiBufferSize)
{
  printf("%s\r\n",__FUNCTION__);
  return 0;
}

#endif /* defined VAOPT_ENABLE_VSE */

/* End of File */
