#include <dtveventsource.h>
#include <ngl_log.h>
#include <view.h>
#include <dvbepg.h>
NGL_MODULE(DTVEVENTSOURCE);

namespace nglui{

  DtvEventSource*DtvEventSource::mInst=nullptr;
  static void TVMSG_CBK(UINT msg,const SERVICELOCATOR*svc,DWORD wp,ULONG lp,void*userdata){
      DtvEventSource*source=(DtvEventSource*)userdata;
      if(svc)source->push_event(*svc,msg);
  }
  DtvEventSource::DtvEventSource():EventSource(){
      msgmap[MSG_STREAM_CHANGE]=0;
      msgmap[MSG_SERVICE_CHANGE]=1;
      msgmap[MSG_EPG_PF]=2;
      msgmap[MSG_EPG_SCHEDULE]=3;
      DtvRegisterNotify(TVMSG_CBK,this);
  }
  DtvEventSource*DtvEventSource::getInstance(){
      if(nullptr==mInst)
          mInst=new DtvEventSource();
      return mInst;
  }
  bool DtvEventSource::dispatch(EventHandler &func){
      process();
      return func(*this);
  }

  bool DtvEventSource::prepare(int&){
      return events.size()>0;
  }

  bool DtvEventSource::check(){
      return events.size()>0;
  }
  void DtvEventSource::push_event(const SERVICELOCATOR&svc,int msg){
      int64_t s64=(svc.netid<<48)|(svc.tsid<<32)|(svc.sid<<16)|svc.tpid;
      events[s64]|=(1<<msgmap[msg]);
  }
  void DtvEventSource::process(){
      if(events.size()==0)return;
      auto itr=events.begin();
      int64_t svc=itr->first;
      int msg=itr->second;
      events.erase(itr);
      if(msg&1)dispatchMessage(MSG_STREAM_CHANGE,svc>>32,svc);
      if(msg&2)dispatchMessage(MSG_SERVICE_CHANGE,svc>>32,svc);
      if(msg&4)dispatchMessage(MSG_EPG_PF,svc>>32,svc);
      if(msg&8)dispatchMessage(MSG_EPG_SCHEDULE,svc>>32,svc);
  }
  void DtvEventSource::dispatchMessage(DWORD msg,DWORD wp,ULONG lp){
      for(v:views){
          v->sendMessage(msg,wp,lp);
      }
  }
  void DtvEventSource::registe(View*v){
      views.emplace(v);
  }
  void DtvEventSource::unregiste(View*v){
      views.erase(v);
  }
}

