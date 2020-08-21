#include <windows.h>
#include <appmenus.h>
#include <dvbepg.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_pvr.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define IDC_CHANNELS 100
#define IDC_MEDIALIST 101

NGL_MODULE(TVRECORD)
#define W_ID      100
#define W_PROGRAM 500
#define W_CHANNEL 300
#define W_DATE    250

#define W_EVENT   320
#define W_ACTION  160
#define W_CYCLE   150
namespace ntvplus{

class PVRItem:public ListView::ListItem{
public:
    int id;
    std::string fname;
    std::string channel;
    std::string datetime;
public:
    PVRItem(int id_,const std::string&name,std::string ch):ListView::ListItem(name){
        id=id_;
        channel=ch;
    }
};

static void PVRPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    PVRItem& md=(PVRItem&)itm;
    char buf[32];
    canvas.set_color(state?0xFF00FF00:lv.getBgColor());
    canvas.rectangle(itm.rect);
    canvas.fill();
     
    RECT r=itm.rect;
    canvas.set_color(lv.getFgColor());
    sprintf(buf,"%3d",md.id);
    canvas.draw_text(r,buf,DT_LEFT|DT_VCENTER);//id
    r.offset(W_ID,0);//id width;
    canvas.draw_text(r,md.getText(),DT_LEFT|DT_VCENTER);//;program
   
    NGLOG_DEBUG("=======draw pvritem %s",md.getText().c_str()); 
    r.offset(W_PROGRAM,0);//program width
    canvas.draw_text(r,md.fname,DT_LEFT|DT_VCENTER);//channel
    r.offset(W_CHANNEL,0);
    canvas.draw_text(r,md.datetime,DT_LEFT|DT_VCENTER);//datetime
}

class RecordItem:public ListView::ListItem{
public:
};

static void SchedulePainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
}
class PVRWindow:public NTVWindow{
protected:
   ToolBar*mdtype;
   ToolBar*header;
   ListView*mdlist;
   std::string filters;
   std::string pvr_path;
   std::string schedule_path;
public:
   PVRWindow(int x,int y,int w,int h);
   int loadPVR(const std::string&path);
   int loadSchedules(const std::string path);
   void setMode(int md);
   virtual bool onKeyUp(KeyEvent&k)override{
       switch(k.getKeyCode()){
       case KEY_LEFT: 
       case KEY_RIGHT:{
               bool rc=mdtype->onKeyUp(k);
               if(mdtype->getIndex()==0)
                   loadPVR(pvr_path);
               else
                   loadSchedules(schedule_path);
               return rc;
            }
       case KEY_ENTER:
            if(mdtype->getIndex()==0){
                PVRItem*itm=(PVRItem*)mdlist->getItem(mdlist->getIndex());
                nglPvrPlayerOpen(itm->getText().c_str());
            }
            break;
       default:return NTVWindow::onKeyUp(k);
       }
   }
   static void onMediaItemClick(View&lv){
      ListView*mdlist=(ListView*)lv.getParent()->findViewById(IDC_MEDIALIST);
      int index=mdlist->getIndex();
      PVRItem*itm=(PVRItem*)mdlist->getItem(index);
   }
};
PVRWindow::PVRWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
    initContent(NWS_TITLE|NWS_TOOLTIPS);
    mdtype=CreateNTVToolBar(1200,30);

    mdtype->setPos(40,70);
    mdtype->addButton("PVR",-1,120);
    mdtype->addButton("Record Schedule",-1,320);
    mdtype->setIndex(0);
    addChildView(mdtype);

    header=new ToolBar(1200,30);
    header->setBgColor(0xFF000000);
    header->setPos(40,100);
    addChildView(header);
       
    mdlist=new ListView(1200,CHANNEL_LIST_ITEM_HEIGHT*13);
    mdlist->setPos(40,130);
    mdlist->setItemSize(-1,40);
    mdlist->setFlag(SCROLLBARS_VERTICAL);
    mdlist->setBgColor(getBgColor());
    mdlist->setFgColor(getFgColor());
    mdlist->setItemPainter(PVRPainter);
    addChildView(mdlist)->setId(IDC_MEDIALIST);
    mdlist->setClickListener(onMediaItemClick);
}

void PVRWindow::setMode(int md){
    header->clear();
    mdlist->clear();
    if(md==0){
        header->addButton("NO.",-1,W_ID);
        header->addButton("Program",-1,W_PROGRAM);
        header->addButton("Channel",-1,W_CHANNEL);
        header->addButton("Date/Time",-1,W_DATE);
        mdlist->setItemPainter(PVRPainter);
        addTipInfo("help_icon_4arrow.png","Navigation",160);
        addTipInfo("help_icon_ok.png","Select",160);
        addTipInfo("help_icon_exit.png","Exit",260);
        addTipInfo("help_icon_red.png","Rename",160);
        addTipInfo("help_icon_green.png","Delete",160);
        addTipInfo("help_icon_yellow.png","DeleteAll",160);
    }else{
        header->addButton("Event",-1,W_EVENT);
        header->addButton("Channel",-1,W_CHANNEL); 
        header->addButton("Action",-1,W_ACTION);
        header->addButton("Cycle",-1,W_CYCLE);
        header->addButton("Date/Time",-1,W_DATE);
        mdlist->setItemPainter(SchedulePainter);
        addTipInfo("help_icon_ok.png","Navigation",160);
        addTipInfo("help_icon_red.png","Delete",160);
        addTipInfo("help_icon_green.png","Add",160);
        addTipInfo("help_icon_yellow.png","Sort",160);
        addTipInfo("help_icon_save.png","Save",260);
    }
}

int PVRWindow::loadPVR(const std::string&path){
    int count=0;
    DIR *dir=opendir(path.c_str());
    struct dirent *ent;
    setMode(0);
    mdlist->clear();
    NGLOG_DEBUG("%s",path.c_str());
    pvr_path=path;
    NGLOG_DEBUG("loadpvr from %s  dir=%p",path.c_str(),dir);
    if(dir==nullptr)return 0;
    while(ent=readdir(dir)){
        std::string fname=path+"/"+ent->d_name;
        NGLOG_DEBUG("pvr item %s",fname.c_str());
        mdlist->addItem(new PVRItem(count++,fname,std::string()));
    }
    closedir(dir);
    return count;
}

int PVRWindow::loadSchedules(const std::string path){
    schedule_path=path;
    setMode(1);
    return 0;
}

Window*CreatePVRWindow(){
    PVRWindow*w=new PVRWindow(0,0,1280,720);
    w->setText("TV Records");
    w->loadPVR("/mnt/usb/sda1/ALIDVRS2");
    w->show();
    return w;
}
}//namespace
