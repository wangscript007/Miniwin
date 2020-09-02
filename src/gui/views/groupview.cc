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

#include <groupview.h>
#include <ngl_log.h>
#include <uievents.h>

NGL_MODULE(GroupView);

using namespace std::chrono;
using namespace choreograph;

namespace nglui {

GroupView::GroupView(int w,int h)
  : GroupView(0,0,w,h) {
   //internal_name_ = VIEW_NAME;
}

GroupView::GroupView(int x,int y,int w,int h)
:INHERITED(w,h){
   setBound(x,y,w,h);
   setFlag(ENABLED);
   focusRectSrc.set(0,0,0,0);
   focusRectDest.set(0,0,0,0);
   focusRect.set(0,0,0,0);

   PhraseRef<float> mv=makeRamp(.0f,1.f,.5f,EaseInOutQuad());
   timeline.apply<float>(&focus_target,mv).updateFn([this](){
        focusRect.x=focusRectSrc.x+(focusRectDest.x-focusRectSrc.x)*focus_target.value();
        focusRect.y=focusRectSrc.y+(focusRectDest.y-focusRectSrc.y)*focus_target.value();
        focusRect.width=focusRectSrc.width+(focusRectDest.width-focusRectSrc.width)*focus_target.value();
        focusRect.height=focusRectSrc.height+(focusRectDest.height-focusRectSrc.height)*focus_target.value();
   });
   (*timeline.begin())->setRemoveOnFinish(false);
}
GroupView::~GroupView() {
}

const SIZE& GroupView::getPreferSize(){
    return mPreferSize;
}

void GroupView::transformPointToViewLocal(View&child,POINT&point) {
     //point.x += mScrollX - child.getX();
     //point.y += mScrollY - child.getY();
     /*if (!child.hasIdentityMatrix()) {
         child.getInverseMatrix().mapPoints(point);
     }*/
}

bool GroupView::isTransformedTouchPointInView(int x,int y, View& child,POINT*outLocalPoint) {
    POINT point ={x,y};
    transformPointToViewLocal(child,point);
    bool isInView = child.pointInView(point.x,point.y);
    if (isInView && outLocalPoint != nullptr) {
        outLocalPoint->set(point.x, point.y);
    }
    return isInView;
}

void GroupView::onDraw(GraphContext& canvas) {
    // Draw the background color, if enabled
    INHERITED::onDraw(canvas);
    canvas.save();
    onDrawFocusRect(canvas,focusRect);
    canvas.restore();
}

void GroupView::onDrawFocusRect(GraphContext&canvas,const RECT&r){
    canvas.set_source_rgba(1,0,0,.5);
    canvas.rectangle(r.x,r.y,r.width,r.height);
    canvas.fill();
}

void GroupView::onResize(UINT cx,UINT cy) {
    // Notify all children to do resizing
    if (mLayout != nullptr) {
        mLayout->onLayout(this);
    }
}

bool GroupView::onInterceptTouchEvent(MotionEvent& evt){
    return false; 
}

bool GroupView::dispatchTouchEvent(MotionEvent&event){
    int x=event.getX();
    int y=event.getY();
    if(onInterceptTouchEvent(event))
       return onTouchEvent(event);
    for(auto child : mChildren){
       if(child->pointInView(x,y)){
          event.offsetLocation(-child->getX(),-child->getY()); 
          return child->dispatchTouchEvent(event);
       }
    }
    return false; 
}

bool GroupView::onTouchEvent(MotionEvent& evt) {
    // Iterate each child to find out which one to distribute this event
    for (auto child : mChildren) {
       // Check if it's enabled
       if (child->isEnable() == false) {
           continue;
       }
       RECT rb=child->getBound();
       // Check if it's intersected, if not, just ignore this view
       if (rb.intersect(evt.getX(), evt.getY()) == false) {
            continue;
       }
       if (child->hasFlag(TRANSPARENT) == true) {
           // If it's a LAYOUT view, distribute this event to it
           // And let it do the rest
           child->onTouchEvent(evt);
           continue;
       }
       if (child->onTouchEvent(evt) == true) {
           // If it's not a LAYOUT view, but a WIDGET one.
           // Try to call its handler to see whether it will be handled.
           // If so, terminate this process.
           // If not, continue to check out next WIDGET view.
           return true;
       }
  }
  return false;
}

bool GroupView::onKeyDown(KeyEvent& evt) {
    // Iterate each child to find out which one to distribute this event
    for (auto child : mChildren) {
        // Check if it's enabled
        if (child->isEnable() == false) {
            continue;
        }
        if (child->hasFlag(TRANSPARENT) == true) {
            // If it's a LAYOUT view, distribute this event to it
            // And let it do the rest
            child->onKeyDown(evt);
            continue;
        }
        if (child->onKeyDown(evt) == true) {
            // If it's not a LAYOUT view, but a WIDGET one.
            // Try to call its handler to see whether it will be handled.
            // If so, terminate this process.
            // If not, continue to check out next WIDGET view.
            return true;
        }
    }
    return false;
}

View*GroupView::getFocused(){
    View*first=nullptr;//first focusable view
    for(auto it=mChildren.begin();it!=mChildren.end();it++){
       if(nullptr==first&&(*it)->hasFlag(FOCUSABLE))
           first=(*it);
       if((*it)->hasFlag(FOCUSED)){
           return (*it);
       }
    }
    return first;
}

View* GroupView::getNextFocus(View*cv,int key){
    int idx=getViewOrder(cv);
    int dir=0,odx=idx;
    RECT rect;
    switch(key){
    case KEY_LEFT:
    case KEY_UP:dir=-1;break;
    case KEY_RIGHT:
    case KEY_DOWN:dir=1;break;
    default:dir=1;break;
    }
    for(int i=0;i<getChildrenCount();i++){
        idx=(idx+dir+getChildrenCount())%getChildrenCount();
        View*v=getChildView(idx);
        if(v->hasFlag(FOCUSABLE)&&v->isEnable()){
             if(cv){
                 cv->clearFlag(FOCUSED);
                 cv->invalidate(nullptr);
             }
             v->setFlag(FOCUSED);
             v->getFocusRect(rect);
             moveFocusTo(rect);
             NGLOG_VERBOSE("changefocuse from %p to %p %d->%d",cv,v,odx,idx);
             return v;
        }
    }
    return nullptr;
}

choreograph::Output<float>ow;
void GroupView::moveFocusTo(const RECT&r){
    focus_target=.0f;
    focusRectSrc=focusRectDest;
    focusRectDest=r;
    (*timeline.begin())->resetTime();
    time_lastframe=std::chrono::steady_clock::now();

    if(hasFlag(ATTR_ANIMATE_FOCUS))
       sendMessage(WM_TIMER,0,0,10);
}

void GroupView::invalidateChildrenInFocusRect(){
    for(int i=0;i<getChildrenCount();i++){
        View*v=getChildView(i);
        RECT r;
        if(!r.intersect(v->getBound(),focusRect))continue;
        r.offset(-v->getX(),-v->getY());
        v->invalidate(&r);
    }
}

static int isExcludedKeys(int key){
   return key==KEY_MENU||key==KEY_ESC||key==KEY_EXIT;
}

bool GroupView::onKeyUp(KeyEvent& evt) {
    // Iterate each child to find out which one to distribute this event
    if(isExcludedKeys(evt.getKeyCode()))return false;
    View*cfv=getFocused();
    if(cfv==nullptr)return false;
    if(!cfv->hasFlag(FOCUSED)){
       NGLOG_VERBOSE("focus not setted ,autofocus %p",cfv);
       cfv->setFlag(FOCUSED);
    }
    if( cfv->onKeyUp(evt)==false ){
        View*nfv=getNextFocus(cfv,evt.getKeyCode());
        NGLOG_VERBOSE("currentfocus=%p newfocus=%p",cfv,nfv);
        if(nfv)nfv->invalidate(nullptr);
    }
    return false;
}

bool GroupView::hasActiveAnimations(){
    return (timeline.size()>1)&&(timeline[1]&&timeline[1]->time()<timeline.getDuration());
}

bool GroupView::onMessage(DWORD msgid,DWORD wParam,ULONG lParam){
    RECT rect;
    bool needmoretimer=0;
    switch(msgid){
    case WM_TIMER:
        switch(timeline.size()){
        case 2:
            if(timeline[1]->time()<timeline.getDuration()){//2nd is uded for window animation
                duration<double>dur=duration_cast<duration<double>>(steady_clock::now() - time_lastframe);
                timeline.step(dur.count());
                needmoretimer++; 
            }//pass throught
        case 1:
            if( hasFlag(ATTR_ANIMATE_FOCUS) && (timeline[0]->time()<timeline.getDuration()) ){//1st is used for focus animation
                duration<double>dur=duration_cast<duration<double>>(steady_clock::now() - time_lastframe);
                invalidate(&focusRect);
                invalidateChildrenInFocusRect();
                timeline.step(dur.count());
                invalidate(&focusRect);
                invalidateChildrenInFocusRect();
                time_lastframe=steady_clock::now();
                needmoretimer++;
            }
            if(needmoretimer)sendMessage(msgid,wParam,lParam,20);
            break;
        default:break;
        }
        return true;
    case WM_FOCUSRECT:
        rect.x=wParam>>16;
        rect.y=wParam&0xFFFF;
        rect.width=lParam>>16;
        rect.height=lParam&0xFFFF;
        moveFocusTo(rect);
        return true;
    }
    return View::onMessage(msgid,wParam,lParam);
}

void GroupView::startAnimation(const POINT&from,const POINT&to,const choreograph::EaseFn &ease_fn,OnAnimationFinished fn){
    view_target=.0f;
    animateFrom=from;
    animateTo=to;
    time_lastframe=steady_clock::now();
    PhraseRef<float> mv=makeRamp(.0f,1.f,2.f,ease_fn);//EaseInOutQuad());
    timeline.apply<float>(&view_target,mv).updateFn([this](){
        int wx=animateFrom.x+(animateTo.x-animateFrom.x)*view_target.value();
        int wy=animateFrom.y+(animateTo.y-animateFrom.y)*view_target.value();
        setPos(wx,wy);
    }).finishFn(fn);
    sendMessage(WM_TIMER,0,0,0);
}

}  // namespace ui
