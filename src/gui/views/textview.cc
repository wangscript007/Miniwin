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
#include "textview.h"
#include <app.h>
#include <ngl_log.h>

NGL_MODULE(TextView);

namespace nglui {

TextView::TextView(const std::string& text, int width, int height)
  : INHERITED(text, width, height) {
    multiline=false;
}

TextView::~TextView() {
}

void TextView::setMultiLine(bool m){
     if(m)
         mTextAlignment|=DT_MULTILINE;
     else
         mTextAlignment&=(~DT_MULTILINE);
}

void TextView::setImage(int dir,const std::string&resname){
    if(resname.empty()==false)
        images[dir]=App::getInstance().loadImage(resname);
}

void TextView::onDraw(GraphContext& canvas) {
    INHERITED::onDraw(canvas);
    RECT rcimg,rect=getClientRect();
    // Border
    if(hasFlag(BORDER)){
        canvas.set_color(getFgColor());
        canvas.set_line_width(1);
        canvas.rectangle(0,0,getWidth(),getHeight());
        canvas.stroke();
    }
    if(images[2]){
        rcimg=rect;
        rcimg.width=images[2]->get_width();
        canvas.draw_image(images[2],&rcimg,nullptr,ST_CENTER_INSIDE);
        rect.x+=rcimg.width; 
        rect.width-=rcimg.width;
    }
    // Text
    canvas.set_color(mFgColor);
    canvas.set_font_size(getFontSize());
    canvas.draw_text(rect,mText.c_str(),getAlignment());
}

const SIZE& TextView::getPreferSize() {
  mPreferSize.set(getWidth(), getHeight());
  return mPreferSize;
}

}  // namespace ui
