#include <windows.h>
#include <stdlib.h>
#include <ngl_log.h>
#include <json/json.h>
#include <appmenus.h>
#include <satellite.h>
NGL_MODULE(SATTP)
namespace ntvplus{

class TPManagerWindow:public NTVWindow{
protected:
    int mode;
    ListView*satlst;
    ListView*tplst;
public:
    TPManagerWindow(int x,int y,int w,int h);
    ~TPManagerWindow(){
       SaveSatellite2DB("sattest.json");
    }
    SATELLITE*getSatellite(){
        SatelliteItem*itm=(SatelliteItem*)satlst->getItem(satlst->getIndex());
        return itm?&itm->satellite:nullptr;
    }
    TRANSPONDER*getTransponder(){
        TransponderItem*itm=(TransponderItem*)tplst->getItem(tplst->getIndex());
        return itm?&itm->tp:nullptr;
    }
    int loadData(const std::string&fname);
    void SetEditorMode(int md);
    virtual bool onKeyUp(KeyEvent&k)override;
};

TPManagerWindow::TPManagerWindow(int x,int y,int w,int h)
    :NTVWindow(x,y,w,h){
    initContent(NWS_TITLE|NWS_SIGNAL|NWS_TOOLTIPS);
    satlst=new ListView(500,460);
    satlst->setPos(100,80);
    satlst->setItemPainter(SettingPainter);

    tplst=new ListView(500,460);
    tplst->setPos(680,80); 
    tplst->setItemPainter(SettingPainter);
    satlst->setBgColor(0xFF000000);
    satlst->setFgColor(0xFFFFFFFF);
    satlst->setFlag(BORDER);
    tplst->setBgColor(0xFF000000);
    tplst->setFgColor(0xFFFFFFFF);
    tplst->setFlag(BORDER);
    addChildView(satlst);
    addChildView(tplst);

    tplst->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        TransponderItem&itm=(TransponderItem&)lvitem;
        NGLOG_DEBUG("switch to tp %d ",index);
        ConfigureTransponder(&itm.tp);
        nglTunerLock(0,&itm.tp);
    });

    satlst->setItemSelectListener([&](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        TRANSPONDER tps[32];
        NGLOG_DEBUG("prepare to switch sateliite");
        int cnt=GetSatelliteTp(index,tps,32);
        NGLOG_DEBUG("switch to satellite %d has %d tp",index,cnt);
        tplst->clearAllItems();
        for(int i=0;i<cnt;i++)tplst->addItem(new TransponderItem(tps[i]));
        tplst->setIndex(0);
        NGLOG_DEBUG("");
    });

    TextView*lb=new TextView("Signal Intensity",150,30);
    lb->setPos(80,555);
    addChildView(lb)->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    sig_strength->setSize(400,8);
    sig_strength->setPos(250,565);

    lb=new TextView("Signal Quality",150,30);
    lb->setPos(650,555);
    addChildView(lb)->setBgColor(0xFF000000).setFgColor(0xFFFFFFFF);
    sig_quality->setSize(400,8);
    sig_quality->setPos(830,565);

    SetEditorMode(1);
}

void TPManagerWindow::SetEditorMode(int md){
    clearTips();
    if(md){
        addTipInfo("help_icon_red.png","Add SAT");
        addTipInfo("help_icon_green.png","Add TP");
        addTipInfo("help_icon_yellow.png","Modify SAT");
        addTipInfo("help_icon_blue.png","Delete SAT");
    }else{
        addTipInfo("help_icon_red.png","Add TP");
        addTipInfo("help_icon_yellow.png","Modify TP");
        addTipInfo("help_icon_blue.png","Delete TP");
    }
}

bool TPManagerWindow::onKeyUp(KeyEvent&k){
    bool satmd=satlst->hasFlag(FOCUSED);
    switch(k.getKeyCode()){
    case KEY_LEFT:
    case KEY_RIGHT:
          NTVWindow::onKeyUp(k);
          SetEditorMode(satlst->hasFlag(FOCUSED));
          return true;
    case KEY_RED://new sat | tp   
           if(satmd)CreateSatEditorWindow(nullptr);
           else CreateTPEditorWindow(nullptr);//getSatellite());
           return satmd;
    case KEY_GREEN://new tp 
           if(satmd)
               CreateTPEditorWindow(nullptr);//getTransponder());
           return true;
    case KEY_YELLOW://modify sat | tp
           if(satmd)CreateSatEditorWindow(getSatellite());
           else CreateTPEditorWindow(getTransponder());
           return true;
    case KEY_BLUE://delete sat | tp
           if(satmd){
               RemoveSatellite(satlst->getIndex());
               satlst->removeItem(satlst->getIndex());
           }else{
               TRANSPONDER*tp=getTransponder();
               RemoveTransponder(tp?tp->u.s.tpid:-1);
               tplst->removeItem(tplst->getIndex());
           }
           break;
    default:return NTVWindow::onKeyUp(k);
    }
}

int TPManagerWindow::loadData(const std::string&fname){
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

Window*CreateTPManagerWindow(){
    TPManagerWindow*w=new TPManagerWindow(0,0,1280,720);
    w->setText("Satllite& TP Manager");
    w->loadData("satellites.json");
    w->show();
    return w;
}

}//endof namespace
