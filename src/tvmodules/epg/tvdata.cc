#include <tvdata.h>
#include <vector>
#include <lrucache.h>
#include <dvbepg.h>
#include <algorithm>
#include <vector>
#include <ngl_log.h>
#include <ngl_dmx.h>
#include <favorite.h>
//#include <json/json.h>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include <iostream>
#include <fstream>
#include <map>
#include <mutex>
#include <thread>

NGL_MODULE(TVDATA);

namespace std{
    template<>
    struct hash<SERVICELOCATOR>{
        size_t operator()(SERVICELOCATOR const& s) const{
            int64_t key=((int64_t)s.netid<<32)|(s.tsid<<16)|s.sid;
            key = (~key) + (key << 18);
            key = key ^ (key >> 31);
            key = key * 21;
            key = key ^ (key >> 11);
            key = key + (key << 6);
            key = key ^ (key >> 22);
            return (size_t)key;
        }
    };
}

static SECTIONLIST epgpf;
static SECTIONLIST bats;
typedef lru::Cache<SERVICELOCATOR,SECTIONLIST,std::mutex > EPGCache;
typedef EPGCache::node_type EPGNode;
static EPGCache epgs_cache(128,1);
static STREAMLIST gStreams;
static std::mutex mtx_seclist;

static int GetMemory(SECTIONLIST lst){
   int memsize=0;
   for(auto s=lst.begin();s!=lst.end();s++){
      memsize+=(*s).sectionLength()+3;
   }
   return memsize;
}

static int GetMemories(){
   int memsize=0;
   auto fsum=[&](const EPGNode &n){
       memsize+=GetMemory(n.value);
   };
   epgs_cache.cwalk(fsum);
   return memsize;
}

int AddEITPFSection(const EIT&eit,int *pchanged){
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    SECTIONLIST::iterator itr=std::find(epgpf.begin(),epgpf.end(),eit);
    int changed=(itr==epgpf.end());
    if(changed){
        epgpf.push_back(eit);
        std::sort(epgpf.begin(),epgpf.end(),std::less<PSITable>());
    }
    if(pchanged)*pchanged=changed;
    const SERVICELOCATOR loc=eit;
    NGLOG_VERBOSE_IF(changed,"EITPF %d.%d.%d secs=%d changed=%d",loc.netid,loc.tsid,loc.sid,epgpf.size(),changed);
    return epgpf.size();
}

int AddEITSSection(const EIT &eit,int*pchanged){
    const SERVICELOCATOR loc=eit;
    SECTIONLIST secs;
    if(epgs_cache.tryGet(loc,secs)){ 
         SECTIONLIST::iterator itr=std::find(secs.begin(),secs.end(),eit);
         int changed=(itr==secs.end());
         if(changed){
              secs.push_back(eit);
              std::sort(secs.begin(),secs.end(),std::less<PSITable>());
         }else{
              changed=(itr->crc32()!=eit.crc32());
              *itr=eit;
         }
         if(pchanged)*pchanged=changed;
         NGLOG_VERBOSE_IF(changed,"%d.%d.%d/%d %d sections memsize=%d/%d changed=%d/%d",loc.netid,loc.tsid,loc.sid,eit.sectionNo(),
             secs.size(),GetMemory(secs),GetMemories(),changed,eit.crcOK());
         return secs.size();
    }else{
         NGLOG_VERBOSE("create cache for EITS  %d.%d.%d cache.size=%d",loc.netid,loc.tsid,loc.sid,epgs_cache.size());
         secs.push_back(eit);
         epgs_cache.insert(loc,secs);
         if(pchanged)*pchanged=true;
         return 1;
    }
    return 0;
}

int AddBATSection(const BAT&bat,int*pchanged){
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    SECTIONLIST::iterator itr=std::find(bats.begin(),bats.end(),bat);
    bool changed=(itr==bats.end());
    if(changed){
         bats.push_back(bat);
         NGLOG_VERBOSE("addbat tableid %x %d.%d",bat.tableId(),bat.extTableId(),bat.sectionNo());
    }
    if(pchanged)*pchanged=changed;
    std::sort(bats.begin(),bats.end(),std::less<PSITable>());
    NGLOG_VERBOSE_IF(changed,"rcv BAT %d",bat.extTableId()); 
    return bats.size();
}

STREAMDB*FindStream(USHORT nid,USHORT tsid){
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    for(STREAMLIST::iterator ts=gStreams.begin();ts!=gStreams.end();ts++){//auto ts:gStreams){
        if( (ts->netid==nid) && (ts->tsid==tsid) )
           return &(*ts);
    }
    return nullptr;
}
int AddStreamDB(const STREAMDB&ts){
    STREAMDB*find=FindStream(ts.netid,ts.tsid);
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    if(find){
         NGLOG_ERROR("stream %d.%d exists! add failed",ts.netid,ts.tsid);
         return NGL_ERROR;
    }else{
        if(ts.pmt.size()&&ts.pat.size())
            gStreams.push_back(ts);
    }
    return NGL_OK;
}

int UpdateStreamData(SERVICELOCATOR*svc,BYTE*sec){
    STREAMDB*ts=FindStream(svc->netid,svc->tsid);
    SECTIONLIST::iterator itr;
    PSITable tbl(sec);
    std::unique_lock<std::mutex> lck(mtx_seclist);
    switch(sec[0]){
    case TBID_PAT:
        itr=std::find(ts->pat.begin(),ts->pat.end(),tbl);
        if(itr==ts->pat.end())
           ts->pat.push_back(tbl);
        else *itr=tbl;
        break;
    case TBID_PMT:
        itr=std::find(ts->pmt.begin(),ts->pmt.end(),tbl);
        if(itr==ts->pat.end())
           ts->pmt.push_back(tbl);
        else *itr=tbl;
        break;
    case TBID_SDT:
        itr=std::find(ts->sdt.begin(),ts->sdt.end(),tbl);
        if(itr==ts->sdt.end())
           ts->sdt.push_back(tbl);
        else *itr=tbl;
        break;
    dwefault:break;
    }
}

static size_t SaveSectionList(FILE*f,SECTIONLIST&seclst){
    size_t size=0;
    for(auto s:seclst){
         const BYTE*sec=s;
         NGLOG_DUMP("TABLE:",sec,16);
         size_t sz=fwrite(sec,s.sectionLength()+3,1,f);
         size+=sz;
    }
    return size;
}

int DtvLoadProgramsData(const char*fname){
    FILE*f=fopen(fname,"rb");
    char header[8];
    unsigned char sec[4096];
    unsigned short seclen;
    int stream_count;
    if(NULL==f)return 0;
    fread(header,8,1,f);
    if(strncmp(header,"NGL.DATA",8))return 0;
    fread(&stream_count,sizeof(int),1,f);
    NGLOG_DEBUG("%s streams.size=%d tune.size=%d",fname,stream_count,sizeof(NGLTunerParam));
    for(int i=0;i<stream_count;i++){
        STREAMDB ts;
        fread(&ts.netid,sizeof(USHORT),1,f);
        fread(&ts.tsid,sizeof(USHORT),1,f);
        fread(&ts.tune,sizeof(NGLTunerParam),1,f);
        do{
            PSITable psi(sec,false);
            fread(sec,4,1,f);
            NGLOG_VERBOSE("TABLE:%x %d.%d",psi.tableId(),psi.extTableId(),psi.sectionNo());
            if(psi.tableId()==0xFF)break;
            seclen=((sec[1]&0x0F)<<8)|sec[2];
            fread(sec+4,psi.sectionLength()-1,1,f);
            switch(psi.tableId()){
            case TBID_PAT:ts.pat.push_back(psi);break;
            case TBID_PMT:ts.pmt.push_back(psi);break;
            case TBID_SDT:
            case TBID_SDT_OTHER:ts.sdt.push_back(psi);break;
            default:NGLOG_ERROR("unknown tableid %x",sec[0]);
            }
        }while(!feof(f));
        NGLOG_DEBUG("load ts %d.%d deliverytype=%d tpid=%d freq:%d sdt.size=%d pmt.size=%d",ts.netid,ts.tsid,
             ts.tune.delivery_type,ts.tune.u.s.tpid,ts.tune.u.s.frequency,ts.sdt.size(),ts.pmt.size());
        gStreams.push_back(ts);
    }
    do{
         BAT bat(sec,false);
         fread(sec,4,1,f);
         if(bat.tableId()==0xFF)break;
         NGLOG_VERBOSE("BAT:%x %d.%d",bat.tableId(),bat.extTableId(),bat.sectionNo());
         fread(sec+4,bat.sectionLength()-1,1,f);
         AddBATSection(bat,NULL);
    }while(!feof(f));
    fclose(f);
    NGLOG_DEBUG("%d streams %d bat Loaded",gStreams.size(),bats.size());
    return gStreams.size();
}

int DtvSaveProgramsData(const char*fname){
    FILE*f=fopen(fname,"wb");
    const char *header="NGL.DATA";
    int stream_count=gStreams.size();
    int section_end=0xFFFFFFFF;
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    if(NULL==f)return 0;
    fwrite(header,8,1,f);
    fwrite(&stream_count,sizeof(int),1,f);
    for(auto g:gStreams){
        fwrite(&g.netid,sizeof(USHORT),1,f);
        fwrite(&g.tsid,sizeof(USHORT),1,f);
        fwrite(&g.tune,sizeof(NGLTunerParam),1,f);
        SaveSectionList(f,g.pat);
        SaveSectionList(f,g.pmt);
        SaveSectionList(f,g.sdt); 
        fwrite(&section_end,sizeof(int),1,f);
    }
    SaveSectionList(f,bats);
    NGLOG_DEBUG("save %d streams %d bat",gStreams.size(),bats.size());
    fwrite(&section_end,sizeof(int),1,f);
    fclose(f);
    return gStreams.size();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ISDVBS(x) (((x)==DELIVERY_S)||((x)==DELIVERY_S2))
INT DtvGetTPByService(const SERVICELOCATOR*loc,TRANSPONDER*tp){
    memset(tp,0,sizeof(NGLTunerParam));
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    for(auto t:gStreams){
         if( (t.netid==loc->netid) &&(t.tsid==loc->tsid) &&
            ((ISDVBS(t.tune.delivery_type)&&(t.tune.u.s.tpid==loc->tpid))||!ISDVBS(t.tune.delivery_type))){
             *tp=t.tune;
             NGLOG_VERBOSE("%d.%d.%d.%d freq=%d type=%d tpid=%d",loc->netid,loc->tsid,loc->sid,loc->tpid,tp->u.s.frequency,tp->delivery_type,tp->u.s.tpid);
             return NGL_OK;
         }
    }
    return NGL_ERROR;
}

INT DtvTuneByService(const SERVICELOCATOR*loc){
    SERVICELOCATOR cur;
    NGLTunerParam tp;
    DtvGetCurrentService(&cur);
    DtvGetTPByService(loc,&tp);
    if(cur.netid!=loc->netid||cur.tsid!=loc->tsid){
         NGLOG_INFO("tunning to ts %d.%d %d",loc->netid,loc->tsid,tp.u.s.frequency);
         if(tp.delivery_type==DELIVERY_S||tp.delivery_type==DELIVERY_S2)
            ConfigureTransponder(&tp);
         nglTunerLock(0,&tp);
    }
    return NGL_OK;
}


INT DtvRemoveStreamByTP(int tpid){
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    for(auto t=gStreams.begin();t!=gStreams.end();t++){
        if( (t->tune.delivery_type==DELIVERY_S||t->tune.delivery_type==DELIVERY_S2) && t->tune.u.s.tpid==tpid){
            gStreams.erase(t);
            return 0;
        }
    }
    return -1;
}

INT DtvEnumTS(TS_CBK cbk,void*userdata){
    for(auto ts:gStreams){
       cbk(ts,userdata);
    } 
}

const PSITable* FindPmtInTS(const STREAMDB&ts,USHORT serviceid){
    for(auto itr=ts.pmt.begin();itr!=ts.pmt.end();itr++){
        if(itr->extTableId()==serviceid){
           return &(*itr);
        } 
    }
    return nullptr;
}

INT DtvEnumTSService(const STREAMDB&ts,DTV_SERVICE_CBK cbk,void*userdata){
    int rc=0;
    NGLOG_DEBUG("Streams %d.%d pat=%d sdt=%d pmt=%d",ts.netid,ts.tsid,ts.pat.size(),ts.sdt.size(),ts.pmt.size());
    SERVICELOCATOR loc;
    if(ts.sdt.size()){
        char sname[128]={0};
        for(auto itr_sdt:ts.sdt){
            SDT sdt(itr_sdt);
            DVBService svcs[32];
            loc.netid=sdt.getNetId();
            loc.tsid=sdt.getStreamId();
            loc.tpid=ISDVBS(ts.tune.delivery_type)?ts.tune.u.s.tpid:0xFFFF;
            int cnt=sdt.getServices(svcs,false);
            NGLOG_VERBOSE("service count=%d",cnt);
            for(int i=0;i<cnt;i++){
                svcs[i].getServiceName(sname);
                loc.sid=svcs[i].service_id;
                const PSITable*pmt=FindPmtInTS(ts,loc.sid);
                NGLOG_VERBOSE("%d.%d.%d.%d  %s pmt=%p",loc.netid,loc.tsid,loc.sid,loc.tpid,sname,pmt);
                NGLOG_ERROR_IF(nullptr==pmt,"%d.%d.%d's PMT is not found",loc.netid,loc.tsid,loc.sid);
                if(NULL==pmt)
                    rc+=(0!=cbk(&loc,svcs+i,NULL,userdata));
                else
                    rc+=(0!=cbk(&loc,svcs+i,*pmt,userdata));
            }
        }
    }else if( ts.pat.size() && ts.pmt.size() ){
        PAT pat(ts.pat.front());
        loc.tsid=pat.getStreamId();
        loc.tpid=ISDVBS(ts.tune.delivery_type)?ts.tune.u.s.tpid:0xFFFF;
        for(auto itr_pmt:ts.pmt){
            PMT pmt(itr_pmt);
            DVBService svc;
            loc.sid=pmt.getProgramNumber();
            rc+=(0!=cbk(&loc,&svc,pmt,userdata));
        }
    }
    return rc;
}
INT DtvEnumService(DTV_SERVICE_CBK cbk,void*userdata){
    int rc=0;
    NGLOG_VERBOSE("%d streams",gStreams.size());
    for(auto itr_ts:gStreams ){
        rc+=DtvEnumTSService(itr_ts,cbk,userdata);
    }
    return rc;
}

static void PMT_CBK(HANDLE filter,const BYTE *Buffer,UINT BufferLength, void *UserData){
    void**params=(void**)UserData;
    memcpy(params[1],Buffer,BufferLength);
    nglSetEvent((HANDLE)params[0]);
    NGLOG_DEBUG("rcv PMT");
}
static int GetPMT(USHORT pid,USHORT sid,BYTE*buffer){
    BYTE mask[8],match[8];
    mask[0]=0xFF;  match[0]=0x02;mask[1]=mask[2]=0;
    mask[3]=0xFF;  match[3]=sid>>8;
    mask[4]=0xFF;  match[4]=sid&0xFF;
    HANDLE hevt=nglCreateEvent(0,0);
    void *params[2];
    params[0]=(void*)hevt;
    params[1]=buffer;
    HANDLE flt=nglAllocateSectionFilter(0,pid,PMT_CBK,params,DMX_SECTION);
    nglSetSectionFilterParameters(flt,mask,match,5);
    nglStartSectionFilter(flt);
    int rc=nglWaitEvent(hevt,5000);
    nglFreeSectionFilter(flt);
    nglDestroyEvent(hevt);
    return rc;
}

INT DtvGetServerPmtPid(const SERVICELOCATOR*sloc){
    USHORT pmtpid=PID_INVALID;
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    NGASSERT(sloc);
    for(auto itr_ts:gStreams ){
        NGLOG_ERROR_IF(itr_ts.pat.size()==0,"TS %d.%d pat.size=%d",itr_ts.netid,itr_ts.tsid,itr_ts.pat.size());
        if(itr_ts.netid==sloc->netid&&itr_ts.tsid==sloc->tsid){
             PAT pat(itr_ts.pat.front());
             pmtpid=pat.getPMTPID(sloc->sid);
             break;
        }
    }
    return pmtpid;
}


INT DtvGetServicePmt(const SERVICELOCATOR*sloc,BYTE*pmtbuf){
    USHORT pmtpid=0;
    STREAMDB *ts=nullptr;
    if(NULL==sloc||NULL==pmtbuf)return 0;
    NGLOG_DEBUG("%d.%d.%d streams.size=%d",sloc->netid,sloc->tsid,sloc->sid,gStreams.size());
    pmtpid=DtvGetServerPmtPid(sloc);
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    for(auto itr_ts:gStreams ){
        if(itr_ts.netid==sloc->netid&&itr_ts.tsid==sloc->tsid){
             ts=&itr_ts;
             PAT pat(itr_ts.pat.front());
             pmtpid=pat.getPMTPID(sloc->sid);
             const PSITable*section=FindPmtInTS(*ts,sloc->sid);
             if(section){
                  memcpy(pmtbuf,*section,section->sectionLength()+3);
                  return  section->sectionLength()+3;
             } 
             return 0;
        }
    }
    if(ts&&pmtpid!=0&&pmtpid!=0x1FFF){
        int rc=GetPMT(pmtpid,sloc->sid,pmtbuf);
        NGLOG_DEBUG("service %d.%d.%d pmt not found ,try getting from live stream",sloc->netid,sloc->tsid,sloc->sid);
        if(NGL_OK==rc){
            PMT psi(pmtbuf,false);
            ts->pmt.push_back(psi);
            return psi.sectionLength()+3;
        }
    }
    NGLOG_DEBUG("%d.%d.%d stream.founded",sloc->netid,sloc->tsid,sloc->sid);
    return 0;
}

INT DtvGetServicePidInfo(const SERVICELOCATOR*sloc,ELEMENTSTREAM*es,USHORT*pcr){
    BYTE pmtbuf[1024];
    if(DtvGetServicePmt(sloc,pmtbuf)>0){
        PMT pmt(pmtbuf);
        if(pcr)*pcr=pmt.pcrPid();
        return pmt.getElements(es,true);
    }
    return 0;
}

INT DtvGetTeletexts(const SERVICELOCATOR*sloc,TeletextItem*items){
    BYTE* section=new BYTE[1024];
    if(DtvGetServicePmt(sloc,section)>0){
       PMT pmt(section);
       return pmt.getTeletexts(items);
    }
    return 0;
}

INT DtvGetSubtitles(const SERVICELOCATOR*sloc,SubtitleItem*items){
    BYTE* section=new BYTE[1024];
    if(DtvGetServicePmt(sloc,section)>0){
       PMT pmt(section);
       return pmt.getSubtitles(items);
    }
    return 0;
}

static BOOL Stream_CategoryFilter(const ELEMENTSTREAM*es,void*userdata){
    return es->getCategory()==(long)userdata;
}

static BOOL Stream_TypeFilter(const ELEMENTSTREAM*es,void*userdata){
    return es->stream_type==(long)userdata;
}

static ELEMENT_FILTER StreamFilters[]={
    Stream_TypeFilter,
    Stream_CategoryFilter
};

INT DtvGetServiceElements(const SERVICELOCATOR*sloc,ELEMENTSTREAM*es,ELEMENT_FILTER fn,void*userdata){
    BYTE pmtbuf[1024];
    if((long)fn>0&&(long)fn<=sizeof(StreamFilters)/sizeof(ELEMENT_FILTER))
        fn=StreamFilters[((long)fn)-1];
    if(DtvGetServicePmt(sloc,pmtbuf)>0){
        PMT pmt(pmtbuf,false);
        ELEMENTSTREAM ess[32];
        int idx=0,cnt=pmt.getElements(ess,false);
        for(int i=0;i<cnt;i++){
            NGLOG_VERBOSE("pid=%d[%d] %s cat=%d",ess[i].pid,ess[i].stream_type,ess[i].iso639lan,ess[i].getCategory());
            if((fn==NULL)||fn(ess+i,userdata)){
                es[idx].pid=ess[i].pid;
                es[idx].stream_type=ess[i].stream_type;
                memcpy(es[idx].iso639lan,ess[i].iso639lan,sizeof(ess[i].iso639lan));
                es[idx].setDescriptor(ess[i],ess[i].getLength(),true);
                idx++;
            }
        }
        NGLOG_DEBUG("svc:%d.%d.%d found %d/%d with filter=%p data=%p",sloc->netid,sloc->tsid,sloc->sid,idx,cnt,fn,userdata);
        return idx;
    }
    return 0;
}

INT DtvGetPFEvent(const SERVICELOCATOR*sloc,DVBEvent*p){
    int rc=0;
    int flags[3]={0,1,3};
    std::unique_lock<std::mutex> lck(mtx_seclist); 
    for(auto itr:epgpf){
        EIT eit(itr);
        if(sloc->netid==eit.getNetId()&&sloc->tsid==eit.getStreamId()&&sloc->sid==eit.getServiceId()){
            int cnt=eit.getEvents(p,true);
            if(eit.sectionNo()==0){
               rc|=flags[cnt];
            }
            if(eit.sectionNo()==1){
               rc|=2;
            }p+=cnt;
        }
    }
    return rc;
}

INT DtvGetEvents(const SERVICELOCATOR*loc,std::vector<DVBEvent>&evts){
    int rc=0;
    SECTIONLIST secs;
    if(epgs_cache.tryGet(*loc,secs)){
       //NGLOG_VERBOSE("EITS %d.%d.%d secs=%d ",loc->netid,loc->tsid,loc->sid,secs->size());
       for(auto t:secs){
           EIT eit(t);
           SERVICELOCATOR l=eit;
           DVBEvent es[64];
           int cnt=eit.getEvents(es,false);
           for(int i=0;i<cnt;i++){
               evts.push_back(es[i]);
           }
       };
       return evts.size();
    }else{
        NGLOG_VERBOSE("no EITS  %d.%d.%d",loc->netid,loc->tsid,loc->sid);
       return 0;
    }
}

INT DtvGetEvents(const SERVICELOCATOR*loc,DVBEvent*evts,int evt_size){
    int rc=0;
    SECTIONLIST secs;
    if(epgs_cache.tryGet(*loc,secs)){
       NGLOG_VERBOSE("EITS %d.%d.%d secs=%d ",loc->netid,loc->tsid,loc->sid,secs.size());
       for(auto t:secs){
           EIT eit(t);
           int cnt=eit.getEvents(evts+rc,true);
           rc+=cnt;
           if(rc>=evt_size)break;
       }
       return rc;
   }else{
       NGLOG_VERBOSE("no EITS  %d.%d.%d",loc->netid,loc->tsid,loc->sid);
       return 0;
   }
}


INT DtvCreateGroupByBAT(){
    int count=0;
    NGLOG_DEBUG("bats.size=%d",bats.size());
    SERVICELOCATOR *svcs=new SERVICELOCATOR[256];
    for(auto sec:bats){
        BAT b(sec);
        INT numts=0;
        char name[256]={0};
        int numsvc=0;
        b.getName(name,NULL);
        DVBStream tss[32];
        numts=b.getStreams(tss,false);
        UINT favid=(GROUP_BAT<<16)|b.extTableId();
        if(numts==0)continue;
        FavAddGroupWithID(favid,name);
        count++;
        for(int i=0;i<numts;i++){
            int sc=tss[i].getServices(svcs);
            numsvc+=sc;
            for(int j=0;j<sc;j++)
                 FavAddService(favid,&svcs[j]);
        }
        NGLOG_VERBOSE("bouquetid[%d]:%s has %d ts %d svc ",b.extTableId(),name,numts,numsvc);
    }
    delete []svcs;
    return count;
}

class ServiceData:public DVBService{
public:
   BOOL deleted;
   BOOL visible;
   USHORT lcn;
   ServiceData(const BYTE*buf,INT len,bool deep=true):DVBService(buf,len,deep){
       lcn=0xFFFF;
       deleted=FALSE;
       visible=TRUE;
   }
   ServiceData(const DVBService&b):DVBService(b){
       lcn=0xFFFF;
       deleted=FALSE;
   }
};

static std::unordered_map<SERVICELOCATOR,ServiceData*>service_lcn;
static USHORT lcnmask=0xFFFF;
INT DtvGetServiceItem(const SERVICELOCATOR*svc,SERVICE_KEYITEM item,INT*value){
    NGASSERT(svc);
    std::unordered_map<SERVICELOCATOR,ServiceData*>::const_iterator got=service_lcn.find(*svc);
    if(got==service_lcn.end()||(value==NULL))
        return NGL_ERROR;
    switch(item){
    case SKI_PMTPID :*value=DtvGetServerPmtPid(svc);break;
    case SKI_VISIBLE:*value=got->second->visible;break;
    case SKI_DELETED:*value=got->second->deleted;break;
    case SKI_LCN    :*value=got->second->lcn;break;
    default:return NGL_ERROR;
    }
    return NGL_OK;
}

INT LoadServiceAdditionals(const char*fname){
    rapidjson::Document d;
    std::ifstream fin(fname);
    rapidjson::IStreamWrapper isw(fin);
    if(!fin.good())return 0;
    d.ParseStream(isw);

    for(int i=0;i<d.Size();i++){
        rapidjson::Value& jssvc=d[i];
        SERVICELOCATOR svc;
        svc.netid=jssvc["netid"].GetInt();
        svc.tsid =jssvc["tsid"].GetInt();
        svc.sid  =jssvc["sid"].GetInt();
        service_lcn[svc]->deleted=jssvc["deleted"].GetInt();//additinal data
    }
    return d.Size();
}

INT SaveServiceAdditionals(const char*fname){
    INT rc=0;
    rapidjson::Document d;
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
    std::ofstream fout(fname);
    rapidjson::OStreamWrapper out(fout);
    d.SetArray();
    std::unordered_map<SERVICELOCATOR,ServiceData*>::const_iterator itr;
    for(itr=service_lcn.begin();itr!=service_lcn.end();itr++,rc++){
        SERVICELOCATOR svc=itr->first;
        rapidjson::Value jssvc(rapidjson::kObjectType);
        jssvc["netid"]=svc.netid;
        jssvc["tsid"]=svc.tsid;
        jssvc["sid"]=svc.sid;
        jssvc["deleted"]=itr->second->deleted;//addtional data
        d.PushBack(jssvc,d.GetAllocator());
    }
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(out);
    d.Accept(writer);
    return rc;
}

const DVBService*DtvGetServiceInfo(const SERVICELOCATOR*svc){
    std::unordered_map<SERVICELOCATOR,ServiceData*>::const_iterator got=service_lcn.find(*svc);
    if(got!=service_lcn.end()){
        return got->second;
    }
    return NULL;
}

class LCNDATA{
public:
   UINT lcn_start;
   std::unordered_map<SERVICELOCATOR,USHORT>&lcnmap;
   INT getLCN(const SERVICELOCATOR&s,USHORT*lcn){
        std::unordered_map<SERVICELOCATOR,USHORT>::const_iterator got=lcnmap.find(s);
        if(got!=lcnmap.end()){
             *lcn=got->second;
             return NGL_OK;
        }return NGL_ERROR;
   }
};

static INT LCN_CBK(const SERVICELOCATOR*loc,const DVBService*s,const BYTE*pmt,void*userdata){
    LCNDATA *lcndata=(LCNDATA*)userdata;
    USHORT lcn;
    if(NGL_OK!=lcndata->getLCN(*loc,&lcn)){
       if(lcndata->lcn_start!=0xFFFFFFFF)
           service_lcn[*loc]->lcn=lcn=lcndata->lcn_start++;
    }else {
       service_lcn[*loc]->visible=lcn&0x8000;
       service_lcn[*loc]->lcn=(lcn&=0x3FF);
    }
    NGLOG_VERBOSE("\t%d.%d.%d lcn:%d lcnmask=%x",loc->netid,loc->tsid,loc->sid,lcn,lcnmask);
    return 1;
}

INT DtvInitLCN(LCNMODE mode,USHORT lcn_start){
    int count=0;
    std::vector<SERVICELOCATOR> vsvc;
    std::vector<USHORT> vlcn;
    std::unordered_map<SERVICELOCATOR,USHORT>lcnmap;
    NGLOG_DEBUG("lcn start from %d",lcn_start);
    if(mode&LCN_FROM_BAT){
        for(auto sec:bats){
            BAT b(sec);
            SERVICELOCATOR svcs[64];
            USHORT lcns[64];
            DVBStream tss[32];
            int tscount=b.getStreams(tss,false);
            int num=b.getLCN(svcs,lcns);
            if(num>0){//Nordig LCNV1
                 count+=num;
                 for(int i=0;i<num;i++){
                     vsvc.push_back(svcs[i]);
                     vlcn.push_back(lcns[i]);
                     lcnmask&=(0x7FFF&lcns[i]);
                 }
                 b.matchServices(vsvc.data(),vsvc.size());
            }else{//Nordig V2
                for(int i=0;i<tscount;i++){
                    int sc=tss[i].getLCN(svcs,lcns);
                    num+=sc;
                    for(int i=0;i<num;i++){
                        vsvc.push_back(svcs[i]);
                        vlcn.push_back(lcns[i]);
                        lcnmask&=(0x7FFF&lcns[i]);
                    }
                }
            }
        }
        NGLOG_DEBUG("%d service %d lcn",vsvc.size(),vlcn.size());
        for(int i=0;i<vsvc.size();i++){ 
            lcnmap[vsvc[i]]=vlcn[i];
            NGLOG_VERBOSE("\t%d.%d.%d-->%d visible=%d",vsvc[i].netid,vsvc[i].tsid,vsvc[i].sid,(vlcn[i]&0x3FFF)&(~lcnmask),(vlcn[i]&0x8000)!=0);
        }
    }
    LCNDATA lcndata={0xFFFFFFFF,lcnmap};
    if(mode&LCN_FROM_USER)//service with lcn has been putto lcnmap,so we enum all service,add theservice without lcn to be identified from lcnstart
      lcndata.lcn_start=lcn_start;
    DtvEnumService(LCN_CBK,&lcndata);
    NGLOG_DEBUG("lcnmask=%x LCN::fromBAT=%d fromUSER=%d",lcnmask,!!(mode&LCN_FROM_BAT),!!(mode&LCN_FROM_USER));
    return count;
}

INT DtvGetServiceByLCN(USHORT lcn,SERVICELOCATOR*loc){
    for(auto s:service_lcn){
        if(lcn==s.second->lcn){
            *loc=s.first;
            return NGL_OK;
        }
    }
    return NGL_ERROR;
}

static INT GRP_CBK(const SERVICELOCATOR*loc,const DVBService*s,const BYTE*pmt,void*userdata){
    FavAddService(FAV_GROUP_ALL,loc);
    service_lcn[*loc]=new ServiceData(*s);
    if(ISVIDEO(s->serviceType)){
         FavAddService(FAV_GROUP_AV,loc);
         FavAddService(FAV_GROUP_VIDEO,loc);
    }
    if(ISAUDIO(s->serviceType)){
         FavAddService(FAV_GROUP_AV,loc);
         FavAddService(FAV_GROUP_AUDIO,loc);
    }
    return 1;
}

void DtvCreateSystemGroups(){
    NGLOG_DEBUG("");
    DtvEnumService(GRP_CBK,NULL);
}

