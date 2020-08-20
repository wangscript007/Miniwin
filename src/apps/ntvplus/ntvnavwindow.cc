#include <ntvnavwindow.h>
#include <ntvcommon.h>
#include <dtveventsource.h>
NGL_MODULE(NTVNavWindow);

namespace ntvplus{

NTVNavWindow::NTVNavWindow(int x,int y,int w,int h)
  :NTVWindow(x,y,w,h){
    DtvEventSource::getInstance()->registe(this);
}

NTVNavWindow::~NTVNavWindow(){
    DtvEventSource::getInstance()->unregiste(this);
}

void NTVNavWindow::onGroup(UINT favid,const std::string&favname){
    NGLOG_DEBUG("inherited class must be implemented");
}

void NTVNavWindow::onService(const DVBService*svc){
    NGLOG_DEBUG("inherited class must be implemented");
}

void NTVNavWindow::onEventPF(const SERVICELOCATOR&svc,const DVBEvent*e,bool present){
    NGLOG_DEBUG("inherited class must be implemented");
}

void NTVNavWindow::onEvents(const SERVICELOCATOR&svc,const std::vector<DVBEvent>&evts){
    NGLOG_DEBUG("inherited class must be implemented");
}

bool NTVNavWindow::onMessage(DWORD msg,DWORD wp,ULONG lp){
    SERVICELOCATOR svc;
    svc.netid=wp>>16;
    svc.tsid=wp&0xFFFF;
    svc.sid=lp>>16;
    svc.tpid=lp&0xFFFF;
    switch(msg){
    case MSG_STREAM_CHANGE:
    case MSG_SERVICE_CHANGE:return false;
    case MSG_EPG_PF: loadEventPF(&svc);return true;
    case MSG_EPG_SCHEDULE:loadEvents(svc);return true;
    default:return NTVWindow::onMessage(msg,wp,lp);
    }
}

TVChannel*NTVNavWindow::DVBService2Channel(const DVBService*info){
    INT lcn;
    TVChannel*ch;
    char name[256];
    SERVICELOCATOR svc=*info;
    svc.tpid=0;
    info->getServiceName(name);
    ch=new TVChannel(name,&svc,info->freeCAMode);
    DtvGetServiceItem(&svc,SKI_LCN,&lcn);
    ch->setId(lcn);
    ch->isHD=ISHDVIDEO(info->serviceType);
    return ch;
}

int NTVNavWindow::loadGroups(){
     int count=FavGetGroupCount();
     for(int i=0;i<count;i++){
          char name[64];
          UINT favid;
          FavGetGroupInfo(i,&favid,name);
          onGroup(favid,name);
          NGLOG_VERBOSE("%x %s %d services",favid,name,FavGetServiceCount(favid));
     }
     return count;
}

int NTVNavWindow::loadServices(UINT favid){
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

int NTVNavWindow::loadEventPF(const SERVICELOCATOR*svc){
    SERVICELOCATOR loc;
     DVBEvent pf[2];
    if(svc)loc=*svc;
    else DtvGetCurrentService(&loc);
    int rc=DtvGetPFEvent(&loc,pf);
    if(rc&1)onEventPF(loc,&pf[0],true);
    if(rc&2)onEventPF(loc,&pf[1],false);
}

int NTVNavWindow::loadEvents(const SERVICELOCATOR&svc){
    std::vector<DVBEvent>evts;
    DtvGetEvents(&svc,evts);
    NGLOG_DEBUG("%s rcv %d events",SERVICELOCATOR2String(&svc).c_str(),evts.size());
    onEvents(svc,evts);
    return evts.size();
}

static void ChannelPainterInner(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas,bool hasid){
    TVChannel&ch=(TVChannel&)itm;
    RECT r=itm.rect;
    r.inflate(0,-1);
    canvas.save();
    if(state){
        canvas.set_source_rgba(0.6,0,0,.6);
        canvas.rectangle(r);
        canvas.fill();
    }
    canvas.restore();
    r=itm.rect;
    canvas.set_color(lv.getFgColor());

    r.width=hasid?80:20;
    r.x=20;
    if(hasid){
        canvas.draw_text(r,std::to_string(itm.getId()),DT_RIGHT|DT_VCENTER);
        r.x=r.x+r.width+20;
    }
    r.width=itm.rect.width-r.x;

    canvas.draw_text(r,itm.getText(),DT_LEFT|DT_VCENTER);
    r=itm.rect;
    r.inflate(6,0);
    DrawSplitter(canvas,r);

    r.set(r.width-r.height,r.y,r.height,r.height);
    if(ch.camode)canvas.draw_text(r,"$",DT_LEFT|DT_VCENTER);
}

void  NTVNavWindow::ChannelPainterLCN(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    ChannelPainterInner(lv,itm,state,canvas,true);
}
void  NTVNavWindow::ChannelPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    ChannelPainterInner(lv,itm,state,canvas,false);
}

}
