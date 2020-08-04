#include<ntvwindow.h>
#include<appmenus.h>
#include<ngl_log.h>
#include<satellite.h>
NGL_MODULE(CHANNELSEARCH)

namespace ntvplus{

TransponderItem::TransponderItem(const TRANSPONDER&t):AbsListView::ListItem(std::string(),tp.u.c.frequency){
    const char*polars[]={"H","V","L","R"};
    char buf[64];
    int pol=t.u.s.polar;
    tp=t;
    NGLOG_DEBUG("tp:%d freq:%d symb:%d polar:%d",t.u.s.tpid,t.u.s.frequency,t.u.s.symbol_rate,t.u.s.polar);
    sprintf(buf,"%d/%s/%d",t.u.s.frequency/1000,((pol>=0&&pol<4)?polars[pol]:"X"),t.u.s.symbol_rate);
    setText(buf);
}


#define IDC_SATS 100
#define IDC_TPS 101
#define WM_GETSIGNAL 1000
class SearchWindow:public NTVWindow{
protected:
    Selector*satlst;
    Selector*tplst;
    Selector*ch_available;
    Selector*ch_type;
    Selector*sch_mode;
    Selector*search;
public:
    SearchWindow(int x,int y,int w,int h,bool advance=false);
    int loadData(const std::string&fname);
    int getSearchTransponders(std::vector<TRANSPONDER>&tps);
    virtual bool onKeyUp(KeyEvent&k)override;
};

static Window*OnPopupWindow(int w,int h){
    Window*win=new Window(0,0,w,h);
    ListView*lv=new ListView(w,h);
    lv->setBgColor(0xFF222222);
    lv->setFgColor(0xFFFFFFFF);
    lv->setItemPainter(SettingPainter);
    win->addChildView(lv);
    return win;
};

SearchWindow::SearchWindow(int x,int y,int w,int h,bool advance):NTVWindow(x,y,w,h){
    const char*cha[]={"All","Commerical","Free of charge",nullptr};
    const char*cht[]={"All","TV","Radio",nullptr};
    const char*chm[]={"Single","Network",nullptr};
    int i,yy=150;
    initContent(NWS_TITLE|NWS_SIGNAL|NWS_TOOLTIPS);

    ch_available=ch_type=sch_mode=search=nullptr;

    satlst=new NTVSelector("Satellite",1000,40);
    satlst->setPos(140,yy);  yy+=50;   
    satlst->setLabelWidth(600);
    satlst->setItemPainter(SettingPainter);
    satlst->setPopupListener(OnPopupWindow);

    tplst=new NTVSelector("Transponder",1000,40);
    tplst->setLabelWidth(600);
    tplst->setPos(140,yy);   yy+=50;
    tplst->setItemPainter(SettingPainter);
    tplst->setPopupListener(OnPopupWindow);
    addChildView(satlst);
    addChildView(tplst);
    tplst->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        TransponderItem&itm=(TransponderItem&)lvitem;
        NGLOG_DEBUG("switch to tp %d ",index);
        TRANSPONDER&tp=itm.tp;
        ConfigureTransponder(&tp);
        nglTunerLock(0,&tp);
    });
    satlst->setItemSelectListener([&](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        TRANSPONDER tps[32];
        NGLOG_DEBUG("prepare to switch sateliite");
        int cnt=GetSatelliteTp(index,tps,32);
        NGLOG_DEBUG("switch to satellite %d has %d tp",index,cnt);
        tplst->clearAllItems();
        for(int i=0;i<cnt;i++)tplst->addItem(new TransponderItem(tps[i]));
        tplst->setIndex(0);
    });
    
    /*******************************************/
    if(advance){
        ch_available=new NTVSelector("Channel availability",1000,40);  
        ch_available->setPos(140,yy); yy+=50;
        ch_available->setItemPainter(SettingPainter);
        ch_available->setPopupListener(OnPopupWindow);

        ch_type=new NTVSelector("Channel type",1000,40);               
        ch_type->setPos(140,yy); yy+=50;
        ch_type->setItemPainter(SettingPainter);
        ch_type->setPopupListener(OnPopupWindow);

        sch_mode=new NTVSelector("Search mode",1000,40);
        sch_mode->addItem(new Selector::ListItem("Single"));
        sch_mode->addItem(new Selector::ListItem("Network"));
        sch_mode->setPos(140,yy);yy+=50;
        sch_mode->setItemPainter(SettingPainter); 
        sch_mode->setPopupListener(OnPopupWindow);
    
        for(i=0;cha[i];i++)ch_available->addItem(new Selector::ListItem(cha[i]));
        for(i=0;cht[i];i++)ch_type->addItem(new Selector::ListItem(cht[i]));
        for(i=0;chm[i];i++)sch_mode->addItem(new Selector::ListItem(chm[i]));
        ch_available->setIndex(0);
        ch_type->setIndex(0);
        sch_mode->setIndex(0);
    
        ch_available->setLabelWidth(600);
        ch_type->setLabelWidth(600);
        sch_mode->setLabelWidth(600);
        addChildView(ch_available);
        addChildView(ch_type);
        addChildView(sch_mode);
    }
    /*******************************************/

    search=new NTVSelector("Search",1000,40);
    search->setItemPainter(SettingPainter);
    search->setPopupListener(OnPopupWindow);
    search->setPos(140,yy); yy+=50;
    search->addItem(new Selector::ListItem("No"));
    search->addItem(new Selector::ListItem("Yes"));
    search->setIndex(1);search->setLabelWidth(600);
    addChildView(search);
    search->setFlag(View::FOCUSED);

    TextView*lb=new TextView("Signal Intensity",150,30);
    lb->setPos(50,552);
    addChildView(lb)->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    sig_strength->setSize(420,8);//=new NTVProgressBar(420,15);
    sig_strength->setPos(220,565);sig_strength->setProgress(35);

    lb=new TextView("Signal Quality",150,30);
    lb->setPos(670,552);
    addChildView(lb)->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    sig_quality->setSize(420,8);//=new NTVProgressBar(420,15);
    sig_quality->setPos(820,565);
    sig_quality->setProgress(43);

    addTipInfo("help_icon_green.png","satellite");
    addTipInfo("help_icon_blue.png","transponder");
   
}

int SearchWindow::loadData(const std::string&fname){
    if(GetSatelliteCount()==0)
       LoadSatelliteFromDB(fname.c_str());
    int count=GetSatelliteCount();
    for(int i=0;i<count;i++){
        SATELLITE sat;
        GetSatelliteInfo(i,&sat);
        NGLOG_DEBUG("sat[%d]:%s",i,sat.name);
        satlst->addItem(new SatelliteItem(sat,i));
    }
    if(count)satlst->setIndex(0);
    return count;
}

int SearchWindow::getSearchTransponders(std::vector<TRANSPONDER>&tps){
    int idx;
    NGLOG_DEBUG("schmode=%p",sch_mode);
    if(sch_mode&&sch_mode->getIndex()==0){//index=0 single tp ,index=1 by nit
        idx=tplst->getIndex();
        TransponderItem*itm=(TransponderItem*)tplst->getItem(idx);
        if(itm)tps.push_back(itm->tp);
    }else{//all tp of selected satellite
        for(int i=0;i<tplst->getItemCount();i++){
              TransponderItem*itm=(TransponderItem*)tplst->getItem(i);
              tps.push_back(itm->tp);
        }
    }
    return tps.size();
}

bool SearchWindow::onKeyUp(KeyEvent&k){
     NGLOG_DEBUG("search=%p schmod=%d",search,(sch_mode?sch_mode->getIndex():-1));
     if( search && search->hasFlag(View::FOCUSED) && (search->getIndex()==1)
               && (k.getKeyCode()==KEY_ENTER) ){
         std::vector<TRANSPONDER>tps;
         getSearchTransponders(tps);
         int mode=tps.size();//(sch_mode&&sch_mode->getIndex()==1)?0:tps.size();
         CreateSearchResultWindow(tps,mode);
         return true;
     }
     if(k.getKeyCode()==KEY_GREEN||k.getKeyCode()==KEY_BLUE){
         CreateTPManagerWindow();
         return true;
     }
     return Window::onKeyUp(k);
}

Window*CreateChannelSearch(bool advance){
    SearchWindow*w=new SearchWindow(0,0,1280,720,advance);
    w->setText("Channel Search");
    w->loadData("satellites.json");
    w->show();
    return w; 
}

}//namespace
