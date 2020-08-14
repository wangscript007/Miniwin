#include <rect.h>
namespace nglui{
void Point::set(int x_,int y_){
    x=x_;
    y=y_;
}

void Rectangle::set(int x_,int y_,int w,int h){
   x=x_;y=y_;width=w;height=h;
}

bool Rectangle::empty()const{
    return width<=0||height<=0;
}

void Rectangle::offset(int dx,int dy){
    x+=dx;
    y+=dy;
}

void Rectangle::inflate(int dx,int dy){
   x-=dx;y-=dy;
   width+=(dx+dx);
   height+=(dy+dy);
}

bool Rectangle::intersect(const Rectangle&b){
    return intersect(*this,b);
}

bool Rectangle::intersect(int x_,int y_){
    return x_>=x&&x_<x+width && y_>=y&&y_<y+height;
}

bool Rectangle::intersect(const Rectangle&a,const Rectangle&b){
    if(a.empty()||b.empty()){
        set(0,0,0,0);
        return false;
    }
    //check if the 2 Rectangle intersect
    if( a.x + a.width <= b.x || b.x + b.width <= a.x || a.y + a.height <= b.y || b.y + b.height <= a.y ){
          // No intersection
          set(0,0,0,0);
          return false ;//Rectangle::emptyRectangle;
    }

    //calculate the coordinates of the intersection
    int i_x = a.x > b.x ? a.x : b.x;
    int i_y = a.y > b.y ? a.y : b.y;

    int thisWBorder  = a.x + a.width;
    int otherWBorder = b.x + b.width;
    int thisHBorder  = a.y + a.height;
    int otherHBorder = b.y + b.height;

    int i_w = thisWBorder > otherWBorder ? otherWBorder - i_x : thisWBorder - i_x;
    int i_h = thisHBorder > otherHBorder ? otherHBorder - i_y : thisHBorder - i_y;
    set(i_x,i_y,i_w,i_h);
    return true;
}
}//end namespace
