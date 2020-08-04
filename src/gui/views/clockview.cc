#include <clockview.h>
#include <chrono>
using namespace std::chrono;
namespace nglui{


static void DefaultClockPainter(View&v,GraphContext&canvas,int part){
   double r=.5*v.getWidth();
   switch(part){
   case ClockView::BOARD:
       {
         canvas.set_source_rgba(.3,.3,.3,.3);;
         canvas.arc(r,r,r,0,M_PI*2);
         canvas.fill();
         canvas.set_source_rgb(1,1,1);;
         canvas.arc(r,r,r*.95,0,M_PI*2);
         canvas.set_line_width(r*.055);
         canvas.stroke();
         canvas.translate(r,r);
         for(int i=0;i<12;i++){
            double lw=r*.05;
            canvas.rotate_degrees(30);
            canvas.move_to(r*.88,0);
            canvas.line_to(r*.95,0);
         }
         canvas.stroke();
       }break;
   case ClockView::HOUR:
   case ClockView::MINUTE:
   case ClockView::SECOND:
       canvas.set_source_rgb(1,.2*part,.1*part);
       canvas.move_to(-3,0);
       canvas.line_to(r*(.5+.1*part),0);
       canvas.stroke();
   } 
}

ClockView::ClockView(int w,int h):View(w,h){
    painter=DefaultClockPainter;
    setBgColor(0);
}

void ClockView::onAttached(){
    sendMessage(WM_TIMER,0,0,1000);
}

void ClockView::onDraw(GraphContext&canvas){
    View::onDraw(canvas);
    int cx=getWidth()/2;
    int cy=getHeight()/2;
    auto tt =system_clock::to_time_t(system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60];
    RECT rect=getClientRect();
    sprintf(date, "%02d:%02d:%02d",(int)ptm->tm_hour,(int)ptm->tm_min,(int)ptm->tm_sec);

    canvas.save();
    painter(*this,canvas,BOARD);
    canvas.restore();
    
    canvas.translate(cx,cy);

    canvas.save();

    canvas.rotate_degrees((ptm->tm_hour%12)*30. + .5*ptm->tm_min-90.); 
    painter(*this,canvas,HOUR);
    canvas.restore();

    canvas.save();
    canvas.rotate_degrees(6.*ptm->tm_min+.1*ptm->tm_sec-90); 
    painter(*this,canvas,MINUTE);
    canvas.restore();

    canvas.save();
    canvas.rotate_degrees(6.*ptm->tm_sec-90); 
    painter(*this,canvas,SECOND);
    canvas.restore();
}

void ClockView::setClockPainter(ClockPainter pnt){
    painter=pnt;
}

bool ClockView::onMessage(DWORD msg,DWORD wp,ULONG lp){
     if(msg==WM_TIMER){
         sendMessage(msg,wp,lp,1000);
         invalidate(nullptr);
         return true;
     }
     return onMessage(msg,wp,lp);
}


}
