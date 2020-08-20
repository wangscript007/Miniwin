#include <windows.h>
#include <appmenus.h>
#include <ngl_log.h>
#include <dvbepg.h>
#include <ngl_video.h>
#include <channelepgview.h>
#include <ntvnavwindow.h>
#include <ruleview.h>

NGL_MODULE(TVGUIDE);

namespace ntvplus{

#define MSG_EPGS_UPDATE 1010
#define CHANNEL_HEIGHT 46
class TVChannelExt:public nglui::ChannelBar{
public:
   SERVICELOCATOR svc;
   int update;
   BOOL isHD;
public:
   TVChannelExt(const std::string&txt,const SERVICELOCATOR*svc_):ChannelBar(txt){
       svc=*svc_;
       update=1;
   }
};

class TVWindow:public NTVNavWindow{
protected:
   TextView*prg_title;
   TextView*prg_time;
   TextView*prg_dsc;
   ChannelEpgView *lv;
   RuleView*rule;
   ULONG tdt_time;
public:
   TVWindow(int x,int y,int w,int h):NTVNavWindow(x,y,w,h){
       lv=new ChannelEpgView(1240,CHANNEL_HEIGHT*7);
       lv->setPos(20,336).setBgColor(0).setFlag(BORDER);
       lv->setChannelNameWidth(320);
       lv->setItemSize(0,CHANNEL_HEIGHT);
       addChildView(lv)->setBgColor(0);
       lv->setItemSelectListener([this](AbsListView&lv,const ListView::ListItem&lvitem,int index){
           TVChannelExt&itm=(TVChannelExt&)lvitem;
           DtvPlay(&itm.svc,nullptr);
           NGLRect rcv={30,70,320,240};
           nglAvSetVideoWindow(0,NULL,&rcv);
           loadEvents(itm.svc);
       });
       tdt_time=0;
       rule=new RuleView(1240,24);
       lv->setPixelPerMinute(2);
       rule->setOffset(lv->getChannelNameWidth()).setInterval(lv->getPixelPerMinute()*6).setFontSize(12);
       rule->setLabeListener([](RuleView&v,int idx,std::string&label){
           if(idx%10==0){
               char tbuf[16];
               sprintf(tbuf,"%02d:00",(v.getMin()+idx/10)%24);
               label=tbuf;
           }else label="";
       });
       addChildView(rule)->setPos(20,310).setBgColor(0x60406080);

       prg_title=new TextView("ProgramTitle",920,38);
       addChildView(prg_title)->setPos(360,68).setBgColor(0x80000000).setFontSize(35).setFgColor(0xFFFFFFFF);

       prg_time=new TextView("Program time&duration",870,30);
       addChildView(prg_time)->setPos(360,108).setBgColor(0).setFgColor(0xFFFFFFFF);

       prg_dsc=new TextView("Program description",870,150);
       addChildView(prg_dsc)->setPos(360,130).setFgColor(0xFFFFFFFF).setBgColor(0);

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
   int loadServices(UINT favid)override{
       lv->clearAllItems();
       int rc=NTVNavWindow::loadServices(favid);
       lv->sort([](const ListView::ListItem&a,const ListView::ListItem&b)->int{
                            return a.getId()-b.getId()<0;
                       },false);
        return rc;
   }
   void onService(const DVBService*info)override{
        char name[128];
        INT lcn;
        SERVICELOCATOR cur,loc=*info;
        DtvGetCurrentService(&cur);
        info->getServiceName(name);
        DtvGetServiceItem(&loc,SKI_LCN,&lcn);
        TVChannelExt*ch=new TVChannelExt(name,&loc);
        ch->setId(lcn);
        ch->isHD=ISHDVIDEO(info->serviceType);
        lv->addItem(ch);
        if(loc==cur)lv->setIndex(lv->getItemCount()-1);
   }
   void DVBEvent2TVEvent(const DVBEvent&e,TVEvent&te){
       char sname[256],des[256],text[512];
       e.getShortName(sname,des);     
       e.getExtend(text);

       te.id=e.event_id;  te.name=sname;    
       te.desc=des;       te.ext=text;
       te.start=e.start_time;
       te.duration=e.duration;

   }
   void onEvents(const SERVICELOCATOR&svc,const std::vector<DVBEvent>&evts)override{
       for(int i=0;i<lv->getItemCount();i++){
           TVChannelExt*ch=(TVChannelExt*)lv->getItem(i);
           if(ch->svc==svc){
              ch->update++;
              ch->clearEvents();
              for(e:evts){
                  TVEvent te;
                  DVBEvent2TVEvent(e,te);
                  ch->addEvent(te);
              }
              break;
           }
       }
       
   }
   virtual bool onKeyUp(KeyEvent&k)override;
   virtual bool onMessage(DWORD msg,DWORD wp,ULONG lp)override;
};

bool TVWindow::onMessage(DWORD msg,DWORD wp,ULONG lp){
    if(msg!=MSG_EPGS_UPDATE)
        return NTVWindow::onMessage(msg,wp,lp);
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
    case KEY_LEFT: lv->onKeyUp(k);rule->setMin(tstart.tm_hour);break;
    case KEY_RIGHT:lv->onKeyUp(k);rule->setMin(tstart.tm_hour);break;
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
