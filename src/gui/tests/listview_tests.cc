#include <gtest/gtest.h>
#include <windows.h>
#include <ngl_os.h>
#include <channelepgview.h>
#include <expandablelistview.h>
#include <gridview.h>
#include<ngl_timer.h>

using namespace nglui;

class LISTVIEW:public testing::Test{

   public :
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

TEST_F(LISTVIEW,Selector){
    App app;
    Window*w=new Window(0,0,1280,720);
    const char*strings[]={"LEFT","CENTER","RIGHT","LEFT|TOP","CENTER|TOP","RIGHT|TOP"};
    const int align[]={DT_LEFT,DT_CENTER,DT_RIGHT,DT_LEFT|DT_TOP,DT_CENTER|DT_TOP,DT_RIGHT|DT_TOP};
    Selector*s1=new Selector("Alignment",200,40);
    Selector*s2=new Selector("Alignment",400,40);
    Selector*s3=new Selector("Alignment",400,40);
    s2->setLabelWidth(200);
    s1->setItemSelectListener([&](AbsListView&v,const ListView::ListItem&itm,int idx){
        s3->setIndex(idx);
    });
    w->addChildView(s1)->setPos(100,100);
    w->addChildView(s2)->setPos(100,150);
    w->addChildView(s3)->setPos(100,200);
    for(int i=0;i<sizeof(strings)/sizeof(strings[0]);i++){
        s1->addItem(new Selector::ListItem(strings[i],i));
        s2->addItem(new Selector::ListItem(strings[i],i));
        s3->addItem(new Selector::ListItem(strings[i],i));
    } 
    app.exec();
}

static const char*texts[]={
  "ios_base::beg","beginning of the stream",
  "ios_base::cur","current position in the stream",
  "ios_base::end","end of the stream"
};

TEST_F(LISTVIEW,ListView){
    App app;
    Window*w=new Window(100,50,800,620);
    ListView*lv=(ListView*)w->addChildView(new ListView(300,500));
    lv->setPos(10,10);
    int cnt=sizeof(texts)/sizeof(char*);
    for(int i=0;i<cnt*6;i++){
         lv->addItem(new ListView::ListItem(texts[i%cnt]));
    }
    lv->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&itm,int index){
         std::string txt="You Select Item:"+std::to_string(index);
         Toast::makeText(txt,2000)->setPos(200,index*30);
    });
    app.exec();
}

TEST_F(LISTVIEW,GridView){
    App app;
    Window*w=new Window(100,50,800,640);
    GridView*g=new GridView(600,600);
    g->setItemSize(100,100);
    w->addChildView(g)->setPos(10,10);
    for(int i=0;i<100;i++){
       char str[32];
       sprintf(str,"item%02d",i);
       g->addItem(new GridView::ListItem(str,i));
    }
    app.exec();
}

TEST_F(LISTVIEW,ExpandableListView){
    App app;
    Window *w=new Window(100,50,800,600);
    ExpandableListView*ev=new ExpandableListView(800,600);
    w->addChildView(ev);
    for(int i=0;i<sizeof(texts)/sizeof(char*);i++){
       char str[32];
       sprintf(str,"items %d",i);
       TextView*tt=new TextView(texts[i]);
       tt->setBgColor(0xFF0000FF);
       w->addChildView(tt);
       printf("items[%d]=%s %p visible=%d\r\n",i,texts[i],tt,tt->hasFlag(View::VISIBLE)); 
       ExpandableListView::ExpandableListItem*itm=new ExpandableListView::ExpandableListItem((const char*)str,nullptr,i);
       itm->setBuddy(tt);
       itm->setBuddySize(800,160);
       ev->addItem(itm);
    }
    app.exec();
}

TEST_F(LISTVIEW,ChannelEPGView){
    App app;
    Window*w=new Window(100,50,800,620);
    ChannelEpgView*v=new ChannelEpgView(800,500);
    w->addChildView(v);
    ULONG start;
    for(int i=0;i<10;i++){
        nglGetTime(&start);
        ChannelBar*ch=new ChannelBar("channel:"+std::to_string(i));
        for(int j=0;j<20;j++){
            ULONG dur=30+(i*5+j)%30;dur*=60;
            ch->addEvent("",start,dur);start+=dur;
        }
        v->addItem(ch);
    }
    nglGetTime(&start);
    for(int i=0;i<60;i++){
       v->setStartTime(start);start+=300;
       v->invalidate(nullptr);
       nglSleep(100);
    }
    app.exec();
}

