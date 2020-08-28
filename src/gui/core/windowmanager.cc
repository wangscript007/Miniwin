/*
 * Copyright (C) 2015 UI project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <windowmanager.h>
#include <ngl_msgq.h>
#include <ngl_os.h>
#include <ngl_log.h>
#include <ngl_timer.h>
#include <graph_device.h>
#include <uieventsource.h>

NGL_MODULE(WINDOWMANAGER);

namespace nglui {
// Initialize the instance of the singleton to nullptr
WindowManager* WindowManager::instance = nullptr;

class WindowEventSource:public EventSource{
private:
   typedef struct{
       Window*win;
       int action;
       void*userdata;
   }WINACTION;
   std::list<WINACTION>windows;
public:
   WindowEventSource():EventSource(){};
   bool prepare(int&) override{return windows.size()>0;}
   bool check()override{return windows.size()>0;}
   bool dispatch(nglui::EventHandler&fun)override{
       if(fun)fun(*this);
   }
   void push_window(Window*w,int act,void*p){windows.push_back({w,0,p});}
   void processEvents(){
      for(auto a:windows){
         Window*w=a.win;
         if(w->hasFlag(View::FOCUSABLE)&&a.action==0){
             if(a.userdata)((Window*)a.userdata)->onDeactive();
             w->onActive();
         }
         w->onResize(w->getWidth(),w->getHeight());
      }
      windows.clear();
   }
};

WindowManager::WindowManager(){
     GraphDevice::getInstance();
     activeWindow=nullptr;
     winSource=new WindowEventSource();
     Looper::getDefault()->add_event_source(winSource,[this](EventSource&s)->bool{
         ((WindowEventSource&)s).processEvents();
         return true;
     });
}

WindowManager&WindowManager::getInstance(){
    if(nullptr==instance){
        instance=new WindowManager();
    }
    return *instance;
};

WindowManager::~WindowManager() {
    windows.erase(windows.begin(),windows.end());
    NGLOG_DEBUG("%p Destroied",this);
}

void WindowManager::addWindow(Window*win){
    windows.push_back(win);
    std::sort(windows.begin(),windows.end(),[](Window*w1,Window*w2){
       return (w2->window_type-w1->window_type)>0;
    });
    
    for(int idx=0,type_idx=0;idx<windows.size();idx++){
        Window*w=windows.at(idx);
        if(w->window_type!=windows[type_idx]->window_type)
            type_idx=idx;
        w->mLayer=w->window_type*10000+(idx-type_idx)*5;
        NGLOG_VERBOSE("addwin %p window %p[%s] type=%d layer=%d",win,w,w->getText().c_str(),w->window_type,w->mLayer);
    }
    winSource->push_window(win,1,activeWindow);
    Looper::getDefault()->add_event_source(win->source,[](EventSource&e)->bool{
        return ((UIEventSource&)e).processEvents();
    });
    resetVisibleRegion();
    NGLOG_VERBOSE("win=%p source=%p windows.size=%d",win,win->source,windows.size());
}

void WindowManager::broadcast(DWORD msgid,DWORD wParam,ULONG lParam){
    for(auto win:windows)
       win->sendMessage(msgid,wParam,lParam);
}

int WindowManager::enumWindows(WNDENUMPROC cbk){
    int rc=0;
    for(auto win:windows)
       rc+=cbk(win);
    return rc;
}

RefPtr<Region>WindowManager::getVisibleRegion(int mLayer){
    for(auto win:windows)
        if(mLayer==win->mLayer)return win->vis_rgn;
    return nullptr;
}

void WindowManager::removeWindow(Window*w){
   if(w==activeWindow)activeWindow=nullptr;
   if(w->hasFlag(View::FOCUSABLE))
       w->onDeactive();
   for(auto win=windows.begin();win!=windows.end();win++){
       if((*win)==w){
            windows.erase(win);
            NGLOG_VERBOSE("remove win %p size=%dx%d source=%p wins.count=%d",w,w->getWidth(),w->getHeight(),w->source,windows.size());
            delete w;
            break;
       }
   }resetVisibleRegion();
   for(auto it=windows.rbegin();it!=windows.rend();it++){
       if((*it)->hasFlag(View::FOCUSABLE)){
          (*it)->onActive();
          activeWindow=(*it);
          break;
       } 
   }
   NGLOG_VERBOSE("w=%p windows.size=%d",w,windows.size());
}
void WindowManager::processEvent(InputEvent&e){
   switch(e.getType()){
   case EV_KEY: onKeyEvent((KeyEvent&)e); break;
   case EV_ABS: onMotion((MotionEvent&)e);break;
   default:break;
   }
}

void WindowManager::onBtnPress(MotionEvent&event) {
  // Notify the focused child
  for (auto& wind : windows) {
    if (wind->isFocused() == true) {
       //wind->onMousePress(event);
    }
  }
}

void WindowManager::onBtnRelease(MotionEvent&event) {
  // Notify the focused child
  for (auto& wind : windows) {
    if (wind->isFocused() == true) {
       //wind->onMouseRelease(event);
    }
  }
}

void WindowManager::onMotion(MotionEvent&event) {
   // Notify the focused child
   int x=event.getX();
   int y=event.getY();
   for (auto itr=windows.rbegin();itr!=windows.rend();itr++) {
      auto w=(*itr);
      if (w->pointInView(x,y)) {
          w->dispatchTouchEvent(event);
          break;
      }
   }
}

void WindowManager::onKeyEvent(KeyEvent&event) {
    // Notify the focused child
    for (auto itr=windows.rbegin() ;itr!= windows.rend();itr++) {
        Window*win=(*itr);
        if ( win->hasFlag(View::FOCUSABLE) ) {
            switch(event.getAction()){
            case KeyEvent::ACTION_UP  :
                NGLOG_VERBOSE("Window:%p Key:%s[%x]",win,event.getLabel(event.getKeyCode()),event.getKeyCode());
                win->onKeyUp(event);break;
            case KeyEvent::ACTION_DOWN:win->onKeyDown(event);break;
            default:break;
            }
            return;
        }
  }
}

void WindowManager::clip(Window*win){
    RECT rcw=win->getBound();
    for (auto wind=windows.rbegin() ;wind!= windows.rend();wind++){
        if( (*wind)==win )break;
        RECT rc=rcw;
        rc.intersect((*wind)->getBound());
        if(rc.empty())continue;
        rc.offset(-win->getX(),-win->getY());
        win->mInvalid->subtract((const RectangleInt&)rc); 
    }
}

void WindowManager::resetVisibleRegion(){
    for (auto w=windows.begin() ;w!= windows.end();w++){
       RECT rcw=(*w)->getBound();
       RefPtr<Region>newrgn=Region::create((RectangleInt&)rcw);
       for(auto w1=w+1;w1!=windows.end();w1++){
           RECT r=(*w1)->getBound();
           newrgn->subtract((const RectangleInt&)r);
       }
       newrgn->translate(-rcw.x,-rcw.y);
       RefPtr<Region>tmp=newrgn->copy();
       if(!tmp->empty()&&(*w)->vis_rgn){
           tmp->subtract((*w)->vis_rgn);//do_xor,subtract?
           if(!tmp->empty())
           (*w)->invalidate(tmp);
       }
       (*w)->vis_rgn=newrgn; 
       NGLOG_VERBOSE("window %p[%s] Layer=%d",(*w),(*w)->getText().c_str(),(*w)->mLayer);
       DumpRegion("visiblerect",newrgn);
    }
}

}  // namespace ui
