#ifndef __NGLUI_GRIDVIEW_H__
#define __NGLUI_GRIDVIEW_H__
#include <abslistview.h>

namespace nglui{

class GridView:public AbsListView{
protected:
   void getItemSize(int *w,int*h)const override;
public:
   GridView(int w,int h);
   virtual void onDraw(GraphContext&canvas)override;
   virtual bool onKeyUp(KeyEvent&k)override;
   virtual void setIndex(int idx)override;
   void getFocusRect(RECT&r)const override;
typedef AbsListView INHERITED;
};

}//namespace
#endif
