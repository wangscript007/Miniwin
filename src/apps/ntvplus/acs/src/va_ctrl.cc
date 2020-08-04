/*
  FILE : stub_ctrl.c
  PURPOSE: This file is a stub for linking tests.
*/

#include <stdio.h>
extern "C"{
#include "va_ctrl.h"
}
#include <dvbepg.h>

extern UINT16 g_service_id;
INT VA_CTRL_GetProgramInformation( DWORD dwAcsId, tVA_CTRL_ProgramInformation *pProgramInfo )
{
  //printf("%s\r\n",__FUNCTION__);
  SERVICELOCATOR cur;
  DtvGetCurrentService(&cur);
  pProgramInfo->wNetworkId=cur.netid;
  pProgramInfo->wTransportStreamId=cur.tsid;
  pProgramInfo->wServiceId=cur.sid;
  return 0;
}

