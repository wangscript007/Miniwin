/*
  FILE : stub_dmx.c
  PURPOSE: This file is a stub for linking tests.
*/
#include <stdio.h>
#include "va_dmx.h"
#include <ngl_dmx.h>
#include "va_errors.h"
#include <va_setup.h>
#include <strings.h>
#include <ngl_log.h>
NGL_MODULE(ACSDMX)

#define MASK_LEN 8
#define MAX_CHANNEL 32
#define MAX_FILTER  (kVA_SETUP_NBMAX_SECTIONFILTERS)
#define UNUSED_PID -1

#define CHECK(x) x //{/*if((x)!=AUI_RTN_SUCCESS)*/NGLOG_DEBUG("%s:%d %s=%d\n",__FUNCTION__,__LINE__,#x,(x));}

typedef enum{
   FILTER_TOBEINIT=0,
   FILTER_INITED=0x01,
   FILTER_SETTED=0x02,
   FILTER_STARTED=0x04,
   FILTER_STOPPED=0x08,
}FILTERSTAT;
typedef struct{
  WORD pid;
  HANDLE handle;
  tVA_DMX_NotifyFilteredSection CallBack;
  DWORD vaHandle;
  FILTERSTAT state;
}DMXFILTER;

DMXFILTER  Filters[MAX_FILTER];

static UINT CRC32_Table[256];

static int InitCRC32(){
    #define CRC32_POLYNOMIAL 0x04C11DB7
    for (int i = 0; i< 256; i++){
        unsigned int coef32 = i << 24;
        for (int j=0; j<8; j++) {
            if (coef32 & 0x80000000)
                coef32 = ((coef32 << 1) ^ CRC32_POLYNOMIAL);
            else
                coef32 <<= 1;
        }
        CRC32_Table[i] = coef32;
    }
}

UINT GetCRC32(const BYTE*buffer, size_t size){
    unsigned int crc32 = 0xFFFFFFFF;
    unsigned int cntByte;
    if(CRC32_Table[2]==0)
       InitCRC32();
    for (cntByte = 0; cntByte < size; cntByte++){
        crc32 = (crc32 << 8 ) ^ CRC32_Table[((crc32 >> 24) ^ *buffer++) & 0xFF];
    }
    return crc32;
}

static void NGLSectionCB(HANDLE filter_handle,const BYTE *data,UINT len,void*userdata)
{
    DMXFILTER*flt=(DMXFILTER*)userdata;
    NGLOG_DEBUG_IF(flt==NULL,"invalida param");
    //If section_syntax_indicator == 1, we check CRC_32 field
    if((data[1]&0x80) && GetCRC32(data,len)){
         NGLOG_VERBOSE("SECTION %02x %02x %02x CRC ERROR:%02x%02x%02x%02x",data[0],data[3],data[4],data[len-4],data[len-3],data[len-2],data[len-1]);
         return;
    }   
    if( (NULL!=flt->CallBack) )
        flt->CallBack(flt->vaHandle,len,(BYTE*)data,NULL); 
}

void VA_DMX_Init(){
   int i;
   static int sDMX_INITED=0;
   if(sDMX_INITED!=0)
      return;
   nglDmxInit();
   InitCRC32();
   memset(Filters,0,sizeof(Filters));
   NGLOG_DEBUG("MAX_FILTER=%d",MAX_FILTER);
   sDMX_INITED++;
}
static UINT GetCountByPid(WORD pid){
   UINT i;
   UINT count=0;
   for(i=0;i<MAX_FILTER;i++){
        if(Filters[i].handle&&((Filters[i].pid==pid)||(pid>0x1FFF)))
           count++;
   }
   return count;
}
DWORD VA_DMX_AllocateSectionFilter(DWORD dwAcsId, DWORD dwVaFilterHandle, WORD  wPid,
                           tVA_DMX_NotifyFilteredSection pfVA_DMX_NotifyFilteredSection)
{
  int i;
  VA_DMX_Init();
  DMXFILTER*flt=NULL;
  if(dwAcsId>=kVA_SETUP_NBMAX_ACS||dwVaFilterHandle==kVA_ILLEGAL_HANDLE||wPid>0x1FFF)
     return kVA_ILLEGAL_HANDLE;
  for(i=0;i<MAX_FILTER;i++){
      if(0==Filters[i].handle){
          flt=Filters+i; 
          break;
      }
  }
  if( (flt==NULL)||(GetCountByPid(wPid)>=kVA_SETUP_NBMAX_FILTERS_PER_CHANNEL)
            ||(GetCountByPid(0xFFFF)>kVA_SETUP_NBMAX_SECTIONFILTERS))
      return kVA_ILLEGAL_HANDLE;
      
  flt->pid=wPid;
  flt->state=FILTER_INITED;
  flt->vaHandle=dwVaFilterHandle;
  flt->CallBack=pfVA_DMX_NotifyFilteredSection;
  flt->handle=nglAllocateSectionFilter(0,wPid,NGLSectionCB,(void*)flt,DMX_SECTION);
  NGLOG_VERBOSE("filter=%p handle=%p pid=0x%x/%d",flt,flt->handle,wPid,wPid);
  return (DWORD)flt;
}

#define CHECKFILTER(s) {if(s<Filters||s>=& Filters[MAX_FILTER]){NGLOG_DEBUG("");return kVA_INVALID_PARAMETER;}}

INT VA_DMX_FreeSectionFilter( DWORD dwStbFilterHandle )
{
  INT rc=0;
  DMXFILTER*flt=(DMXFILTER*)dwStbFilterHandle;
  CHECKFILTER(flt);
  NGLOG_VERBOSE("handle=%x/%x state=%d",flt->vaHandle,flt->handle,flt->state);
  if((flt->handle==0)||(flt->vaHandle==kVA_ILLEGAL_HANDLE))
      return kVA_INVALID_PARAMETER;

  if((flt->state&FILTER_STARTED)&&(!(flt->state&FILTER_STOPPED))){
       return kVA_STOP_FILTER_FIRST;
  }
  flt->state=FILTER_TOBEINIT;
  flt->pid=0xFFFF;
  nglFreeSectionFilter(flt->handle);
  flt->handle=0;
  return rc;
}

INT VA_DMX_SetSectionFilterParameters(DWORD dwStbFilterHandle,
                                      UINT32 uiLength, BYTE *pValue, BYTE *pMask,
                                      tVA_DMX_NotificationMode eNotificationMode)
{
  BYTE mask[16],value[16];
  DMXFILTER*flt=(DMXFILTER*)dwStbFilterHandle;
  CHECKFILTER(flt);
  NGLOG_VERBOSE("mask=%p value=%p len=%d handle=%p/%p",pMask,pValue,uiLength,flt->vaHandle,flt->handle);
  if( kVA_DMX_MAX_FILTER_SIZE<uiLength || ((NULL==pValue|| NULL==pMask)&&uiLength>0)
     ||(flt->handle==0)||(flt->vaHandle==kVA_ILLEGAL_HANDLE))
        return kVA_INVALID_PARAMETER;
  memcpy(mask,pMask,uiLength);
  memcpy(value,pValue,uiLength);
  
  BOOL needrestart=(flt->state&FILTER_STARTED)&&(!(flt->state&FILTER_STOPPED));
  if(needrestart)
      nglStopSectionFilter(flt->handle);
  nglSetSectionFilterOneshot(flt->handle,(eNotificationMode==eVA_DMX_OneShot));
  nglSetSectionFilterParameters(flt->handle,mask,value,uiLength);
  if(needrestart)
      nglStartSectionFilter(flt->handle);
  flt->state|=FILTER_SETTED;
  return 0;
}

INT VA_DMX_StartSectionFilter(DWORD  dwStbFilterHandle)
{
  DMXFILTER*flt=(DMXFILTER*)dwStbFilterHandle;
  CHECKFILTER(flt);
  NGLOG_VERBOSE("flt=%p vahdl=%p/%p",flt,flt->vaHandle,flt->handle);
  if((flt->handle==0)||(flt->vaHandle==kVA_ILLEGAL_HANDLE))
      return kVA_INVALID_PARAMETER;
  if(!(flt->state&FILTER_SETTED))
      return kVA_SET_FILTER_PARAMETERS_FIRST;
  if(flt->state&FILTER_STARTED)
      return kVA_OK;
  flt->state|=FILTER_STARTED;
  nglStartSectionFilter(flt->handle);
  return kVA_OK;
}

/**AllocSectionFilter without FreeSectionFilter ?*/
INT VA_DMX_StopSectionFilter(DWORD  dwStbFilterHandle)
{
  DMXFILTER*flt=(DMXFILTER*)dwStbFilterHandle;
  NGLOG_VERBOSE("flt=%p %p,%p",flt,Filters,&Filters[MAX_FILTER]);
  CHECKFILTER(flt);
  if((flt->handle==0)||(flt->vaHandle==kVA_ILLEGAL_HANDLE))
      return kVA_INVALID_PARAMETER;
  if(flt->state&FILTER_STOPPED)
      return kVA_OK;
  flt->state|=FILTER_STOPPED;
  nglStopSectionFilter(flt->handle);
  return 0;
}

#if (defined VAOPT_ENABLE_ACS30 || defined VAOPT_ENABLE_ACS31 || defined VAOPT_ENABLE_ACS40 || defined VAOPT_ENABLE_ACS41)
INT VA_DMX_UnlockSectionFilter(DWORD dwStbFilterHandle)
{
  DMXFILTER*flt=(DMXFILTER*)dwStbFilterHandle;
  NGLOG_DEBUG("flt=%p",flt);
  CHECKFILTER(flt);
  return 0;
}
#endif

/* End of File */
