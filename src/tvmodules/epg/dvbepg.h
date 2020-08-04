#ifndef __DVB_EPG_H__
#define __DVB_EPG_H__
#include <ngl_types.h>
#include <ngl_tuner.h>
#include <si_table.h>
#include <satellite.h>
#include <favorite.h>
#include <tvdata.h>
#include <vector>
NGL_BEGIN_DECLS

typedef void(*DTV_TP_CBK)(const TRANSPONDER*tp,int idx,int tpcount,void*userdata);
typedef void(*DTV_ADVANCE_CBK)(USHORT netid,USHORT tsid,std::vector<PSITable>&pat,std::vector<PSITable>&pmt,std::vector<PSITable>&sdt,void*userdata);

#define MSG_STREAM_CHANGE  0x3150
#define MSG_SERVICE_CHANGE 0x3151
#define MSG_EPG_PF         0x3152 
#define MSG_EPG_SCHEDULE   0x3153
#define MSG_BAT_CHANGE     0x3154
#define MSG_CAT_CHANGE     0x3155
#define MSG_PMT_CHANGE     0x3156

typedef void (*DTV_NOTIFY)(UINT msgtype,const SERVICELOCATOR*svc,DWORD wp,ULONG lp,void*userdata);

typedef struct{
   DTV_TP_CBK  NEWTP;
   DTV_TP_CBK  FINISHTP;
   DTV_SERVICE_CBK SERVICE_CBK;    
   DTV_ADVANCE_CBK ADVANCE;//for advance usage
   void*userdata;
}SEARCHNOTIFY;

/*
  tunerparams:array of NGLTunerParam used to tunning,cant be null
  size:element count of tunerparams size<=0 search by NIT otherwise ,seach each element in array tunerparams
  notify:used to recv search callback
  for normal usage ,we can use TP_CBK and SVC_CBK
  for advance usage, we can use ADVANCE_CBK,ie:SI Parser App
*/
int DtvEpgInit();
void DtvSearch(const TRANSPONDER*tunerparams,int size,SEARCHNOTIFY*notify);

DWORD DtvRegisterNotify(DTV_NOTIFY notify,void*userdata);
void DtvUnregisterNotify(DWORD handle);

INT DtvGetServicePidInfo(const SERVICELOCATOR*sloc,ELEMENTSTREAM*es,USHORT*pcr);
INT DtvEnumService(DTV_SERVICE_CBK cbk,void*userdata);
void DtvGetCurrentService(SERVICELOCATOR*sloc);

INT DtvGetServicePmt(const SERVICELOCATOR*sloc,BYTE*pmt);

INT DtvRemoveStreamByTP(int tpid);

INT DtvPlay(SERVICELOCATOR*loc,const char*lan);
INT DtvTuneByService(const SERVICELOCATOR*loc);
INT DtvGetTPByService(const SERVICELOCATOR*loc,TRANSPONDER*tp);

INT DtvGetPFEvent(const SERVICELOCATOR*sloc,DVBEvent*pf);
INT DtvGetEvents(const SERVICELOCATOR*sloc,DVBEvent*evts,int evt_size);
NGL_END_DECLS

INT DtvGetEvents(const SERVICELOCATOR*sloc,std::vector<DVBEvent>&evts);

#endif
