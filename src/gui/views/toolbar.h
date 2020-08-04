#ifndef __NGLUI_TOOLBAR_H__
#define __NGLUI_TOOLBAR_H__
#include <abslistview.h>

namespace nglui{

class ToolBar:public AbsListView{
protected:
   void getItemSize(int *w,int*h)const override;
   int getItemAtPos(int x,int y);
   static void DefaultPainter(AbsListView&lv,const AbsListView::ListItem&itm,int state,GraphContext&canvas);
public:
   class Button:public AbsListView::ListItem{
   public:
       RefPtr<ImageSurface> image;
   public:
       Button(const std::string&txt,int v=0);
       Button(const std::string&img,const std::string&txt,int v=0);
       void setImage(const std::string&);
   };

   ToolBar(int w,int h);
   virtual void addButton(const std::string&txt,int width=-1,int id=0);
   virtual void addButton(const std::string&img,const std::string&txt,int width=-1,int id=0);
   virtual bool onTouchEvent(MotionEvent&event)override;
   virtual void onDraw(GraphContext&canvas)override;
   virtual bool onKeyUp(KeyEvent&k)override;
   virtual void setIndex(int idx)override;
   void getFocusRect(RECT&r)const override;
   typedef AbsListView INHERITED;
};

}//namespace
#endif
