#include <progressbar.h>
#include <ngl_log.h>

NGL_MODULE(ProgressBar);

namespace nglui{

ProgressBar::ProgressBar(int width, int height):Widget(width,height){
    min_=0;
    max_=100;
    progress_=0;
    indeterminate_=FALSE;
    indeterminatePos=0;
}

void ProgressBar::setMin(int value){
    if(min_!=value){
        min_=value;
        invalidate(nullptr);
    }
}

void ProgressBar::setMax(int value){
    if(max_!=value){
        max_=value;
        invalidate(nullptr);
    }
}

void ProgressBar::setRange(int vmin,int vmax){
    if( (min_!=vmin)||(max_!=vmax)){
        min_=vmin;
        max_=vmax;
        invalidate(nullptr);
    }
}

bool ProgressBar::setProgressInternal(int value, bool fromUser){
    if(indeterminate_)return false;
    if(value<min_)value=min_;
    if(value>max_)value=max_;
    if(progress_!=value){
        progress_=value;
        onProgressRefresh(1.,fromUser,value);
        invalidate(nullptr);
        return true;
    }
    return false;
}

void ProgressBar::setProgress(int value){
    setProgressInternal(value,FALSE);
}

int ProgressBar::getProgress(){
    return progress_;
}

void ProgressBar::setIndeterminate(bool indeterminate){
    indeterminate_=indeterminate;
    if(indeterminate){
       sendMessage(WM_TIMER,0,0,200);
    }
}

bool ProgressBar::onMessage(DWORD msg,DWORD wp,DWORD lp){
    if( (msg==WM_TIMER) && indeterminate_ ){
        indeterminatePos+=(max_-min_)/20;
        invalidate(nullptr);
        sendMessage(WM_TIMER,0,0,200);
    }
    return Widget::onMessage(msg,wp,lp); 
}

void ProgressBar::onDraw(GraphContext&canvas){
    RECT rect=getClientRect();
    Widget::onDraw(canvas);
    canvas.set_color(getBgColor());
    canvas.rectangle(rect);
    canvas.fill();
    
    if(rect.width!=rect.height){
        canvas.set_color(getFgColor());
        canvas.rectangle(rect);
        canvas.stroke();
    }

    RECT r=rect;
    r.inflate(-1,-1);
    canvas.set_font_size(getFontSize());
#define ANGLE(x) (M_PI*2.0*(double)(x)/(double)(max_-min_))
    if(!indeterminate_){
       if(getWidth()>getHeight()){
            r.width=r.width*progress_/(max_-min_);
       }else{
            int h=r.height;
            r.height=r.height*progress_/(max_-min_);
            r.offset(0,h-r.height);
       }
       if(getWidth()!=getHeight()){
           canvas.set_source_rgb(.0,.0,1.);
           canvas.rectangle(r);
           canvas.fill(); 
       }else{
           char str[32];
           sprintf(str,"%d%%",100*progress_/(max_-min_));
           double pc=(double)progress_/(max_-min_);
           canvas.save();
           canvas.set_line_width((double)r.width/15);
           RefPtr<Gradient>pat=LinearGradient::create(0,0,getWidth(),getHeight());
           pat->add_color_stop_rgba(.0,.2,.0,.2,.2);
           pat->add_color_stop_rgba(pc,1.,.0,.0,1.);
           pat->add_color_stop_rgba(1.,.2,.0,.2,.2);
           canvas.set_source(pat);//_rgb(0,1.,0);
           canvas.arc(r.width/2,r.width/2,r.width/2,0,ANGLE(progress_));
           canvas.stroke();
           canvas.restore();

           r=getClientRect();   
           canvas.set_font_size(r.width/4);
           canvas.set_source_rgb(.0,.0,1.);
           canvas.draw_text(r,str,DT_CENTER|DT_VCENTER);
       }
       return;
    }
    //interminate progress drawing 
    double cp=(double)(indeterminatePos%(max_-min_))/(max_-min_);
    if(getWidth()!=getHeight()){
        RefPtr<Gradient>pat=LinearGradient::create(0,0,getWidth(),getHeight());
        pat->add_color_stop_rgba(cp-0.25,.2,.2,.2,.2);
        pat->add_color_stop_rgba(cp,1.,.0,.0,1.);
        pat->add_color_stop_rgba(cp+0.25,.2,.2,.2,.2);
        canvas.set_source(pat);
        canvas.rectangle(0,0,getWidth(),getHeight());
        canvas.fill();
    }else{
        double radius=getWidth()/2;
        canvas.save();
        RefPtr<Gradient>pat=LinearGradient::create(0,0,getWidth(),getHeight());
        pat->add_color_stop_rgba(cp-.25,.2,.0,.2,.2);
        pat->add_color_stop_rgba(cp,1.,.0,.0,1.);
        pat->add_color_stop_rgba(cp+.25,.0,.2,.2,.2);
        canvas.set_source(pat);
        canvas.set_line_width((double)r.width/15);
        canvas.arc(r.width/2,r.width/2,r.width/2,0,M_PI*2);
        canvas.stroke();
        canvas.restore();
    }
}

}//end namespace
