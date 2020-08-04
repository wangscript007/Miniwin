#include <windows.h>
#include <ntvwindow.h>
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

class ChannelsWindow:public NTVWindow{
protected:
   ListView*chlst;
   TextView*namep,*descp;
   TextView*namef,*descf;
   ToolBar*tbfavs;
   std::vector<unsigned int>favgroups;
public:
   ChannelsWindow(int x,int y,int w,int h);
   virtual bool onKeyUp(KeyEvent&k)override;
   int loadGroups();
   int loadServices(UINT favid);
};

static bool EPGMessageListener(View&v,DWORD msg,DWORD wp,ULONG lp){
    if(msg==1000){
         TextView*namep,*namef,*descp,*descf;
         DVBEvent pf[2];
         NGLOG_VERBOSE("Get Event P/F to ui");
         char name[256]={0},des[256]={0};
         ListView*lv=(ListView*)v.findViewById(IDC_CHANNELS);
         int idx=lv->getIndex();
         if(idx<0)return false;
         ChannelItem*itm=(ChannelItem*)lv->getItem(idx);
         int rc=DtvGetPFEvent(&itm->svc,pf);
         NGLOG_VERBOSE("DtvGetPFEvent=%d",rc); 
         namep=(TextView*)v.findViewById(IDC_NAMEP);
         namef=(TextView*)v.findViewById(IDC_NAMEF);
         descp=(TextView*)v.findViewById(IDC_DESCP);
         descf=(TextView*)v.findViewById(IDC_DESCF);
         if(rc&1){pf[0].getShortName(name,des);namep->setText(name);descp->setText(des);}
         else {namep->setText("");descp->setText("");}
         if(rc&2){pf[1].getShortName(name,des);namef->setText(name);descf->setText(des);}
         else {namef->setText("");descf->setText("");}
    }
}
ChannelsWindow::ChannelsWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
    initContent(NWS_TITLE|NWS_TOOLTIPS);
    setText("ChannelList");
    tbfavs=CreateNTVToolBar(1280,38);

    tbfavs->setPos(0,70);
    loadGroups();
    addChildView(tbfavs);

    chlst=new ListView(430,CHANNEL_LIST_ITEM_HEIGHT*13);
    chlst->setPos(20,110);
    chlst->setItemSize(-1,CHANNEL_LIST_ITEM_HEIGHT);
    chlst->setBgColor(0);
    chlst->setFgColor(getFgColor());
    chlst->setFlag(View::SCROLLBARS_VERTICAL);
    chlst->setFlag(View::BORDER);
    chlst->setItemPainter(ChannelPainterLCN);
    addChildView(chlst)->setId(IDC_CHANNELS);
    addChildView(new TextView("tpinfo",200,28))->setId(IDC_TPINFO).setPos(1000,630).setFontSize(20).setBgColor(0);
    chlst->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        ChannelItem&ch=(ChannelItem&)lvitem;
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
    });
    chlst->setItemClickListener([](AbsListView&lv,const ListView::ListItem&lvitem,int index){
        ChannelItem&ch=(ChannelItem&)lvitem;
        DtvPlay(&ch.svc,nullptr);
    });
    if(favgroups.size())loadServices(favgroups[0]);
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
    setMessageListener(EPGMessageListener);
}

bool ChannelsWindow::onKeyUp(KeyEvent&k){
   int idx;
   switch(k.getKeyCode()){
   case KEY_LEFT :
   case KEY_RIGHT:
          tbfavs->onKeyUp(k);
          idx=tbfavs->getIndex();
          loadServices(favgroups[idx]);
          return true;
   case KEY_ENTER:
        {
            int idx=chlst->getIndex();
            ChannelItem*itm=(ChannelItem*)chlst->getItem(idx);
            NGLOG_VERBOSE("itm=%p",itm);
            if(itm)DtvPlay(&itm->svc,nullptr); 
        }break;
   case KEY_YELLOW:CreateChannelEditWindow();return true;
   default:return NTVWindow::onKeyUp(k);
   }
}

int ChannelsWindow::loadGroups(){
     int count=FavGetGroupCount();
     for(int i=0;i<count;i++){
          char name[64];
          UINT favid;
          FavGetGroupInfo(i,&favid,name);
          favgroups.push_back(favid);
          tbfavs->addItem(new ToolBar::Button(name,favid));
          NGLOG_VERBOSE("%x %s %d services",favid,name,FavGetServiceCount(favid));
     }
     tbfavs->setIndex(0);
     return count;
}

int ChannelsWindow::loadServices(UINT favid){
     char name[128];
     size_t count=FavGetServiceCount(favid);
     FavGetGroupName(favid,name);
     NGLOG_DEBUG("%x[%s] has %d svc",favid,name,count);
     chlst->clearAllItems();
     SERVICELOCATOR cur;
     DtvGetCurrentService(&cur);
     for(size_t i=0;i<count;i++){
          SERVICELOCATOR svc;
          FavGetService(favid,&svc,i);
          const DVBService*info=DtvGetServiceInfo(&svc);
          if(NULL==info)continue;
          info->getServiceName(name);
          ChannelItem*ch=new ChannelItem(name,&svc,info->freeCAMode); 
          INT lcn;
          DtvGetServiceItem(&svc,SKI_LCN,&lcn);
          ch->setId(lcn);
          ch->isHD=ISHDVIDEO(info->serviceType);
          NGLOG_VERBOSE("    %d %d.%d.%d.%d:%s  %p hd=%d type=%d",i,svc.netid,svc.tsid,svc.sid,svc.tpid,name,info,ch->isHD,info->serviceType);
          chlst->addItem(ch);
          if(svc.sid==cur.sid&&svc.tsid==cur.tsid&&cur.netid==svc.netid)
             chlst->setIndex(i);
     }
     NGLOG_DEBUG("%d services loaded CUR:%d.%d.%d index=%d",chlst->getItemCount(),cur.netid,cur.tsid,cur.sid,chlst->getIndex());
     chlst->sort([](const ListView::ListItem&a,const ListView::ListItem&b)->int{
                            return a.getId()-b.getId()<0;
                       },false);
}

Window*CreateChannelList(){
    ChannelsWindow*w=new ChannelsWindow(0,0,1280,720);
    w->show();
    return w;
}
}//namespace
