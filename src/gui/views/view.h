#ifndef __NGL_VIEW_H__
#define __NGL_VIEW_H__
#include <eventcodes.h>
#include <uievents.h>
#include <graph_context.h>
#include <layout.h>
#include <memory>
#include <vector>
#include <functional>
#include <rect.h>

#ifndef _GLIBCXX_FUNCTIONAL
#define DECLARE_UIEVENT(type,name,...) typedef type(*name)(__VA_ARGS__)
#else
#define DECLARE_UIEVENT(type,name,...) typedef std::function< type(__VA_ARGS__) >name
#endif

namespace nglui{

#define SIF_ALL             0xFF//:整个结构都有效
#define SIF_DISABLENOSCROLL 0x01//:该值仅在设定参数时使用，视控件参数设定的需要来对本结构的成员进行取舍。
#define SIF_PAGE            0x02//:nPage成员有效
#define SIF_POS             0x04//:nPos成员有效
#define SIF_RANGE           0x08//:nMin和nMax成员有效
#define SIF_TRACKPOS        0x10

#define SB_VERT 0
#define SB_HORZ 1

typedef struct tagSCROLLINFO {  // si 
    UINT cbSize; 
    UINT fMask; 
    int  nMin; 
    int  nMax; 
    UINT nPage; 
    int  nPos; 
    int  nTrackPos; 
}SCROLLINFO;

/*typedef struct tagSCROLLBARINFO {
  RECT  rcScrollBar;
  int   dxyLineButton;
  int   xyThumbTop;
  int   xyThumbBottom;
  int   reserved;
  DWORD rgstate[CCHILDREN_SCROLLBAR + 1];
} SCROLLBARINFO;*/

class View{
public:
  enum{
      ENABLED =0x00,
      DISABLED=0x01,
      ENABLED_MASK=0x1,
      VISIBLE =0x02,
      FOCUSABLE=0x04,
      BORDER   =0x08,
      FOCUSED  =0x10,
      SCROLLBARS_NONE=0,
      SCROLLBARS_HORIZONTAL=0x100,
      SCROLLBARS_VERTICAL=0x200,
      SCROLLBARS_MASK =0x300,
      CLIPCHILDREN=0x400,
      TRANSPARENT =0x800,
      CLICKABLE=0x1000,
      LONG_CLICKABLE=0x2000,
      TOOLTIP=0x4000,
  };
  enum {
    WM_CREATE    =0,
    WM_DESTROY   =1,//no param
    WM_INVALIDATE=2,
    WM_TIMER     =3,//wParam it timerid lParam unused
    WM_CLICK     =4,//wParam is view's id
    WM_CHAR      =5,//wparam is unicode char
    WM_FOCUSRECT =6,//move focus rect wParam(x/y)lParam(width,height)
    WM_USER      =0x1000,/*the 1st user defined MESSAGE*/
  };

DECLARE_UIEVENT(void,ClickListener,View&);
DECLARE_UIEVENT(bool,MessageListener,View&,DWORD,DWORD,ULONG);
protected:
    int mId;
    int mFontSize;
    int mFgColor;
    int mScrollX;
    int mScrollY;
    RefPtr<const Pattern>mBgPattern;
    int mViewFlags;
    int mPrivateFlags;
    View*mParent;
    std::string mText;
    RECT mBound;
    SIZE mPreferSize;
    SCROLLINFO mScrollInfos[2];
    RefPtr<Region>mInvalid;
    RefPtr<Region>vis_rgn;//visible region
    std::vector< View* > mChildren;
    std::unique_ptr<class Layout> mLayout;
    ClickListener onClick_;
    MessageListener onMessage_;
    virtual void onAttached(){};
    virtual GraphContext*getCanvas();
    virtual void  onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    virtual void drawChildren();
    virtual void onFocusChanged(bool);
public:
    View(int w,int h);
    virtual ~View();
    virtual void draw();
    virtual void onDraw(GraphContext&ctx);
    virtual void invalidate(const RECT*);
    virtual void invalidate(const RefPtr<Region>crgn);
    const RECT getBound()const;
    virtual void getFocusRect(RECT&r)const;
    virtual View& setBound(const RECT&);
    virtual View& setBound(int x,int y,int w,int h);
    virtual View& setPos(int x,int y);
    virtual View& setSize(int x,int y);
    int getX()const;//x pos to screen
    int getY()const;//y pos to screen
    int getWidth()const;
    int getHeight()const;
    virtual void setText(const std::string&txt);
    virtual const std::string& getText()const;
    bool pointInView(int x,int y)const;
    void clip(GraphContext&canvas);
    void resetClip();
    const RECT getClientRect()const;
    virtual void setClickListener(ClickListener ls);
    virtual void setMessageListener(MessageListener ls);
  // Foreground color
    virtual View& setBgPattern(const RefPtr<const Pattern>& source);
    virtual RefPtr<const Pattern>getBgPattern();
    virtual View& setFgColor(UINT color);
    virtual UINT getFgColor() const;

  // Background color
    virtual View& setBgColor(UINT color);
    virtual UINT getBgColor() const;

    virtual const SIZE&getPreferSize(); 
    View& setId(int id);
    int getId()const;
    void setScrollInfo(int bar,const SCROLLINFO*info,bool redraw=true);
    virtual void scrollTo(int x,int y);
    void scrollBy(int dx,int dy);
    void setScrollX(int x);
    void setScrollY(int y);
    int getScrollX()const;
    int getScrollY()const;
    virtual void drawScrollBar(GraphContext&canvas,int bar);
      // Font size
    virtual int32_t getFontSize() const;
    virtual View& setFontSize(int32_t sz);

    // Layout
    virtual Layout* setLayout(Layout* layout);
    virtual Layout* getLayout() const;
    virtual void onLayout();
 
   // Attribute
    virtual void setFlag(int flag);
    virtual void clearFlag(int flag);
    virtual bool hasFlag(int flag) const;
    virtual bool isFocused()const;
    // Enable & Visible
    virtual void setVisible(bool visable);
    virtual bool isVisible() const;
    virtual void setEnable(bool enable);
    virtual bool isEnable() const;

    // Parent and children views
    virtual int getViewOrder(View*v);
    virtual View*getParent();
    virtual void setParent(View*p);
    virtual View*getChildView(size_t idx);
    virtual View*findViewById(int id);
    virtual View*addChildView(View* view);
    virtual void removeChildView(View* view);/*only remove view from children,no deleteion*/
    virtual void removeChildView(size_t idx);
    virtual void removeChildren();
    virtual size_t getChildrenCount() const;

    virtual bool dispatchTouchEvent(MotionEvent& event);

    virtual void onResize(UINT cx,UINT cy);
    virtual bool onKeyUp(KeyEvent& evt);
    virtual bool onKeyDown(KeyEvent& evt);
    virtual bool onInterceptTouchEvent(MotionEvent& evt);
    virtual bool onTouchEvent(MotionEvent& evt);
    virtual void sendMessage(DWORD msgid,DWORD wParam,ULONG lParam,DWORD delayedtime=0);
    virtual void sendMessage(View*view,DWORD msgid,DWORD wParam,ULONG lParam,DWORD delayedtime=0);
    virtual bool onMessage(DWORD msgid,DWORD wParam,ULONG lParam);
private:
    DISALLOW_COPY_AND_ASSIGN(View);
};

}//endof namespace nglui
#endif
