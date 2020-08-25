#include <windows.h>
#include <ntvnavwindow.h>
#include <appmenus.h>
#include <ngl_types.h>
#include <dvbepg.h>
#include <ngl_log.h>
#include <ngl_panel.h>
#define IDC_CHANNELS 100
#define IDC_NAMEP 101
#define IDC_DESCP  102
#define IDC_NAMEF 103
#define IDC_DESCF  104
#define IDC_TPINFO 105
NGL_MODULE(CHANNELLIST)

namespace ntvplus{

class ChannelsWindow:public NTVNavWindow{
protected:
   ListView*chlst;
   TextView*namep,*descp;
   TextView*namef,*descf;
   ToolBar*tbfavs;
public:
   ChannelsWindow(int x,int y,int w,int h);
   virtual bool onKeyUp(KeyEvent&k)override;
   virtual void onGroup(UINT favid,const std::string&name)override{
       tbfavs->addItem(new ToolBar::Button(name,favid));
   }
   virtual void onService(const DVBService*svc)override{
        TVChannel*ch=DVBService2Channel(svc);
        chlst->addItem(ch);
   }
   virtual int loadServices(UINT favid)override{
       chlst->clear(); 
       NTVNavWindow::loadServices(favid);
       chlst->sort([](const ListView::ListItem&a,const ListView::ListItem&b)->int{
                            return a.getId()-b.getId()<0;
                     },false);
   }
   void onEventPF(const SERVICELOCATOR&svc,const DVBEvent*e,bool precent)override{
        char name[256]={0},des[256]={0};
        int idx=chlst->getIndex();
        TVChannel*itm=(TVChannel*)chlst->getItem(idx);
        if(itm->svc==svc){
            if(precent){e->getShortName(name,des);namep->setText(name);descp->setText(des);}
            else {e->getShortName(name,des);namef->setText(name);descf->setText(des);}
        }
   }
};
ChannelsWindow::ChannelsWindow(int x,int y,int w,int h):NTVNavWindow(x,y,w,h){
    initContent(NWS_TITLE|NWS_TOOLTIPS);
    setText("ChannelList");
    tbfavs=CreateNTVToolBar(1280,38);

    tbfavs->setPos(0,70);
    loadGroups();
    addChildView(tbfavs);

    chlst=new ListView(430,CHANNEL_LIST_ITEM_HEIGHT*13);
    chlst->setPos(20,110);
    chlst->setItemSize(-1,CHANNEL_LIST_ITEM_HEIGHT);
    chlst->setBgColor(0).setFgColor(getFgColor());
    chlst->setFlag(View::SCROLLBARS_VERTICAL).setFlag(View::BORDER);
    chlst->setItemPainter(ChannelPainterLCN);
    addChildView(chlst)->setId(IDC_CHANNELS);
    addChildView(new TextView("tpinfo",200,28))->setId(IDC_TPINFO).setPos(1000,630).setFontSize(20).setBgColor(0);

    chlst->setItemSelectListener([this](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        TVChannel&ch=(TVChannel&)lvitem;
        int lcn;char buf[8];
        DtvGetServiceItem(&ch.svc,SKI_LCN,&lcn);
        sprintf(buf,"%d",lcn);
        nglFPShowText(buf,4);
        lv.getParent()->sendMessage(1000,0,0,500);
        TextView*tpinfo=(TextView*)lv.getParent()->findViewById(IDC_TPINFO);
        TRANSPONDER tp; 
        std::string tpstr;
        int rc=DtvGetTPByService(&ch.svc,&tp);
        GetTPString(&tp,tpstr); 
        NGLOG_VERBOSE("DtvGetTPByService=%d tpinfo=%p [%s]",rc,tpinfo,tpstr.c_str());
        tpinfo->setText(tpstr);
        loadEventPF(&ch.svc); 
    });
    chlst->setItemClickListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        TVChannel&ch=(TVChannel&)lvitem;
        DtvPlay(&ch.svc,nullptr);
    });

    if(tbfavs->getItemCount())loadServices(tbfavs->getItem(0)->getId());

    namep=new TextView("NOW",800,36);
    addChildView(namep)->setId(IDC_NAMEP).setBgColor(0).setFgColor(0xFFFFFFFF).setPos(455,110);

    descp=new TextView(std::string(),800,220);
    descp->setAlignment(DT_LEFT|DT_TOP|DT_MULTILINE);
    addChildView(descp)->setPos(455,148).setId(IDC_DESCP).setBgColor(0);

    namef=new TextView("NEXT",800,36);
    addChildView(namef)->setId(IDC_NAMEF).setBgColor(0).setFgColor(0xFFFFFFFF).setPos(455,370);

    descf=new TextView(std::string(),800,220);
    descf->setAlignment(DT_LEFT|DT_TOP|DT_MULTILINE);
    addChildView(descf)->setPos(455,408).setId(IDC_DESCF).setBgColor(0);

    addTipInfo("help_icon_4arrow.png","Navigation",160);
    addTipInfo("help_icon_ok.png","Select",160);
    addTipInfo("help_icon_exit.png","Exit",260);
    addTipInfo("help_icon_red.png","Sort",160);
    addTipInfo("help_icon_yellow.png","EditChannel",160);
}

bool ChannelsWindow::onKeyUp(KeyEvent&k){
   int idx;
   switch(k.getKeyCode()){
   case KEY_LEFT :
   case KEY_RIGHT:
          tbfavs->onKeyUp(k);
          idx=tbfavs->getIndex();
          loadServices(tbfavs->getItem(idx)->getId());
          return true;
   case KEY_ENTER:
        {
            int idx=chlst->getIndex();
            TVChannel*itm=(TVChannel*)chlst->getItem(idx);
            NGLOG_VERBOSE("itm=%p",itm);
            if(itm)DtvPlay(&itm->svc,nullptr); 
        }break;
   case KEY_YELLOW:CreateChannelEditWindow();return true;
   default:return NTVWindow::onKeyUp(k);
   }
}

Window*CreateChannelList(){
    ChannelsWindow*w=new ChannelsWindow(0,0,1280,720);
    w->show();
    return w;
}
}//namespace
