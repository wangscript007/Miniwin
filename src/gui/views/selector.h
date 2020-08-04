#ifndef __NGLUI_SELECTOR_H__
#define __NGLUI_SELECTOR_H__
#include <abslistview.h>
namespace nglui{

class Selector:public AbsListView{
private:
   class Window* createPopupWindow();
   DECLARE_UIEVENT(Window*,CreatePopupListener,int w,int h);
protected:
   int labelBkColor;
   int label_width_;
   int show_arrows_;
   RECT popupRect;
   CreatePopupListener onpopwin;
   void getItemSize(int*w,int*h)const;
public:
    enum{
        SHOW_NEVER=0,
        SHOW_ALWAYS=1,
        SHOW_ONFOCUSED=2
    }ARROW;
    Selector(int w,int h);
    Selector(const std::string&txt,int w,int h);
    void setLabelWidth(int w);
    void setLabelColor(int color);
    int getLabelWidth();
    virtual void setPopupListener(CreatePopupListener ls);
    virtual void showArrows(int mode);
    virtual void setIndex(int idx)override;
    virtual void setPopupRect(int x,int y,int w,int h);
    virtual void setPopupRect(const RECT&rect);
    virtual void onDraw(GraphContext&canvas)override;
    virtual bool onKeyUp(KeyEvent&)override;
    typedef AbsListView INHERITED;
};
}//namespace
#endif
