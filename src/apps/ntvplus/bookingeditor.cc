#include <windows.h>
#include <appmenus.h>
#include <dvbepg.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <assets.h>
#include <subscriber.h>
#include <sstream>
#include <iomanip>
NGL_MODULE(BOOKING)
#define IDC_CHANNELS 100
#define W_ID   100
#define W_NAME 500
#define W_TIME 300
#define W_TYPE 200

namespace ntvplus{

class BookingItem:public ListView::ListItem{
public:
public:
    system_clock::time_point time;
    int  type;
public:
    BookingItem(const std::string&name):ListView::ListItem(std::string()){
        setText(name);
    }
    BookingItem(const SubscribeItem&itm):ListView::ListItem(std::string()){
        time=itm.time;
        setId(itm.eventid);
        setText(itm.name);
        type=itm.type;
    }
    std::string type2Name(){
       switch(type){
       case 0:return "Run Once";
       case 1:return "Daily";
       case 2:return "Weekly";
       default:return "";
       }
    }
};

static void BookingPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas){
    BookingItem& md=(BookingItem&)itm;
    std::ostringstream oss;
    if(state){
       canvas.set_source_rgba(.6,0,0,.8);
       canvas.rectangle(itm.rect);
       canvas.fill();
    }
 
    RECT r=itm.rect;
    canvas.set_color(lv.getFgColor());
    oss<<md.getId();
    canvas.draw_text(r,oss.str(),DT_LEFT|DT_VCENTER);//id
    r.offset(W_ID,0);//id width;
    canvas.draw_text(r,md.getText(),DT_LEFT|DT_VCENTER);//name;
    r.offset(W_NAME,0);
    oss.str("");

    std::time_t tmTime = std::chrono::system_clock::to_time_t(md.time);
    oss<<ctime(&tmTime);//std::put_time(std::localtime(&tmTime), "%F %T");
    canvas.draw_text(r,oss.str(),DT_LEFT|DT_VCENTER);//draw datetime
    r.offset(W_TIME,0);
    oss.str(md.type2Name());
    canvas.draw_text(r,oss.str(),DT_LEFT|DT_VCENTER);//draw type
}

class BookingWindow:public NTVWindow{
protected:
   ToolBar*header;
   ListView*mdlist;
   bool sort_revert;
public:
   BookingWindow(int x,int y,int w,int h);
   ~BookingWindow(){
   }
   int loadBookings();
   virtual bool onKeyUp(KeyEvent&k)override;
};

BookingWindow::BookingWindow(int x,int y,int w,int h):NTVWindow(x,y,w,h){
    sort_revert=false;
    clearFlag(ATTR_ANIMATE_FOCUS);
    initContent(NWS_TITLE|NWS_TOOLTIPS);

    header=new ToolBar(1280,30);
    header->addButton(" NO.",W_ID);
    header->addButton("Name",W_NAME);
    header->addButton("Time",W_TIME);
    header->addButton("Type",W_TYPE);
    addChildView(header)->setPos(0,98).setBgColor(0x88000000);
       
    mdlist=new ListView(1200,CHANNEL_LIST_ITEM_HEIGHT*13);
    mdlist->setPos(40,130);
    mdlist->setItemSize(-1,CHANNEL_LIST_ITEM_HEIGHT);
    mdlist->setFlag(View::SCROLLBARS_VERTICAL);
    mdlist->setBgColor(getBgColor());
    mdlist->setFgColor(getFgColor());
    mdlist->setItemPainter(BookingPainter);
    addChildView(mdlist);

    mdlist->setItemClickListener([&](AbsListView&lv,const ListView::ListItem&itm,int index){
    });
}
bool BookingWindow::onKeyUp(KeyEvent&k){
    switch(k.getKeyCode()){
    case KEY_RED:
         mdlist->sort([](const ListView::ListItem&a,const ListView::ListItem&b)->int{
                            return strcmp(a.getText().c_str(),b.getText().c_str())>0;
                       },sort_revert); 
         sort_revert=!sort_revert;
         mdlist->invalidate(nullptr);
         break;
    default: return NTVWindow::onKeyUp(k);
    }
}

int BookingWindow::loadBookings(){
    int count=10;
    std::vector<SubscribeItem>items;
    Subscriber::getInstance()->getItems(items);
#if DEBUG
   if(items.size()==0)
   for(int i=0;i<10;i++){
       std::ostringstream oss;
       SubscribeItem itm;
       itm.eventid=i;
       oss<<"subscribe.test "<<i;
       itm.name=oss.str();
       itm.time=system_clock::now()+seconds(10+i*i);
       Subscriber::getInstance()->addOnce(itm); 
       items.push_back(itm);
   }
#endif
    for(auto a:items){
        BookingItem*itm=new BookingItem(a);
        mdlist->addItem(itm);
    } 
    return count;
}

Window*CreateBookingEditor(){
    NGLOG_DEBUG("");
    BookingWindow*w=new BookingWindow(0,0,1280,720);
    w->setText("Booking Manager");
    w->addTipInfo("help_icon_4arrow.png","Navigation",160);
    w->addTipInfo("help_icon_ok.png","Select",160);
    w->addTipInfo("help_icon_back.png","Back",160);
    w->addTipInfo("help_icon_exit.png","Exit",260);
    w->addTipInfo("help_icon_red.png","A->Z",160);
    w->addTipInfo("help_icon_green.png","Rename",160);
    w->addTipInfo("help_icon_yellow.png","Delete",160);
    w->show();
    w->loadBookings();
    return w;
}
}//namespace
