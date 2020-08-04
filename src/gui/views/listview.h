#ifndef __NGLUI_LISTVIEW_H__
#define __NGLUI_LISTVIEW_H__
#include <abslistview.h>

namespace nglui{

class ListView:public AbsListView{
private:
   POINT point_touch;
protected:
   void getItemSize(int *w,int*h)const override;
public:
   ListView(int w,int h);
   virtual void onDraw(GraphContext&canvas)override;
   virtual bool onKeyUp(KeyEvent&k)override;
   virtual void setIndex(int idx)override;
   virtual bool onTouchEvent(MotionEvent&event)override;
   void getFocusRect(RECT&r)const override;
typedef AbsListView INHERITED;
};

}//namespace
#endif
