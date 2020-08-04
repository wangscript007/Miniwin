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

#include "stacklayout.h"

namespace nglui {

StackLayout::StackLayout()
  : INHERITED() {;
}

StackLayout::~StackLayout() {
}

void StackLayout::onLayout(View* view) {
    int y = getPaddingTop();
    int x = getPaddingLeft();

    for (int i=0;i<view->getChildrenCount();i++){
         View* child=view->getChildView(i);
        SIZE size = child->getPreferSize();
        child->setPos(x, y);
        y += (size.height() + getMarginTop());
    }
}

}  // namespace nglui
