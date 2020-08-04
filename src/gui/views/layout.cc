#include <layout.h>

namespace nglui{

Layout::Layout() {
   gravity_=GRAVITY::LEFT;
   setMargins(0);
   setPaddings(0);
}

Layout::~Layout() {
}

void Layout::setMargins(int v) {
    for(int i=0;i<4;i++)margins_[i]=v;
}

void Layout::setMargin(int tb,int lr){
    margins_[0]=margins_[1]=tb;
    margins_[2]=margins_[3]=lr;   
}

void Layout::setMargin(int top,int bottom,int left,int right){
    margins_[0]=top;
    margins_[1]=bottom;
    margins_[2]=left;
    margins_[3]=right;
}

void Layout::setPaddings(int v){
    for(int i=0;i<4;i++)paddings_[i]=v;
}

void Layout::setPaddings(int tb,int lr){
    paddings_[0]=paddings_[1]=tb;
    paddings_[2]=paddings_[3]=lr;
}

void Layout::setPaddings(int top,int bottom,int left,int right){
    paddings_[0]=top;
    paddings_[1]=bottom;
    paddings_[2]=left;
    paddings_[3]=right;
}

}//endof namespace
