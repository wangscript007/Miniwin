#include<ntvwindow.h>

namespace ntvplus{

#define W_ID     200
#define W_CHNAME 400
class CHEItem:public ListView::ListItem{
public:
    int id;
    SERVICELOCATOR svc;
public:
    CHEItem(int id_,const std::string&name,const SERVICELOCATOR&s):ListView::ListItem(name){
        id=id_;
        svc=s;
    }
};

static void CHEditorPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    CHEItem& md=(CHEItem&)itm;
    char buf[32];
    canvas.set_color(state?0xFF00FF00:lv.getBgColor());
    canvas.rectangle(itm.rect);
    canvas.fill();

    RECT r=itm.rect;
    canvas.set_color(lv.getFgColor());
    sprintf(buf,"%3d",md.id);
    r.width=W_ID-50;
    canvas.draw_text(r,buf,DT_RIGHT|DT_VCENTER);//channel id
    r.offset(W_ID,0);
    canvas.draw_text(r,md.getText(),DT_LEFT|DT_VCENTER);//channel name

    r.offset(W_CHNAME,0);//program width
    //canvas.draw_text(r,md.getText(),DT_LEFT|DT_VCENTER);//media size
    //r.offset(W_SIZE,0);
}
static INT SVC_CBK2(const SERVICELOCATOR*loc,const DVBService*svc,const BYTE*pmt,void*userdata);
class ChannelEditorWindow:public NTVWindow{
protected:
    ToolBar*header;
    ToolBar*columns;
    ListView*chlst;
public:
    ChannelEditorWindow(int x,int y,int w,int h);
};

ChannelEditorWindow::ChannelEditorWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
    
    header=CreateNTVToolBar(1280,40);

    header->addButton("Edit Channels",60,200);
    header->addButton("Set Favorites",-1,200);
    addChildView(header)->setPos(0,70);
    
    columns=new ToolBar(1280,40);
    columns->addButton("Number",100,W_ID);
    columns->addButton("Channel Name",-1,W_CHNAME);
    columns->setBgColor(getBgColor());
    columns->setFgColor(getFgColor());
    addChildView(columns)->setPos(0,110);
    
    
    chlst=new ListView(1100,500);
    chlst->setItemSize(-1,CHANNEL_LIST_ITEM_HEIGHT);
    chlst->setBgColor(getBgColor());
    chlst->setFgColor(getFgColor());
    chlst->setItemPainter(CHEditorPainter);
    addChildView(chlst)->setPos(90,150);
   
    addTipInfo("help_icon_exit.png","Exit",120);
    addTipInfo("help_icon_red.png","Set Favorites",200);
    addTipInfo("help_icon_green.png","Delete",120);
    addTipInfo("help_icon_yellow.png","Lock",120);
    addTipInfo("help_icon_blue.png","Move",120);
    DtvEnumService(SVC_CBK2,chlst);
}


Window*CreateChannelEditWindow(){
    NTVWindow*w=new ChannelEditorWindow(0,0,1280,720);
    w->setText("ChannelEditor");
    w->show();
    return w;
}
static INT SVC_CBK2(const SERVICELOCATOR*loc,const DVBService*svc,const BYTE*pmt,void*userdata)
{
   ListView*lv=(ListView*)userdata;
   char servicename[128];
   svc->getServiceName(servicename);
   lv->addItem(new CHEItem(lv->getItemCount(),servicename,*loc));
   return 1;
}

}//namespace
