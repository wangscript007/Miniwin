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

#ifndef __NGLUI_WINDOWMANAGER_H__
#define __NGLUI_WINDOWMANAGER_H__

#include <view.h>
#include <window.h>
#include <vector>
#include <stdint.h>
#include <queue>
#include <unordered_set>

namespace nglui {

class WindowManager {
public:
  DECLARE_UIEVENT(bool,WNDENUMPROC,Window*);
  virtual ~WindowManager();
  static WindowManager& getInstance();
  void addWindow(Window*w);
  void removeWindow(Window*w);
  void processEvent(InputEvent&e);
  void clip(Window*win);
  void resetVisibleRegion();
  void broadcast(DWORD msgid,DWORD wParam,ULONG lParam);
  int enumWindows(WNDENUMPROC cbk);
  RefPtr<Region>getVisibleRegion(int mLayer); 
protected:
  virtual void onKeyEvent(KeyEvent&key);
  virtual void onBtnPress(MotionEvent&event);
  virtual void onBtnRelease(MotionEvent&event);
  virtual void onMotion(MotionEvent&event);
private:
  WindowManager();
  Window*activeWindow;/*activeWindow*/
  std::vector< Window* > windows_;
  static WindowManager* instance_;
  DISALLOW_COPY_AND_ASSIGN(WindowManager);
};

}  // namespace ui

#endif  // __NGLUI_WINDOWMANAGER_H__
