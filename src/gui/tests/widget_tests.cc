#include <gtest/gtest.h>
#include <windows.h>
#include <ngl_os.h>
#include <channelepgview.h>
#include <seekbar.h>
#include <expandablelistview.h>
#include<ngl_timer.h>

using namespace nglui;
#define ID_OK 10
#define ID_CANCEL 15
#define ID_LISTVIEW 20
#define ID_TIPINFO 30

class WIDGET:public testing::Test{

   public :
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

static void onClick(View&v){
    std::string txt="You clicked:";
    txt+=((Widget&)v).getText();
    txt+="   id:"+std::to_string(v.getId());
    Toast::makeText(txt,2000)->setPos(200,v.getId()*20);
}

TEST_F(WIDGET,TextView){
    App app;
    Window*w=new Window(0,0,1280,720);
    const char*strings[]={"LEFT","CENTER","RIGHT","LEFT|TOP","CENTER|TOP","RIGHT|TOP"};
    const int align[]={DT_LEFT,DT_CENTER,DT_RIGHT,DT_LEFT|DT_TOP,DT_CENTER|DT_TOP,DT_RIGHT|DT_TOP};
    w->setLayout(new LinearLayout());
    TextView*t1=new TextView("",400,200);
    TextView*t2=new TextView(std::string(),400,300);
    w->addChildView(t1);
    w->addChildView(t2); 
    app.exec();
}

TEST_F(WIDGET,Button){
   App app;
   Window*w=new Window(100,100,800,600);
   w->setLayout(new LinearLayout());
   Button*btn1=new Button("OK",100,30);
   Button*btn2=new Button("Cancel",100,30);
   btn1->setId(ID_OK);
   btn1->setClickListener(onClick); //it's same as following lambda segment
   btn1->setClickListener([](View&v){
       std::string txt="You clicked:";
       txt+=((Widget&)v).getText();
       txt+="   id:"+std::to_string(v.getId());
       Toast::makeText(txt,2000)->setPos(200,v.getId()*20);
   });
   //btn2->setOnClickListener(click);//it click listener is not set ,view's parent will recv WM_CLICK message
   w->addChildView(btn1);
   w->addChildView(btn2);
   btn2->setId(ID_CANCEL);
   app.exec();
}

TEST_F(WIDGET,EditText){
    const std::string sss[]={
         "Normal","","",
         "A-Z","Abc","^[A-Za-z]+$",
         "Digit","123","^[1-9]\\d*$",
         "IP","0.0.0.0","^((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))$"//"\\d+\\.\\d+\\.\\d+\\.\\d+",
    };
    App app;
    Window*w=new Window(100,100,800,600);
    w->setLayout(new LinearLayout());
    EditText *e=new EditText(780,40);
    e->setHint("Input Text Here!");
    w->addChildView(e);
    for(int i=0;i<4;i++){
        EditText *e=new EditText(780,30+i*5);
        e->setBgColor(0Xff444444);
        e->setLabelWidth(300);
        e->setLabel(sss[i*3]); 
        e->setFontSize(30+i*5);
        e->setText(sss[i*3+1]);
        e->setPattern(sss[i*3+2]);     
        w->addChildView(e);
        e->setEditMode(i!=2?EditText::INSERT:EditText::REPLACE);
    }
    app.exec();
}

TEST_F(WIDGET,ProgressBar){
    App app;
    int pos=0;
    Window*w=new Window(100,100,800,600);
    ProgressBar*pb;
    w->setLayout(new LinearLayout());
    pb=new ProgressBar(800,20);
    w->addChildView(pb)->setId(100);
    pb->setProgress(30);
    w->addChildView(new ProgressBar(30,200))->setId(101);
    pb=new ProgressBar(800,20);
    w->addChildView(pb);
    pb->setIndeterminate(true);
    w->addChildView(new ProgressBar(200,200))->setId(102);
    pb=new ProgressBar(200,200);
    w->addChildView(pb);
    pb->setIndeterminate(true);

    w->sendMessage(View::WM_TIMER,0,0,500);
    w->setMessageListener([&](View&v,DWORD msg,DWORD wp,ULONG lp)->bool{
       if(msg==View::WM_TIMER){
       ((ProgressBar*)v.findViewById(100))->setProgress(pos);
       ((ProgressBar*)v.findViewById(101))->setProgress(pos);
       ((ProgressBar*)v.findViewById(102))->setProgress(pos);
       pos=(pos+1)%100;
       v.sendMessage(View::WM_TIMER,0,0,100);printf("=========ProgressBar::timer\r\n");
       return true;
       }
    });
    app.exec();
}

TEST_F(WIDGET,SeekBar){
    App app;
    Window*w=new Window(100,100,800,600);
    w->setLayout(new LinearLayout());
    SeekBar*sb=new SeekBar(400,40);
    w->addChildView(sb);
    app.exec();
}

static const char*texts[]={
  "ios_base::beg","beginning of the stream",
  "ios_base::cur","current position in the stream",
  "ios_base::end","end of the stream"
};

static void onItemSelect(AbsListView&lv,int index){
     std::string txt="You Select Item:"+std::to_string(index)+" of view id:"+std::to_string(lv.getId());
     Toast::makeText(txt,2000)->setPos(200,lv.getId()*20);
};

TEST_F(WIDGET,Selector){
    App app;
    Window*w=new Window(100,100,800,620);

    Selector*ls=new Selector("CHOISE",600,30);
    ls->setId(23);ls->setLabelWidth(120);
    ls->setPopupRect(600,100,300,620);
    w->addChildView(ls)->setPos(50,50);//3

    TextView*tt=new TextView("Tips:",500,40);
    tt->setId(ID_TIPINFO);
    w->addChildView(tt)->setPos(100,100);//4
    int cnt=sizeof(texts)/sizeof(char*);
    for(int i=0;i<cnt;i++){
         ls->addItem(new ListView::ListItem(texts[i%cnt]));
    }
    ls->setItemSelectListener([&](AbsListView&lv,const ListView::ListItem&itm,int index){
         std::string txt="You Select Item:"+itm.getText();
         tt->setText(txt); 
    });
    app.exec();
}

TEST_F(WIDGET,ListView){
    App app;
    Window*w=new Window(100,100,800,620);    
    ListView*lv=(ListView*)w->addChildView(new ListView(300,500));
    int cnt=sizeof(texts)/sizeof(char*);
    for(int i=0;i<cnt;i++){
         lv->addItem(new ListView::ListItem(texts[i]));
    }
    lv->setItemSelectListener([](AbsListView&lv,const ListView::ListItem&itm,int index){
         std::string txt="You Select Item:"+std::to_string(index)+" of view id:"+std::to_string(lv.getId());
         Toast::makeText(txt,2000)->setPos(200,index*20);
    });
    app.exec();
}
TEST_F(WIDGET,ExpandableListView){
    App app;
    Window *w=new Window(100,100,800,600);
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

TEST_F(WIDGET,ChannelEPGView){
    App app;
    Window*w=new Window(100,100,800,620);
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

TEST_F(WIDGET,Keyboard){
    App app;
    Window*w=new Keyboard(200,200,800,300);
    app.exec();
}
