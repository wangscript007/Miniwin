#include "linearlayout.h"
#include <ngl_log.h>
NGL_MODULE(LinearLayout);
namespace nglui {

LinearLayout::LinearLayout()
  : Layout() {;
}

LinearLayout::~LinearLayout() {
}

static void LayoutLine(View*parent,std::vector<View*>vs){
    int x=parent->getWidth();
    for(auto v:vs){
        v->setPos(x-v->getWidth(),v->getY());
        NGLOG_DEBUG("setx=%d y=%d", x-v->getWidth(),v->getY());
        x-=v->getWidth();
    }
}

void LinearLayout::onLayout(View* view) {
    int x = getMarginLeft();
    int y = getMarginTop();
    int w = view->getWidth();
    int h = view->getHeight();
    int e_x = x + w-getMarginBottom();
    int e_y = y + h;//endof y of current layout line
    int dx = x;
    int dy = y;
    int max_height = 0;
    std::vector<View*>crow;
    NGLOG_VERBOSE("%p x=%d y=%d",view,dx,dy);
    for (int i=0;i<view->getChildrenCount();i++){
        View*child=view->getChildView(i);
        // Get its prefer size
        SIZE size = child->getPreferSize();
        crow.push_back(child);
        // Check the required size of the view, must be smaller/equal to,
        // and then skip it
        if(child->isVisible()==false)continue;
        if (size.width() > w || size.height() > h) {
            child->setBound(0, 0, 0, 0);
            NGLOG_ERROR("Size (%dx%d)is too large(%dx%d), shrink to 0",size.width(),size.height(),w,h);
            continue;
        }

        // If not wide enough, move to next line
        if (dx + size.width() > e_x) {
            dx = x;
            dy += (max_height + getMarginTop());
            max_height=size.height();
        }

        // If not height enough, shrink it to 0 size, and then skip it
        if (dy + size.height() > e_y) {
            child->setBound(0, 0, 0, 0);
            NGLOG_DEBUG("%d,%d-%d,%d Out of Y(%d) lineheight=%d, shrink to 0",dx,dy,size.width(),size.height() ,e_y,max_height);
            continue;
        }

        // Maintain the maximum height of this line
        if (size.height() > max_height) {
            max_height = size.height();
        }

        // Set the view to a XY position
        child->setPos(dx, dy);

        NGLOG_VERBOSE("layout %p Child %p id=%d/%d pos(%d,%d-%d,%d)",this,child,child->getId(),i, dx,dy,size.x,size.y);
        // Move to next X position
        dx += (size.width() + getMarginRight());

        // If X has exceeded the boundary, then move to next line
        if (dx >= e_x) {
            dx = x;
            dy += (max_height + getMarginBottom());
            max_height = 0; 
            if(getGravity()==GRAVITY::RIGHT)
                LayoutLine(view,crow);
            crow.clear();
        }
    }
    if(getGravity()==GRAVITY::RIGHT)
        LayoutLine(view,crow);
}//  endof onLayout

}//endof namespace
