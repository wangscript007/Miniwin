#include<windows.h>
#include<ngl_log.h>
NGL_MODULE(MAIN)
int main(int argc,const char*argv[]){

    App app(argc,argv);
    Window*w=new Window(100,100,800,600);
    w->addChildView(new TextView("Hello world!",798,200))->setPos(1,1).setFontSize(100);

    TextView*tv=new TextView("E600 -0 \nReciver is not yet enabled to view this service to your receiver,"
            "call <+ 4414 4940283>.Your receiver number is:<3910714199(4) 8>",440,150);
    tv->setMultiLine(true);
    w->addChildView(tv)->setPos(100,210).setFlag(View::BORDER);
    NGLOG_DEBUG("prepare to run app\r\n");
    return app.exec();
}
