#include<appmenus.h>
#include<ntvnavwindow.h>
#include<clockview.h>
#include<ngl_log.h>

#ifdef HAVE_IRDETOCCA
#include<ccairdevent.h>
#endif
NGL_MODULE(CHANNELBAR)

namespace ntvplus{

#define MSG_DATE_TIME 1010
class ChannelBar:public NTVNavWindow{
protected:
   bool present;
   Selector *chlst;
   ProgressBar*media_progress;
   TextView*event_name;
   TextView*event_des;
   TextView*datetime ;
public:
   ChannelBar(int x,int y,int w,int h);
   virtual bool onKeyUp(KeyEvent&k)override;
   void onService(const DVBService*svc)override{
       SERVICELOCATOR cur;
       TVChannel*ch=DVBService2Channel(svc);
       chlst->addItem(ch);
       DtvGetCurrentService(&cur);
       if(cur==*svc)chlst->setIndex(chlst->getItemCount()-1);
   }
   void onEventPF(const SERVICELOCATOR&svc,const DVBEvent*e,bool present)override{
       SERVICELOCATOR loc;
       char name[256],desc[256];
       ChannelItem*itm=(ChannelItem*)chlst->getItem(chlst->getIndex());
       DtvGetCurrentService(&loc);
       if(itm) loc=itm->svc;
       if( (loc==svc) && (present==this->present)){
           time_t now=time(NULL);
           e->getShortName(name,desc);
           event_name->setText(name);
           event_des->setText(desc);
           media_progress->setProgress((now-e->start_time)*100/e->duration);
       }
   }
};

ChannelBar::ChannelBar(int x,int y,int w,int h):NTVNavWindow(x,y,w,h){
   initContent(NWS_SIGNAL);
   chlst=new NTVSelector(std::string(),495,46);
   chlst->showArrows(Selector::SHOW_NEVER);
   chlst->setFontSize(40);
   chlst->clearFlag(View::FOCUSABLE);
   chlst->setBgColor(0).setFgColor(0xFFFFFFFF);
   addChildView(chlst);
   chlst->setItemSelectListener([this](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        ChannelItem&itm=(ChannelItem&)lvitem;
        SERVICELOCATOR&s=itm.svc;
        NGLOG_DEBUG("play %d.%d.%d",s.netid,s.tsid,s.sid);
        DtvPlay(&itm.svc,nullptr);
        loadEventPF(&s);
    });

   event_name=new TextView(std::string(),495,46);
   event_name->setFontSize(28);
   event_name->setPos(500,0);
   addChildView(event_name)->setBgColor(0).setFgColor(0xFFFFFFFF);

   event_des=new TextView(std::string(),880,180);
   event_des->setPos(10,61);
   addChildView(event_des)->setBgColor(0).setFgColor(0xFFFFFFFF);

   datetime=new TextView(std::string(),280,46);
   datetime->setFontSize(28);
   datetime->setPos(1000,0);
   addChildView(datetime)->setBgColor(0).setFgColor(0xFFFFFFFF);

   sig_strength->setSize(400,10);
   sig_strength->setPos(20,50);
   sig_quality->setSize(400,10);
   sig_quality->setPos(44,50);
   addChildView(new ClockView(200,200))->setPos(990,50);   
   media_progress=new NTVProgressBar(w,3);
   media_progress->setPos(0,46);
   media_progress->setProgress(37);
   addChildView(media_progress);
   loadServices(FAV_GROUP_ALL);
   sendMessage(MSG_DATE_TIME,0,0,1000);
   present=true;   
}

bool ChannelBar::onKeyUp(KeyEvent&k){
   switch(k.getKeyCode()){
   case KEY_UP:  
           k.setKeyCode(KEY_LEFT) ;
           return chlst->onKeyUp(k);
   case KEY_DOWN:
           k.setKeyCode(KEY_RIGHT);
           return chlst->onKeyUp(k);
   case KEY_LEFT:
   case KEY_RIGHT:
         present=!present;
         loadEventPF(nullptr);
         return true;
   default:return NTVWindow::onKeyUp(k);
   }
}

Window*CreateChannelPF(){
   Window*w=new ChannelBar(0,470,1280,248);
   w->show();
   return w;
}
}//namespace
