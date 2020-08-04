#ifndef __TV_DATA_H__
#define __TV_DATA_H__
#include<ngl_types.h>
#include<si_table.h>

NGL_BEGIN_DECLS
typedef void (*TS_CBK)(const STREAMDB&ts,void*userdata);
typedef INT (*DTV_SERVICE_CBK)(const SERVICELOCATOR*svcloc,const DVBService*s,const BYTE*pmt,void*userdata);
typedef BOOL (*ELEMENT_FILTER)(const ELEMENTSTREAM*,void*userdata);

int AddEITPFSection(const EIT&eit,int*changed);
int AddEITSSection(const EIT &eit,int*changed);
int AddBATSection(const BAT&bat,int*changed);

int AddStreamDB(const STREAMDB&ts);
int UpdateStreamData(SERVICELOCATOR*svc,BYTE*sec);

int DtvLoadProgramsData(const char*fname);
int DtvSaveProgramsData(const char*fname);

INT DtvEnumTS(TS_CBK cbk,void*userdata);
INT DtvEnumTSService(const STREAMDB&ts,DTV_SERVICE_CBK cbk,void*userdata);

typedef enum{
   SKI_PMTPID,
   SKI_VISIBLE,
   SKI_DELETED,
   SKI_LCN
}SERVICE_KEYITEM;

ULONG DtvGetTdTime();
INT DtvGetServiceItem(const SERVICELOCATOR*svc,SERVICE_KEYITEM item,INT*value);
const DVBService*DtvGetServiceInfo(const SERVICELOCATOR*svc);
INT DtvGetServiceByLCN(USHORT lcn,SERVICELOCATOR*loc);
INT DtvGetServerPmtPid(const SERVICELOCATOR*sloc);
INT DtvGetTeletexts(const SERVICELOCATOR*sloc,TeletextItem*items);
INT DtvGetSubtitles(const SERVICELOCATOR*sloc,SubtitleItem*items);

INT DtvGetServiceElements(const SERVICELOCATOR*sloc,ELEMENTSTREAM*es,ELEMENT_FILTER fn,void*data);
#define DtvGetElementsByStreamType(sloc,type,es) DtvGetServiceElements(sloc,es,(ELEMENT_FILTER)1,(void*)type)
#define DtvGetElementsByCategory(sloc,type,es) DtvGetServiceElements(sloc,es,(ELEMENT_FILTER)2,(void*)type)

typedef enum{
  LCN_FROM_NIT=1,
  LCN_FROM_BAT=2,
  LCN_FROM_USER=4
}LCNMODE;

INT DtvInitLCN(LCNMODE mode,USHORT lcnstart);

INT LoadServiceAdditionals(const char*fname);
INT SaveServiceAdditionals(const char*fname);

INT DtvCreateGroupByBAT();
void DtvCreateSystemGroups();

NGL_END_DECLS

#endif
