#include<windows.h>
#include<ngl_log.h>
#include<gridview.h>
NGL_MODULE(SIMPLEWIN)

static bool onKey(int key){
    NGLOG_DEBUG("rcvkey %d c++=%d",key,__cplusplus);
    switch(key){
    case KEY_ESC:exit(0);
    default:
    Window*w=new Window(100,100,800,600);
    w->setLayout(new LinearLayout());
    w->addChildView(new TextView("HelloWorld!",400,80));
    }
}

int main(int argc,const char*argv[]){
    App app(argc,argv);
    Window*w=new Window(0,0,1280,720);
    w->setLayout(new LinearLayout());
    Button *btn=new Button("Button",200,50);
    View*vv=new View(200,100);
    w->addChildView(btn);
    w->addChildView(vv);
    w->addChildView(new Button("OK",200,50));
    w->addChildView(new EditText("EditText",300,50));
    ListView*lv=new ListView(600,400);
    GridView*gv=new GridView(600,600);
    ToolBar*tb1=new ToolBar(1000,40);
    ToolBar*tb2=new ToolBar(1000,80);
    tb1->setFlag(View::FOCUSABLE);
    NGLOG_DEBUG("listview's bgcolor=%08x",lv->getBgColor());
    lv->setItemSize(0,50);
    gv->setItemSize(100,100);
    for(int i=0;i<58;i++){
       char s[32];
       sprintf(s,"item %d",i);
       lv->addItem(new ListView::ListItem(s,i));
       gv->addItem(new ListView::ListItem(s,i));
       if(i<6){
           tb1->addButton(s,100,i);
           tb2->addButton(s,100,i);
       } 
    }
    lv->setIndex(0);
    w->addChildView(lv);
    w->addChildView(gv);
    w->addChildView(tb1);
    w->addChildView(tb2);
    w->setMessageListener([&](View&v,DWORD msg,DWORD wp,ULONG lp)->bool{
       if(msg==1000){
          vv->invalidate(nullptr);
          v.sendMessage(msg,wp,lp,50);
       }
       return msg==1000;
    });
    w->sendMessage(1000,0,0);
    return app.exec();
}
