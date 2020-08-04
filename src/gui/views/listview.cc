#include<listview.h>
#include <ngl_log.h>

NGL_MODULE(LISTVIEW);

namespace nglui{

ListView::ListView(int w,int h)
 :INHERITED(w,h){
   setBgColor(0xFF000000);
}

void ListView::getItemSize(int *w,int *h)const{
   if( nullptr!=w )
      *w=(item_size.x==-1)?getWidth():item_size.x;

   if( nullptr!=h)
      *h=(item_size.y==-1)?getFontSize()*3/2:item_size.y;
}

bool ListView::onKeyUp(KeyEvent&k){
    int pagesize,itmheight,top0=top_;
    int cnt=getItemCount();
    int idx=getIndex();
    ListItem*itm=getItem(idx);
    if(0==cnt)return false;
    getItemSize(nullptr,&itmheight);
    pagesize=getHeight()/itmheight;
    if(index_<0){
        setIndex(0);
        return true;
    }
    switch(k.getKeyCode()){
    case KEY_UP:
         if(idx==0)return false; 
         if(idx==top_&&idx>0){
             top_--;
             scrollBy(0,-itmheight);
             invalidate(nullptr);
         }
         setIndex(idx>0?idx-1:0);
         break;
    case KEY_DOWN:
         if(idx+1>=top_+pagesize&&idx<list_.size()-1){
             top_++;
             scrollBy(0,itmheight);
             invalidate(nullptr);
         }
         if(idx+1<(int)(list_.size()))
             setIndex(idx+1);
         else return false;
         break;
    case KEY_PAGEUP:
         if(idx==0)return false;
         top_=top_>pagesize?top_-pagesize:0;
         scrollBy(0,(top_-top0)*itmheight);
         idx=index_>pagesize?index_-pagesize:0;
         setIndex(idx);
         return true;
    case KEY_PAGEDOWN:
         if(top_+pagesize>=cnt)return false;
         top_=top_+pagesize<cnt?top_+pagesize:cnt-pagesize;
         scrollBy(0,(top_-top0)*itmheight);
         idx=index_+pagesize<cnt?index_+pagesize:cnt-1;
         setIndex(idx);
         return true;
    default:return INHERITED::onKeyUp(k);
    }
    return true;
}

void ListView::setIndex(int idx){
    RECT rect;
    if(index_!=idx){
        if(index_>=0){
           rect=getItem(index_)->rect;
           rect.offset(-getScrollX(),-getScrollY());
           invalidate(&rect);
        }

        rect=getItem(idx)->rect;
        rect.offset(-getScrollX(),-getScrollY());
        invalidate(&rect);
    }
    if(idx>=0&&idx<list_.size())
       INHERITED::setIndex(idx);
    if(getParent()){
       getFocusRect(rect);
       getParent()->sendMessage(WM_FOCUSRECT,(rect.x<<16|rect.y),(rect.width<<16|rect.height));
    }
}

void ListView::getFocusRect(RECT&r)const{
    ListItem*itm=getItem(index_);
    r.set(0,0,0,0);
    if(itm){
       r=itm->rect;
       r.offset(getX()-getScrollX(),getY()-getScrollY()); 
       r.intersect(getClientRect()); 
    }
}

bool ListView::onTouchEvent(MotionEvent&event){
    int x=event.getX();
    int y=event.getY();
    int item_height;
    int action=event.getActionMasked();
    getItemSize(nullptr,&item_height);
    switch(action){
    case MotionEvent::ACTION_DOWN:
         setIndex((getScrollY()+y)/item_height);
         point_touch.set(x,y);
         break;
    case MotionEvent::ACTION_MOVE:
         NGLOG_VERBOSE("action_move(%d,%d) btns=%x",x,y,event.getActionButton());
         if(event.getActionButton()){
             setScrollY(getScrollY()-(y-point_touch.y));
             point_touch.set(x,y);
             if(getParent()){
                 RECT rect;
                 getFocusRect(rect);
                 getParent()->sendMessage(WM_FOCUSRECT,(rect.x<<16|rect.y),(rect.width<<16|rect.height));
             }
         }
         break;
    case MotionEvent::ACTION_UP:
         NGLOG_VERBOSE_IF(event.getActionButton(),"action_up(%d,%d)sy=%d btns=%x",x,y,getScrollY(),event.getActionButton());
         break;
    }
}

void ListView::onDraw(GraphContext&canvas){
    RECT rect=getClientRect();
    INHERITED::onDraw(canvas);
    RECT r;
    r.set(0,0,getWidth(),0);
    canvas.set_font_size(getFontSize());
    int idx=top_;
    int item_height;
    getItemSize(nullptr,&item_height);
    r.height=item_height;
    r.inflate(-2,0);
    r.y=item_height*top_;

    for(auto itm=list_.begin()+top_;itm<list_.end();itm++,idx++){
        (*itm)->rect=r;
        if(r.bottom()>getScrollY()){
            canvas.save();
            item_painter_(*this,*(*itm),idx==index_,canvas);
            canvas.restore();
        }
        r.offset(0,item_height);
        if(r.y-getScrollY()>=getHeight())break;
    }
    SCROLLINFO sb;
    sb.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
    sb.nPage=idx-top_;
    sb.nMin=0;
    sb.nMax=list_.size()?list_.size()-1:0;
    sb.nPos=top_;
    if(hasFlag(SCROLLBARS_VERTICAL)){ 
       setScrollInfo(SB_VERT,&sb);
       drawScrollBar(canvas,SB_VERT);
    }
    if(hasFlag(BORDER)){
        canvas.set_color(getFgColor());
        canvas.rectangle(getClientRect());
        canvas.stroke();
    }
}

}//namespace:
