#include<appmenus.h>
#include<ntvwindow.h>
#include<ngl_mediaplayer.h>
#include<docview.h>
namespace ntvplus{

#define MSG_CURPOS 1010
class ControlWindow:public NTVWindow{
protected:
   std::vector<std::string>files;
   int cur_index;
   HANDLE player;
   DocumentView*vv;
   ProgressBar*progress;
public:
   ControlWindow(int x,int y,int w ,int h);
   ~ControlWindow(){
       if(player)nglMPClose(player);
   }
   void setPlayer(HANDLE h){player=h;};
   virtual bool onMessage(DWORD msg,DWORD wp,ULONG lp)override{
        if( (msg==MSG_CURPOS) && player){
            UINT pos,duration;
            int rc=nglMPGetTime(player,&pos,&duration);
            if(rc==NGL_OK&&duration>0){
                progress->setProgress(pos);
                progress->setMax(duration);
            }sendMessage(msg,wp,lp,1000);
            return  true;
        }
        return NTVWindow::onMessage(msg,wp,lp);
   }
};

ControlWindow::ControlWindow(int x,int y,int w ,int h)
  :NTVWindow(x,y,w,h){
    initContent(NWS_TITLE|NWS_TOOLTIPS);
    
    vv=new DocumentView(1280,600);
    vv->setFlag(View::BORDER);
    vv->setPos(0,70);
    vv->setBgColor(0x0000).setFgColor(0xFFFF0000);
    addChildView(vv);
  
    progress=new NTVProgressBar(1280,8);
    addChildView(progress)->setPos(0,660);
    player=nullptr;
    sendMessage(MSG_CURPOS,0,0,1000);
}

Window*CreatePlayerCtrlWindow(HANDLE player,const std::string&url){
    ControlWindow*w=new ControlWindow(0,0,1280,720);
    w->setText("PlayCtrl");
    w->setPlayer(player);
    w->show();
    return w;
}


}//namespace
