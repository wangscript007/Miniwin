#include<ntvwindow.h>
#include<ngl_timer.h>
#include<ngl_tuner.h>

NGL_MODULE(NTVWindow);

namespace ntvplus{

#define WM_GETSIGNAL 1001
#define WM_TIME_UPDATE 1002

static void EPG_CBK(UINT msgtype,const SERVICELOCATOR*svc,DWORD wp,ULONG lp,void*userdata){
    NTVWindow*w=(NTVWindow*)userdata;
    switch(msgtype){
    case MSG_EPG_PF:w->onEITPF(svc);break;
    case MSG_EPG_SCHEDULE:w->onEITS(svc);break;
    }
}

NTVWindow::NTVWindow(int x,int y,int w,int h):Window(x,y,w,h){
    clearFlag(BORDER);
    setFlag(ATTR_ANIMATE_FOCUS);
    setFgColor(0xFFFFFFFF);
    RefPtr<SurfacePattern>pat=SurfacePattern::create(App::getInstance().loadImage("light2.jpg"));//im_background.png"));
    pat->set_extend(Pattern::Extend::REPEAT);
    setBgPattern(pat);    
 
    titlebar=nullptr;
    sig_strength=sig_quality=nullptr;

    toolbar=nullptr;
    handle_notify=DtvRegisterNotify(EPG_CBK,this);
    startAnimation(MAKEPOINT(x+w,y),MAKEPOINT(x,y));
}
void NTVWindow::initContent(int styles){
    if(styles&NWS_TITLE){
        titlebar=new NTVTitleBar(getWidth(),68);
        addChildView(titlebar);
        sendMessage(WM_TIME_UPDATE,0,0,500); //this line  can open window's datetime update 
    }
    if(styles&NWS_SIGNAL){
        sig_strength=new NTVProgressBar(200,8);
        sig_quality=new NTVProgressBar(200,8);
        addChildView(sig_strength); 
        addChildView(sig_quality);
        sendMessage(WM_GETSIGNAL,0,0,500);
    }
    if(styles&NWS_TOOLTIPS){
        toolbar=CreateNTVToolBar(getWidth(),38);
        toolbar->setPos(0,getHeight()-42);
        addChildView(toolbar);
    }
}
NTVWindow::~NTVWindow(){
    DtvUnregisterNotify(handle_notify);
}


void NTVWindow::onEITPF(const SERVICELOCATOR*svc){
    DVBEvent pf[2];
    DtvGetPFEvent(svc,pf);
}

void NTVWindow::onEITS(const SERVICELOCATOR*svc){
#if 0 
    std::vector<DVBEvent>evts;
    DtvGetEvents(svc,evts);
    for(auto e:evts){
        char sname[256],des[256];
        e.getShortName(sname,des);
        printf("%s\r\n",sname);
    }
#endif
}

void NTVWindow::addTipInfo(const std::string&img,const std::string&title,int width){
    if(toolbar)toolbar->addButton(img,title,width);
}

void NTVWindow::clearTips(){
    if(toolbar)toolbar->clearAllItems();
}
void NTVWindow::setText(const std::string&txt){
    title=txt;
    if(titlebar)titlebar->setTitle(txt);
}
const std::string& NTVWindow::getText()const{
    return title;
}
bool NTVWindow::onMessage(DWORD msgid,DWORD wParam,ULONG lParam){
    switch(msgid){
    case WM_GETSIGNAL:
        if( sig_strength && sig_quality){
            NGLTunerState ts;
            nglTunerGetState(0,&ts);
            sig_strength->setProgress(ts.strength);
            sig_quality->setProgress(ts.quality);
            sendMessage(msgid,wParam,lParam,200);
        }return true;
    case WM_TIME_UPDATE:
        {
           NGL_TIME now;nglGetTime(&now);
           titlebar->setTime(now);
           sendMessage(msgid,wParam,lParam,995);
           return true; 
        }
    default:return Window::onMessage(msgid,wParam,lParam);
    }
}

}//namespace

