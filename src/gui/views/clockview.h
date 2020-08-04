#ifndef __CLOCK_VIEW_H__
#define __CLOCK_VIEW_H__
#include <view.h>
namespace nglui{

class ClockView:public View{
public:
   DECLARE_UIEVENT(void,ClockPainter,View&,GraphContext&,int);
protected:
   ClockPainter painter;
   void onAttached();
public:
   enum{
       BOARD,
       HOUR,
       MINUTE,
       SECOND
   };
   ClockView(int w,int h);
   void onDraw(GraphContext&canvas)override;
   void setClockPainter(ClockPainter painter);
   bool onMessage(DWORD msg,DWORD wp,ULONG lp);
}; 

}//namespace
#endif
