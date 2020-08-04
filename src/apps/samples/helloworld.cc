#include<windows.h>
#include<ngl_log.h>
NGL_MODULE(MAIN)
int main(int argc,const char*argv[]){

    App app(argc,argv);
    NGLOG_DEBUG("creating window...\r\n");
    Window*w=new Window(100,100,800,600);
    NGLOG_DEBUG("window created\r\n");
    w->addChildView(new TextView("HelloWorld!",800,80))->setFontSize(100);
    NGLOG_DEBUG("prepare to run app\r\n");
    return app.exec();
}
