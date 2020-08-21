#include <gtest/gtest.h>
#include <windows.h>
#include <ngl_os.h>

using namespace nglui;

static const char*texts[]={"Creates 中国智造"," the specified format and dimensions.",
            "Initially the surface contents"," are set to 0.","(Specifically, within each pixel,",
            " each color or alpha channel","belonging to format will be 0.","The contents","of bits within a pixel,",
            " but not belonging","必须使用UTF8编码 " };
class WINDOW:public testing::Test{

   public :
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

class TestWindow:public Window{
public:
   TestWindow(int x,int y,int w,int h):Window(x,y,w,h){}
   virtual bool onKeyRelease(KeyEvent& evt){
     App::getInstance().exit(0);
     sendMessage(View::WM_DESTROY,0,0);
     return true;
   }
};

TEST_F(WINDOW,EmptyWindow){
   App app;
   Window*w=new TestWindow(100,100,800,600);
   app.exec();
}

static void ItemPainter(AbsListView&lv,const AbsListView::ListItem&itm,int state,GraphContext&canvas){
       RefPtr<Cairo::LinearGradient>lg=LinearGradient::create(0,0,800,600);//rect.x,rect.y,rect.width,rect.height);
       lg->add_color_stop_rgba(.0,(select?0:1.0),0,0,0.5);
       lg->add_color_stop_rgba(.5,.0,1.,.0,1.);
       lg->add_color_stop_rgba(1.,(select?1:0),0,(select?0:.1),0.5);
       //if(!select)
       canvas.set_source(lg);
       //else canvas.set_color(255,0,0);
       canvas.rectangle(itm.rect);canvas.fill();
       canvas.set_color(lv.getFgColor());
       canvas.draw_text(itm.rect,itm.getText(),DT_LEFT|DT_VCENTER);
} 
TEST_F(WINDOW,create){
   App app;
   Window*w=new TestWindow(100,100,1000,600);
   w->setLayout(new LinearLayout());
   for(int i=0;i<6;i++)
      w->addChildView(new TextView(texts[i]));
   for(int i=0;i<3;i++){
      Selector *ls=new Selector("Button",380,30);
      ls->setFontSize(20);
      if(i==0)ls->setLabelWidth(100);
      for(int i=0;i<8;i++)
         ls->addItem(new Selector::ListItem(texts[i]));
      ls->setIndex(i);
      w->addChildView(ls);
   }
   ListView*lv=new ListView(300,400);
   ListView*lv1=new ListView(300,400);
   ListView*lv2=new ListView(300,400);
   lv1->setItemPainter(ItemPainter);
   for(int i=0;i<18;i++){
       lv->addItem(new ListView::ListItem(texts[i%10]));
       lv1->addItem(new ListView::ListItem(texts[i%10]));
       lv2->addItem(new ListView::ListItem(texts[i%10]));
   }
   lv->setIndex(3);
   w->addChildView(lv);
   w->addChildView(lv1);
   w->addChildView(lv2);
   app.exec();
}

TEST_F(WINDOW,memleak){
   App app;
   Window*w=new Window(0,0,1280,720);
   nglSleep(10000);
   Button*btn=new Button("test",200,100);
   w->addChildView(btn);
   for(int i=0;i<1000000;i++){
       btn->invalidate(nullptr);
   }
}
TEST_F(WINDOW,LinearLayout){
   App app;
   Window*w=new TestWindow(100,100,820,620);
   GroupView*g1=new GroupView(0,0,800,400);
   GroupView*g2=new GroupView(0,0,800,200);
   w->addChildView(g1);    
   w->addChildView(g2);    
   w->setLayout(new LinearLayout());

   g1->setLayout(new LinearLayout());
   g2->setLayout(new LinearLayout());
   int count=sizeof(texts)/sizeof(char*);
   for(int i=0;i<8;i++){
       g1->addChildView(new Button(texts[i%count],200,40));
       g2->addChildView(new Button(texts[(i+7)%count],200,40));
   }
   for(int i=0;i<0;i++){
       ImageView*img=new ImageView("light.png",300-i*50,300-i*50);
       g1->addChildView(img);
       ImageView*img2=new ImageView("light.png",200-i*50,200-i*60);
       g2->addChildView(img2);
   }
   app.exec();
}

TEST_F(WINDOW,StackLayout){
   App app;
   Window*w=new TestWindow(100,100,820,620);
   w->setLayout(new StackLayout());
   for(int i=0;i<4;i++){
      ListView *lv=new ListView(400,200);
      w->addChildView(lv);
      for(int j=0;j<8;j++)lv->addItem(new ListView::ListItem(texts[j]));
      lv->setIndex(i); 
   }
   app.exec();
}

class MWindow:public TestWindow{
private:
   int dx,dy;
public:
   MWindow(int x,int y,int w,int h):TestWindow(x,y,w,h){dx=dy=50;}
   virtual bool onMessage(DWORD msgid,DWORD wp,ULONG lp){
      if(View::WM_TIMER==msgid){
         sendMessage(msgid,wp,lp,lp);
         int x=getX();
         int y=getY();
         if(x+getWidth()+dx>1280)dx*=-1;
         if(y+getHeight()+dy>720)dy*=-1;
         if(x+dx<0)dx*=-1;
         if(y+dy<0)dy*=-1;
         setPos(x+dx,y+dy);
         return true;
      }
      return false;
   }
   void setDir(int x,int y){dx=x;dy=y;}
};

TEST_F(WINDOW,multiwindow){
    App app; 
    MWindow*w3=new MWindow(200,300,410,280);
    MWindow*w1=new MWindow(100,100,400,320);
    MWindow*w2=new MWindow(300,200,420,300);
    w1->setLayout(new StackLayout());
    w2->setLayout(new LinearLayout());
    w1->setBgColor(0xFFFF0000);
    w2->setBgColor(0xFF00FF00);
    for(int i=0;i<4;i++){
      ListView *lv1=new ListView(400,200);
      ListView *lv2=new ListView(200,100);
      w1->addChildView(lv1);
      w2->addChildView(lv2);
      for(int j=0;j<8;j++){
           lv1->addItem(new ListView::ListItem(texts[j]));
           lv2->addItem(new ListView::ListItem(texts[j]));
      }
      lv1->setIndex(i);
      lv2->setIndex(i+i);
    }
    w1->sendMessage(View::WM_TIMER,0,100,100);w1->setDir(50,40);
    w2->sendMessage(View::WM_TIMER,0,100,80);w2->setDir(66,53);
    w3->sendMessage(View::WM_TIMER,0,100,85);w3->setDir(69,57);
    app.exec();
}

TEST_F(WINDOW,TOAST){
    App app;
    const static char*texts[]={"Hello world","Program is scrambed by provider","The quick brown fox jumps over a lazy dog",
       "Innovation in China","Innovation by Shenzhen"};
    Window*w1=new TestWindow(100,100,820,620);
    ListView *lv1=new ListView(400,600);
    w1->addChildView(lv1);
    int cnt=sizeof(texts)/sizeof(texts[0]);
    for(int i=0;i<12;i++)
       lv1->addItem(new ListView::ListItem(texts[i%cnt]));
    for(int i=0;i<8;i++){
       printf("toast text:%s\r\n",texts[i%cnt]);
       Toast::makeText(std::string(texts[i%cnt]),600+200*i)->setPos(i*80,i*40);
    }
    app.exec();
}
