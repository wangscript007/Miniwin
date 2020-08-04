#include <widget.h>

namespace nglui{

Widget::Widget(int w,int h)
  : Widget(std::string(),w,h){
}

Widget::Widget(const std::string& text)
  : Widget(text,0,0) {
}

Widget::Widget(const std::string& txt, int width, int height)
  : INHERITED(width,height)//,text(txt)
{
   GraphContext*canvas=GraphDevice::getInstance().getPrimaryContext();
   TextExtents te;
   setText(txt);
   canvas->set_font_size(getFontSize());
   canvas->get_text_extents(mText,te);
   if(width<=0)
      width=te.width+getFontSize();
   if(height<=0)
      height=te.height+getFontSize();
   setBound(0,0,width,height);
   mTextAlignment=DT_LEFT|DT_VCENTER;
}

Widget::Widget(const std::string& text, RECT& bound)
  : Widget(text) {
  setBound(bound);
}

Widget::~Widget() {
}

void Widget::setAlignment(int align){
    mTextAlignment=align;
    invalidate(nullptr);
}

int Widget::getAlignment(){
    return mTextAlignment;
}

}
