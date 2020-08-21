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

#ifndef __TEXTVIEW_H__
#define __TEXTVIEW_H__

#include "widget.h"

namespace nglui {

class TextView : public Widget {
protected:
  RefPtr<ImageSurface>images[4];
  bool multiline;
public:
  static constexpr const char* VIEW_NAME = "TextField";
public:
  explicit TextView(const std::string& text, int width=0, int height=0);
  virtual ~TextView();
  virtual void setImage(int dir,const std::string&resname);
  virtual void onDraw(GraphContext& canvas) override;
  virtual const SIZE& getPreferSize() override;
  void setMultiLine(bool);
 private:
  typedef Widget INHERITED;
  DISALLOW_COPY_AND_ASSIGN(TextView);
};

}  // namespace ui

#endif  //__TEXTVIEW_H__
