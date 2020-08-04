#include <ruleview.h>

namespace nglui{

RuleView::RuleView(int w,int h):View(w,h){
    startValue=0;
    interval=10;
    offset=0;
    endValue=w/interval;
    onGetLabel=nullptr;
}

void RuleView::setMin(int v){
    if(startValue!=v){
        startValue=v;
        endValue=v+getWidth()/interval;
        invalidate(nullptr);
    }
}

void RuleView::setMax(int v){
    if(endValue!=v){
        endValue=v;
        invalidate(nullptr);
    }
}

void RuleView::setInterval(int v){
    if(interval!=v){
        interval=v;
        invalidate(nullptr);
    }
}

void RuleView::setOffset(int v){
    if(offset!=v){
        offset=v;
        invalidate(nullptr);
    }
}

void RuleView::setLabeListener(GetLabeListener listener){
    onGetLabel=listener;
}

void RuleView::onDraw(GraphContext&canvas){
    View::onDraw(canvas);
    int x,i;

    canvas.set_color(getFgColor());
    if(offset>0){
        RECT rect=getClientRect();
        rect.width=offset;
        canvas.set_font_size(std::min(getWidth(),getHeight())*4/5);
        canvas.draw_text(rect,getText(),DT_CENTER|DT_VCENTER);
    }

    for(x=offset,i=startValue;x<getWidth()&&i<=endValue;x+=interval,i++){
        switch((i-startValue)%10){
        case 0:canvas.move_to(x,0);break;
        case 5:canvas.move_to(x,getHeight()/2);break;
        default:canvas.move_to(x,getHeight()*3/4);break;
        }
        canvas.line_to(x,getHeight());
    }
    canvas.stroke();
    canvas.set_font_size(getFontSize());
    for(x=offset,i=startValue;onGetLabel && (x<getWidth());x+=interval,i++){
        std::string label;
        onGetLabel(*this,i-startValue,label);
        if(label.empty())continue;
        canvas.move_to(x,getFontSize());
        canvas.show_text(label);
    }canvas.fill();
}

}

