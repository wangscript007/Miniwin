#include <windows.h>
#include <appmenus.h>
#include <ngl_log.h>
#include <dvbepg.h>
#include <ngl_video.h>
#include <channelepgview.h>
#include <ruleview.h>

NGL_MODULE(TVGUIDE);

namespace ntvplus{

class TVChannel:public nglui::ChannelBar{
public:
   SERVICELOCATOR svc;
   int update;
   BOOL isHD;
public:
   TVChannel(const std::string&txt,const SERVICELOCATOR*svc_):ChannelBar(txt){
       svc=*svc_;
       update=1;
   }
};

#define MSG_EPGS_UPDATE 1010
#define CHANNEL_HEIGHT 46

class TVWindow:public NTVWindow{
protected:
   TextView*prg_title;
   TextView*prg_time;
   TextView*prg_dsc;
   ChannelEpgView *lv;
   RuleView*rule;
   ULONG tdt_time;
public:
   TVWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
       lv=new ChannelEpgView(1240,CHANNEL_HEIGHT*7);
       lv->setPos(20,336).setBgColor(0);
       lv->setChannelNameWidth(320);
       lv->setFlag(BORDER);
       lv->setItemSize(0,CHANNEL_HEIGHT);
       addChildView(lv)->setBgColor(0);
       lv->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
           TVChannel&itm=(TVChannel&)lvitem;
           DtvPlay(&itm.svc,nullptr);
           NGLRect rcv={30,70,320,240};
           nglAvSetVideoWindow(0,NULL,&rcv);
       });
       tdt_time=0;
       rule=new RuleView(1240,24);
       rule->setOffset(lv->getChannelNameWidth());
       rule->setFontSize(12);
       lv->setPixelPerMinute(2);
       rule->setInterval(lv->getPixelPerMinute()*6);
       rule->setLabeListener([](RuleView&v,int idx,std::string&label){
           if(idx%10==0){
               char tbuf[16];
               sprintf(tbuf,"%02d:00",(v.getMin()+idx/10)%24);
               label=tbuf;
           }else label="";
       });
       addChildView(rule)->setPos(20,310).setBgColor(0x60406080);

       prg_title=new TextView("ProgramTitle",920,38);
       prg_title->setFontSize(35);
       prg_title->setFgColor(0xFFFFFFFF);

       prg_time=new TextView("Program time&duration",870,30);
       prg_time->setFgColor(0xFFFFFFFF);

       prg_dsc=new TextView("Program description",870,150);
       prg_dsc->setFgColor(0xFFFFFFFF);

       addChildView(prg_title)->setPos(360,68).setBgColor(0x80000000);
       addChildView(prg_time)->setPos(360,108).setBgColor(0);
       addChildView(prg_dsc)->setPos(360,130).setBgColor(0);

       lv->setEventChangeListener([&](ChannelEpgView&,const TVEvent&e){
           char tmbuf[32];
           struct tm te;
           prg_title->setText(e.name);
           gmtime_r(&e.start,&te);
           sprintf(tmbuf,"%d/%d/%d %02d:%02d:%02d [%02d:%02d]",te.tm_year+1900,te.tm_mon+1,te.tm_mday,te.tm_hour,te.tm_min,te.tm_sec,
               (int)(e.duration/3600),(int)((e.duration%3600)/60));
           prg_time->setText(tmbuf);
           prg_dsc->setText(e.desc);
       });
       lv->setTimeChangeListener([&](ChannelEpgView&view){
           struct tm ts;
           char buf[64];
           time_t tnow=view.getStartTime();
           gmtime_r(&tnow,&ts);
           sprintf(buf,"%d/%02d/%02d %02d:%02d",ts.tm_year+1900,ts.tm_mon+1,ts.tm_mday,ts.tm_hour,ts.tm_min);
           if(rule){
               rule->setMin(ts.tm_hour);
               rule->setText(buf);
               NGLOG_DEBUG("rule.setmim(%d/%d)",ts.tm_hour,rule->getMin());
           }
       });
       clearFlag(ATTR_ANIMATE_FOCUS);
       sendMessage(MSG_EPGS_UPDATE,0,0,1000);
   }

   ~TVWindow(){
       nglAvSetVideoWindow(0,NULL,NULL);
   }

   virtual void onDraw(GraphContext&c){
       NTVWindow::onDraw(c);
       c.save();
       c.set_operator(Context::Operator::CLEAR);
       c.set_color(0);
       c.rectangle(30,70,320,240);
       c.fill();
       c.restore();
   }
   int loadEvents(TVChannel*ch);
   int loadServices(UINT favid);
   virtual void onEITS(const SERVICELOCATOR*svc)override;
   virtual bool onKeyUp(KeyEvent&k)override;
   virtual bool onMessage(DWORD msg,DWORD wp,ULONG lp)override;
};

int TVWindow::loadServices(UINT favid){
     char name[128];
     size_t count=FavGetServiceCount(favid);
     SERVICELOCATOR*svcs=new SERVICELOCATOR[count];
     FavGetGroupName(favid,name);
     NGLOG_DEBUG("%x[%s] has %d svc",favid,name,count);
     lv->clearAllItems();
     SERVICELOCATOR cur;
     DtvGetCurrentService(&cur);
     for(size_t i=0;i<count;i++){
          SERVICELOCATOR svc;
          FavGetService(favid,&svc,i);
          const DVBService*info=DtvGetServiceInfo(&svc);
          if(NULL==info)continue;
          info->getServiceName(name);
          TVChannel*ch=new TVChannel(name,&svc);//info->freeCAMode);
          INT lcn;
          DtvGetServiceItem(&svc,SKI_LCN,&lcn);
          ch->setId(lcn);
          ch->isHD=ISHDVIDEO(info->serviceType);
          NGLOG_VERBOSE("  %d %d.%d.%d.%d:%s  %p hd=%d type=%d",i,svc.netid,svc.tsid,svc.sid,svc.tpid,name,info,ch->isHD,info->serviceType);
          lv->addItem(ch);
          loadEvents(ch);
          if(svc.sid==cur.sid&&svc.tsid==cur.tsid&&cur.netid==svc.netid)
             lv->setIndex(i);
     }
     NGLOG_DEBUG("%d services loaded CUR:%d.%d.%d index=%d",lv->getItemCount(),cur.netid,cur.tsid,cur.sid,lv->getIndex());
     lv->sort([](const ListView::ListItem&a,const ListView::ListItem&b)->int{
                            return a.getId()-b.getId()<0;
                       },false);
     delete []svcs;
}

void TVWindow::onEITS(const SERVICELOCATOR*svc){
    for(int i=0;i<lv->getItemCount();i++){
        TVChannel*ch=(TVChannel*)lv->getItem(i);
        if(ch->svc==*svc){
             ch->update++;
             break;
        }
    }
}

int TVWindow::loadEvents(TVChannel*ch){
    char sname[256],des[256],text[512],tms[64];
    ch->clearEvents();
    std::vector<DVBEvent>evts;
    DtvGetEvents(&ch->svc,evts);
    time_t tmstart=(time_t)lv->getStartTime();
    for(auto e:evts){
       TVEvent te;
       e.getShortName(sname,des);
       e.getExtend(text);
       ctime_r((time_t*)&e.start_time,tms);
       *strchr(tms,'\n')=0;

       te.id=e.event_id;
       te.name=sname;
       te.desc=des;
       te.start=e.start_time;
       te.duration=e.duration;
       te.ext=text;
       if((time_t)(e.start_time+e.duration)>=tmstart)
          ch->addEvent(te);
    }
    return evts.size();
}

bool TVWindow::onMessage(DWORD msg,DWORD wp,ULONG lp){
    if(msg!=MSG_EPGS_UPDATE)
        return NTVWindow::onMessage(msg,wp,lp);
    for(int i=0;i<lv->getItemCount();i++){
        TVChannel*ch=(TVChannel*)lv->getItem(i);
        if(ch->update)
            loadEvents(ch);
    }
    if(tdt_time==0){
       tdt_time=DtvGetTdTime();
       lv->setStartTime(tdt_time); 
    }
    sendMessage(MSG_EPGS_UPDATE,0,0,2000);
    return true;
}

bool TVWindow::onKeyUp(KeyEvent&k){
    LONG tt=lv->getStartTime();
    struct tm tstart;
    gmtime_r(&tt,&tstart);
    switch(k.getKeyCode()){
    case KEY_LEFT: lv->onKeyUp(k);rule->setMin(tstart.tm_hour);break;//if(tt-3600>=tdt_time)tt-=3600;UpdateTime(tt);return true;
    case KEY_RIGHT:lv->onKeyUp(k);rule->setMin(tstart.tm_hour);break;//if(tt+3600<tdt_time+3600*24*8)tt+=3600;UpdateTime(tt);return true;
    default:return NTVWindow::onKeyUp(k);
    }
}

Window*CreateTVGuide(){
    TVWindow*w=new TVWindow(0,0,1280,720);
    w->initContent(NWS_TITLE|NWS_TOOLTIPS);
    w->setText("TV Guide");
    NGLRect rcv={30,70,320,240};
    nglAvSetVideoWindow(0,NULL,&rcv);

    w->addTipInfo("help_icon_4arrow.png","Navigation",160);
    w->addTipInfo("help_icon_ok.png","Select",160);
    w->addTipInfo("help_icon_back.png","Back",160);
    w->addTipInfo("help_icon_exit.png","Exit",160);
    w->addTipInfo("help_icon_blue.png","",160);
    w->addTipInfo("help_icon_red.png","",160);
    w->addTipInfo("help_icon_yellow.png","",160);
    
    w->loadServices(FAV_GROUP_AV);
    w->show();
    return w; 
}

}//namespace
