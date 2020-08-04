#include<appmenus.h>
#include <dvbapp.h>
#include <dvbepg.h>
#include <satellite.h>
#include <diseqc.h>
#include <ngl_log.h>
NGL_MODULE(SEARCHING)

namespace ntvplus{

static INT SVC_CBK(const SERVICELOCATOR*loc,const DVBService*svc,const BYTE*pmt,void*userdata);
static void NEW_TP(const TRANSPONDER*,int idx,int tp_count,void*userdata);
static void FINISHEDTP(const TRANSPONDER*,int idx,int tp_count,void*userdata);
class SearchResultWindow:public NTVWindow{
protected:
    ListView*left;
    ListView*right;
    
    ProgressBar*progress;
    TextView*percent;
    TextView*txt_sat;
    TextView*txt_tp;
    SEARCHNOTIFY notify;
public:
    std::vector<TRANSPONDER>transponders;
public:
    SearchResultWindow(int x,int y,int w,int h);
    void Search(std::vector<TRANSPONDER>tps,INT  schmode){
         transponders=tps;
         notify.SERVICE_CBK=SVC_CBK;
         notify.FINISHTP=FINISHEDTP;
         notify.NEWTP=NEW_TP;
         notify.userdata=this;
         NGLOG_DEBUG("search %d tp searmode=%d(0 for NIT search)",transponders.size(),schmode);
         DtvSearch(transponders.data(),schmode,&notify);
    }
    void onSearchingTP(const TRANSPONDER*tp){
         std::string txt;
         GetTPString(tp,txt);
         txt_tp->setText("Transponder:"+txt);
    }
    void onReceivedService(const SERVICELOCATOR*loc,const DVBService*svc){
         char servicename[256],providername[256];
         svc->getServiceName(servicename,providername);
         if(ISVIDEO(svc->serviceType))
             left->addItem(new ChannelItem(servicename,loc));
         else if (ISAUDIO(svc->serviceType))
             right->addItem(new ChannelItem(servicename,loc));
         
         NGLOG_DEBUG("rcv service %d.%d.%d %d[%s]",loc->netid,loc->tsid,loc->sid,svc->serviceType,servicename);
    }
    void onFinishedTP(const TRANSPONDER*tp,int idx,int tp_count){
         progress->setProgress(idx*100/tp_count);
         percent->setText(std::to_string(idx*100/tp_count)+"%");
         if(nullptr==tp){
              ChannelItem*itm=(ChannelItem*)left->getItem(0);
              if(itm)DtvPlay(&itm->svc,nullptr);
              DtvSaveProgramsData("dvb_programs.dat");
              FavInit(App::getInstance().getArg("favorite","favorites.json").c_str());
              DtvInitLCN((LCNMODE)(LCN_FROM_BAT|LCN_FROM_USER),1000);
         }
    }
};

SearchResultWindow::SearchResultWindow(int x,int y,int w,int h)
:NTVWindow(x,y,w,h){
     initContent(NWS_TITLE|NWS_SIGNAL|NWS_TOOLTIPS); 
     left =new ListView(560,400);
     right=new ListView(560,400);
     left->setFlag(View::BORDER);
     right->setFlag(View::BORDER);
     left->setPos(60,70);
     right->setPos(650,70);
     left->setItemPainter(ChannelPainter);
     right->setItemPainter(ChannelPainter);
 
     addChildView(left);
     addChildView(right);
     left->setBgColor(getBgColor());
     left->setFgColor(getFgColor());
     right->setBgColor(getBgColor());
     right->setFgColor(getFgColor());
     
     txt_sat=new TextView("SatelliteName",200,25);
     addChildView(txt_sat)->setPos(50,480);

     txt_tp=new TextView("Transponderxxx",300,25);
     addChildView(txt_tp)->setPos(50,506);

     addChildView(new TextView("Strength",120,25))->setPos(50,535);
     sig_strength->setSize(300,10);
     sig_strength->setPos(50,560);

     addChildView(new TextView("Quality",120,25))->setPos(50,570);
     sig_quality->setSize(300,10);
     sig_quality->setPos(50,600);    

     percent=new TextView("0%",200,120);
     percent->setPos(1000,520);
     addChildView(percent)->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF).setFontSize(80);

     progress=new ProgressBar(1000,10);
     progress->setPos(50,620);progress->setProgress(35);
     addChildView(progress);

     memset(&notify,0,sizeof(SEARCHNOTIFY));
}

Window*CreateSearchResultWindow(std::vector<TRANSPONDER>tps,INT schmode){
     SearchResultWindow*w=new SearchResultWindow(0,0,1280,720);
     w->setText("Search"); 
     w->Search(tps,schmode);
     w->addTipInfo("help_icon_4arrow.png","Navigation",160);
     w->addTipInfo("help_icon_ok.png","Select",160);
     w->addTipInfo("help_icon_back.png","Back",160);
     w->addTipInfo("help_icon_exit.png","Exit",260);
     w->addTipInfo("help_icon_blue.png","",160);
     w->addTipInfo("help_icon_red.png","",160);
     w->addTipInfo("help_icon_yellow.png","",160);

     w->show();
     return w;
}

static INT SVC_CBK(const SERVICELOCATOR*loc,const DVBService*svc,const BYTE*pmt,void*userdata){
    SearchResultWindow*w=(SearchResultWindow*)userdata;
    w->onReceivedService(loc,svc);
    return 1;
}
static void NEW_TP(const TRANSPONDER*tp,int,int,void*userdata){
    SearchResultWindow*w=(SearchResultWindow*)userdata;
    w->onSearchingTP(tp);
    NGLOG_DEBUG("Searching TP freq:%d",tp->u.s.frequency);
}

static void FINISHEDTP(const TRANSPONDER*tp,int idx,int cnt,void*userdata){
    SearchResultWindow*w=(SearchResultWindow*)userdata;
    w->onFinishedTP(tp,idx,cnt);
    if(tp)
        NGLOG_DEBUG("TP freq:%d Search Finished",tp->u.s.frequency);
    else
        NGLOG_DEBUG("Search Finished");
}
}//namespace
