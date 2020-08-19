#include <ntvplayerwindow.h>

NGL_MODULE(NTVPlayerWindow);

namespace ntvplus{

NTVPlayerWindow::NTVPlayerWindow(int x,int y,int w,int h)
  :NTVWindow(x,y,w,h){
}

NTVPlayerWindow::~NTVPlayerWindow(){
}

void NTVPlayerWindow::onFavoriteGroup(UINT favid,const std::string&favname){
    NGLOG_DEBUG("inherited class must be implemented");
}

void NTVPlayerWindow::onService(const DVBService*svc){
    NGLOG_DEBUG("inherited class must be implemented");
}

void NTVPlayerWindow::onEventPF(const SERVICELOCATOR&svc,const DVBEvent*e,bool present){
    NGLOG_DEBUG("inherited class must be implemented");
}

void NTVPlayerWindow::onEvents(const SERVICELOCATOR&svc,const DVBEvent*e){
    NGLOG_DEBUG("inherited class must be implemented");
}

int NTVPlayerWindow::loadGroups(){
     int count=FavGetGroupCount();
     for(int i=0;i<count;i++){
          char name[64];
          UINT favid;
          FavGetGroupInfo(i,&favid,name);
          onFavoriteGroup(favid,name);
          NGLOG_VERBOSE("%x %s %d services",favid,name,FavGetServiceCount(favid));
     }
     return count;
}

int NTVPlayerWindow::loadServices(UINT favid){
     char name[128];
     size_t count=FavGetServiceCount(favid);
     FavGetGroupName(favid,name);
     NGLOG_DEBUG("%x[%s] has %d svc",favid,name,count);
     SERVICELOCATOR cur;
     DtvGetCurrentService(&cur);
     for(size_t i=0;i<count;i++){
          SERVICELOCATOR svc;
          FavGetService(favid,&svc,i);
          const DVBService*info=DtvGetServiceInfo(&svc);
          if(NULL==info)continue;
          onService(info);
          NGLOG_VERBOSE("   %d %d.%d.%d.%d:%s  %p type=%d",i,svc.netid,svc.tsid,svc.sid,svc.tpid,name,info,info->serviceType);
     }
     return count;
}

int NTVPlayerWindow::loadEventPF(const SERVICELOCATOR*svc){
    SERVICELOCATOR loc;
     DVBEvent pf[2];
    if(svc)loc=*svc;
    else DtvGetCurrentService(&loc);
    int rc=DtvGetPFEvent(&loc,pf);
    if(rc&1)onEventPF(loc,&pf[0],true);
    if(rc&2)onEventPF(loc,&pf[1],false);
}

int NTVPlayerWindow::loadEvents(const SERVICELOCATOR&svc){
    std::vector<DVBEvent>evts;
    DtvGetEvents(&svc,evts);
    for(auto e:evts){
       onEvents(svc,&e);
    }
    return evts.size();
}

}
