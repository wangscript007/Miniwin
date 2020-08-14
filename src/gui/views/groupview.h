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

#ifndef __NGLUI_GROUPVIEW_H__
#define __NGLUI_GROUPVIEW_H__

#include "view.h"
#include <chrono>
#include <choreograph/Choreograph.h>

namespace nglui {

#define ATTR_ANIMATE_FOCUS (0x2000) /*flag to open animate focus*/

class GroupView : public View {
 public:
  static constexpr const char* VIEW_NAME = "GroupView";
  DECLARE_UIEVENT(void,OnAnimationFinished);  
 public:
  GroupView(int w,int h);
  GroupView(int x,int y,int w,int h);
  virtual ~GroupView();
  virtual void onDraw(GraphContext& canvas) override;
  virtual void onResize(UINT cx,UINT cy) override;
  virtual bool dispatchTouchEvent(MotionEvent& event)override;
  virtual bool onInterceptTouchEvent(MotionEvent& evt);
  virtual bool onTouchEvent(MotionEvent& evt) override;
  virtual bool onKeyDown(KeyEvent& evt) override;
  virtual bool onKeyUp(KeyEvent& evt) override;
  virtual const SIZE&getPreferSize()override;
  virtual void onDrawFocusRect(GraphContext&,const RECT&);
  virtual bool onMessage(DWORD msgid,DWORD wParam,ULONG lParam)override;
  virtual void startAnimation(const POINT&ftom,const POINT&to,const choreograph::EaseFn &ease_fn = &choreograph::easeNone,OnAnimationFinished fn=nullptr);
protected:
  choreograph::Timeline timeline;
  choreograph::Output<float>focus_target;
  choreograph::Output<float>view_target;
  std::chrono::steady_clock::time_point time_lastframe;
  View*getFocused();
  bool hasActiveAnimations();
  virtual View* getNextFocus(View*cv,int key);
  void transformPointToViewLocal(View&child,POINT&point);
  bool isTransformedTouchPointInView(int x,int y,View& child,Point*outLocalPoint);
private:
  RECT focusRectSrc;
  RECT focusRectDest;
  RECT focusRect;
  POINT animateTo;//save window boundray  while animating
  POINT animateFrom;//window animate from boundary
  void moveFocusTo(const RECT&r);
  void invalidateChildrenInFocusRect();
  typedef View INHERITED;
  DISALLOW_COPY_AND_ASSIGN(GroupView);
};

}  // namespace ui

#endif  // __NGLUI_GROUPVIEW_H__
