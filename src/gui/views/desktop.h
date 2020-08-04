#ifndef __DESKTOP_H__
#define __DESKTOP_H__
#include<window.h>
namespace nglui{

DECLARE_UIEVENT(bool,KeyHandler,int);
class Desktop:public Window{
private:
   KeyHandler onkeypress;
   class ToolBar*statusbar_;
public:
   Desktop();
   ~Desktop();
   virtual GraphContext*getCanvas()override;
   virtual void setKeyListener(KeyHandler fun);
protected:
   virtual bool onKeyUp(KeyEvent&k)override;
DISALLOW_COPY_AND_ASSIGN(Desktop);
};

}//namespace
#endif
