#include <uieventsource.h>
#include <windowmanager.h>
#include <ngl_log.h>
#include <ngl_timer.h>
#include <view.h>
#include <list>

NGL_MODULE(UIEVENTSOURCE);

namespace nglui{

bool operator>(const MSG&m1,const MSG m2){
   return m1.time>m2.time;
}

bool operator==(const MSG&m1,const MSG m2){
   return m1.msgid==m2.msgid;
}

UIEventSource::UIEventSource(){
}
UIEventSource::~UIEventSource(){
}

int UIEventSource::getEvents(){
    return normal_msgs.size()||hasDelayedMessage()||invalidates.size()||GraphDevice::getInstance().needCompose();
}

bool UIEventSource::processEvents(){
    bool rc=popMessage();//called by Dispatch,return false the  source will be killed
    if(GraphDevice::getInstance().needCompose())
       GraphDevice::getInstance().ComposeSurfaces();
    return rc;
}

void UIEventSource::reset(){
    while(!delayed_msgs.empty())delayed_msgs.pop_front();
    while(!normal_msgs.empty())normal_msgs.pop_front();
}

void UIEventSource::sendMessage(View*v,DWORD msgid,DWORD wp,ULONG lp,DWORD delayedtime){
    MSG msg={v,msgid,wp,lp,delayedtime};
    msg.view=v;
    msg.msgid=msgid;
    msg.wParam=wp;    
    msg.lParam=lp;
    if(msgid==View::WM_INVALIDATE){
        if((v->getParent()==nullptr)||(invalidates.find(v->getParent())==invalidates.end()))
            invalidates.emplace(v);
        return ;
    }
    if(delayedtime!=0){
        NGL_RunTime tnow;
        nglGetRunTime(&tnow);
        msg.time=tnow.uiMilliSec+tnow.uiMicroSec/1000+delayedtime;
        for(auto itr=delayed_msgs.begin();itr!=delayed_msgs.end();itr++){
           if(msg.time<itr->time){
               delayed_msgs.insert(itr,msg);
               return ;
           }
        }
        delayed_msgs.push_back(msg);
        return;
    }
    normal_msgs.push_back(msg);
}

bool UIEventSource::hasDelayedMessage(){
    NGL_RunTime tnow;
    MSG msg;
    if(delayed_msgs.empty())return false;
    nglGetRunTime(&tnow);
    DWORD nowms=tnow.uiMilliSec+tnow.uiMicroSec/1000;
    msg=delayed_msgs.front();
    return msg.time<nowms;
}

bool UIEventSource::popMessage(){
    MSG msg={0};
    if(normal_msgs.size()){
        msg=normal_msgs.front();
        normal_msgs.pop_front();
    }else if(hasDelayedMessage()){
        msg=delayed_msgs.front();
        delayed_msgs.pop_front();
    }
    if (invalidates.size()){
        std::for_each(invalidates.begin(),invalidates.end(),[](View*v){v->draw();});
        GraphDevice::getInstance().flip();
        invalidates.clear();
    }
    if(msg.view){
        msg.view->onMessage(msg.msgid,msg.wParam,msg.lParam);
    }
    if(msg.msgid==View::WM_DESTROY){
        WindowManager::getInstance().removeWindow(static_cast<Window*>(msg.view));
        return false;//return false to tell Eventsource::Dispath,and EventLooper will remove the source. 
    }
    return true;
}

bool UIEventSource::hasMessage(const View*v,DWORD msgid){
    const MSG msg={(View*)v,msgid};
    return delayed_msgs.end()==std::find(delayed_msgs.begin(),delayed_msgs.end(),msg);
}

void UIEventSource::removeMessage(const View*v,DWORD msgid){
    delayed_msgs.remove_if([&](MSG m)->bool{
        return m.msgid==msgid&&m.view==v;
    });
}

}//end namespace
