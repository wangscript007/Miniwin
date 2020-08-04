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

#ifndef __NGLUI_STACKLAYOUT_H__
#define __NGLUI_STACKLAYOUT_H__

#include "layout.h"
#include "view.h"

namespace nglui {

class StackLayout : public Layout {
 public:
  static constexpr int32_t DELIMITER = 5;

  StackLayout();
  virtual ~StackLayout();

  virtual void onLayout(View* view) override;

 private:
  typedef Layout INHERITED;
  DISALLOW_COPY_AND_ASSIGN(StackLayout);
};

}  // namespace ui

#endif  // UI_LIBUI_STACKLAYOUT_H_
