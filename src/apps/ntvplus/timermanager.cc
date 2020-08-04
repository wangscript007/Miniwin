#include <windows.h>
#include <appmenus.h>
#include <dvbepg.h>
#include <ngl_types.h>
#include <ngl_log.h>

#define IDC_CHANNELS 100

NGL_MODULE(CHANNELLIST)

namespace ntvplus{

class TimerItem:public ListView::ListItem{
public:
    std::string event;
    std::string channel;
    std::string action;
    std::string datetime;
public:
    TimerItem(const std::string&txt):ListView::ListItem(txt){
    }
};

static void TimerPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    TimerItem& tt=(TimerItem&)itm;
    canvas.set_color(state?0xFFFF0000:lv.getBgColor());
    canvas.rectangle(itm.rect);
    canvas.fill();
     
    RECT r=itm.rect;
    canvas.set_color(lv.getFgColor());
    canvas.draw_text(r,itm.getText(),DT_LEFT|DT_VCENTER);//id
    r.offset(40,0);//id width;
    canvas.draw_text(r,tt.event,DT_LEFT|DT_VCENTER);//event;
    
    r.offset(400,0);//channel width
    canvas.draw_text(r,tt.channel,DT_LEFT|DT_VCENTER);//channel
    r.offset(300,0);//chanel width
    canvas.draw_text(r,tt.datetime,DT_LEFT|DT_VCENTER);//datetime
}

Window*CreateTimerManager(){
    NTVWindow*w=new NTVWindow(0,0,1280,720);
    ListView*lv=new ListView(1200,580);
    lv->setPos(40,70);
    lv->setItemSize(-1,CHANNEL_LIST_ITEM_HEIGHT);
    w->setText("Timer Manager");
    lv->setBgColor(w->getBgColor());
    lv->setFgColor(w->getFgColor());
    w->addChildView(lv)->setId(IDC_CHANNELS);
    lv->setItemPainter(TimerPainter);

    lv->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        TimerItem*itm=(TimerItem*)&lvitem;
        //if(itm)DtvPlay(&itm->svc,nullptr);
    });
    
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

}
