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

#ifndef __NGLUI_WINDOW_H__
#define __NGLUI_WINDOW_H__
#include <ngl_types.h>
#include <groupview.h>

namespace nglui {
class Window : public GroupView {
  friend class WindowManager;
public:
  typedef enum{
     TYPE_WALLPAPER    =1,
     TYPE_APPLICATION  =2,
     TYPE_SYSTEM_WINDOW=2000,
     TYPE_STATUS_BAR   =2001,
     TYPE_SEARCH_BAR   =2002,
     TYPE_SYSTEM_ALERT =2003,
     TYPE_KEYGUARD     =2004,
     TYPE_TOAST        =2005,
  }WindowType;

  Window(int x,int y,int w,int h,int type=TYPE_APPLICATION);
  virtual ~Window();
  virtual void show();
  virtual void hide();
  virtual GraphContext*getCanvas()override;
  virtual View& setPos(int x,int y)override;
  virtual View& setSize(int cx,int cy)override;
  virtual bool onKeyUp(KeyEvent& evt) override;
  virtual void onActive();
  virtual void onDeactive();
  virtual void sendMessage(DWORD msgid,DWORD wParam,ULONG lParam,DWORD delayedtime=0){
     sendMessage(this,msgid,wParam,lParam,delayedtime);
  }
  virtual void sendMessage(View*v,DWORD msgid,DWORD wParam,ULONG lParam,DWORD delayedtime=0)override;
  static void broadcast(DWORD msgid,DWORD wParam,ULONG lParam);
protected:
  GraphContext*canvas;
  int window_type;/*window type*/
  int mLayer;/*surface layer*/
  class UIEventSource*source;
  virtual void draw()override;
  typedef GroupView INHERITED;
  DISALLOW_COPY_AND_ASSIGN(Window);
};
void closeWindow(Window*w);

}  // namespace nglui

#endif  // UI_LIBUI_WINDOW_H_
