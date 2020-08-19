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

#include <window.h>
#include <windowmanager.h>
#include <ngl_log.h>
#include <uieventsource.h>
#include <chrono>
#include <choreograph/Choreograph.h>


//#pragma GCC diagnostic ignored "-fpermissive"

NGL_MODULE(Window);

using namespace choreograph;
using namespace std::chrono;

namespace nglui {

Window::Window(int x,int y,int width,int height,int type)
  : INHERITED(x,y,width,height),source(nullptr),canvas(nullptr),window_type(type){
    // Set the boundary
    // Do the resizing at first time in order to invoke the OnLayout
    source=new UIEventSource();
    NGLOG_VERBOSE("%p source=%p visible=%d size=%dx%d",this,source,hasFlag(VISIBLE),width,height);
    setBound(x, y, width, height);
    WindowManager::getInstance().addWindow(this);
    //we need at least 1 invalidate after add UIEventSource to make sure the window have a invalidate message on message queue
    setFlag(BORDER);
    setFlag(FOCUSABLE);
}

void Window::show(){
    NGLOG_DEBUG("visible=%d",hasFlag(VISIBLE));
    if(!hasFlag(VISIBLE)){
        setFlag(VISIBLE);
        //draw(true);
        invalidate(nullptr);
    }
}

void Window::hide(){
    clearFlag(VISIBLE);
    GraphDevice::getInstance().remove(canvas);
    canvas=nullptr;
}

View& Window::setPos(int x,int y){
    if(x!=mBound.x || y!=mBound.y){
        mBound.x=x;
        mBound.y=y;
        WindowManager::getInstance().resetVisibleRegion();
        if(canvas){
           canvas->set_position(x,y);
           canvas->invalidate(getClientRect());
        }
    }
    return *this;
}

View& Window::setSize(int cx,int cy){
    if(cx!=getWidth()||cy!=getHeight()){
        mBound.width=cx;
        mBound.height=cy;
        WindowManager::getInstance().resetVisibleRegion();
        onResize(cx,cy);
    }
    return *this;
}

GraphContext*Window::getCanvas(){
//for children's canvas is allcated by it slef and delete by drawing thread(UIEventSource)
    if(canvas==nullptr){
        canvas=GraphDevice::getInstance().createContext(mBound);
        WindowManager::getInstance().resetVisibleRegion();
    }
    canvas->setLayer(mLayer);
    return canvas;
}

void Window::draw(){
    RectangleInt rc=mInvalid->get_extents();
    steady_clock::time_point t2,t1=steady_clock::now();
    GraphContext*c=getCanvas(); 
    c->set_antialias(ANTIALIAS_GRAY);
    c->set_font_size(getFontSize());
    c->set_source(mBgPattern);
    WindowManager::getInstance().clip(this);
    clip(*c);
    c->translate(getScrollX(),getScrollY());
    if(!mInvalid->empty()&&!vis_rgn->empty()){
        mInvalid->intersect(vis_rgn);
        NGLOG_VERBOSE("%p[%s]%d rects",this,getText().c_str(),vis_rgn->get_num_rectangles());
        onDraw(*c);
        drawChildren();
        c->invalidate(mInvalid);
    }
    resetClip();
    t2=steady_clock::now();
    duration<double>dur=duration_cast<duration<double>>(t2 - t1);
    NGLOG_VERBOSE("%p[%s] used time %f",this,getText().c_str(),dur);
}

Window::~Window() {
    NGLOG_DEBUG("%p source=%p canvas=%p",this,source,canvas);
    delete canvas;
}

void Window::onActive(){
    NGLOG_DEBUG("%p[%s]",this,getText().c_str());
}

void Window::onDeactive(){
    NGLOG_DEBUG("%p[%s]",this,getText().c_str());
}

bool Window::onKeyUp(KeyEvent& evt){
    if(INHERITED::onKeyUp(evt))
        return true;
    switch(evt.getKeyCode()){
    case KEY_ESC:
    case KEY_EXIT:
         NGLOG_DEBUG("recv %s",KeyEvent::getLabel(evt.getKeyCode()));
         if(hasActiveAnimations())return false;
         startAnimation(MAKEPOINT(getX(),getY()),MAKEPOINT(-getWidth(),getY()),
             choreograph::EaseInOutQuad(),[this](){sendMessage((DWORD)WM_DESTROY,0,0,0);});
         return true;
    } 
    return false;
}

void Window::sendMessage(View*v,DWORD msgid,DWORD wParam,ULONG lParam,DWORD delayedtime){
    if(source)source->sendMessage(v,msgid,wParam,lParam,delayedtime);
}

void Window::broadcast(DWORD msgid,DWORD wParam,ULONG lParam){
    WindowManager::getInstance().broadcast(msgid,wParam,lParam);
}

void  closeWindow(Window*w){
    w->sendMessage(w,View::WM_DESTROY,0,0);
}

}  // namespace ui
