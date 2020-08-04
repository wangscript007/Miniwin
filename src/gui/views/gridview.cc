#include<gridview.h>
#include <ngl_log.h>

NGL_MODULE(GRIDVIEW);

namespace nglui{

GridView::GridView(int w,int h)
 :INHERITED(w,h){
   setBgColor(0xFF000000);
}

void GridView::getItemSize(int *w,int *h)const{
   if( nullptr!=w )
      *w=(item_size.x==-1)?getWidth():item_size.x;

   if( nullptr!=h)
      *h=(item_size.y==-1)?getFontSize()*3/2:item_size.y;
}

bool GridView::onKeyUp(KeyEvent&k){
    int pagesize,itmwidth,itmheight;
    int idx=getIndex();
    int cnt=getItemCount();
    int cols,rows,top0=top_;
    ListItem*itm=getItem(idx);
    if(0==cnt)return false;
    getItemSize(&itmwidth,&itmheight);
    cols=getWidth()/itmwidth;
    rows=getHeight()/itmheight;
    pagesize=cols*rows;
    switch(k.getKeyCode()){
    case KEY_UP:
         if(idx<cols)return false;
         if(top_>0&&idx<=top_+cols){
             top_-=cols;
             scrollBy(0,-itmheight); 
             invalidate(nullptr);
         }
         setIndex(idx-cols);
         break;
    case KEY_DOWN:
         if(index_<0){setIndex(0);break;}
         if(idx>=list_.size()-1)return false;
         if(idx+cols>=top_+pagesize){
             top_+=cols;
             scrollBy(0,itmheight); 
             invalidate(nullptr);
         }
         setIndex((idx+cols<list_.size())?(idx+cols):(list_.size()-1));
         break;
    case KEY_LEFT:
         if( idx==0 ) return false;
         if(idx>top_) idx-=1;
         else if(top_>0){
             top_-=cols;
             idx-=1;
             scrollBy(0,-itmheight);
             invalidate(nullptr);
         }
         setIndex(idx);
         break;
    case KEY_RIGHT:
         if(idx+1>=list_.size())return false;
         if(idx+1<list_.size()){
             if(idx+1>=top_+cols*rows){
                 top_+=cols;
                 scrollBy(0,itmheight);
                 invalidate(nullptr);
             }
             setIndex(idx+1);
         }
         break;
    case KEY_PAGEUP:
         top_=top_>pagesize?top_-pagesize:0;
         idx=index_>pagesize?index_-pagesize:0;
         scrollBy(0,(top_-top0)*itmheight/cols);
         setIndex(idx);
         return true;
    case KEY_PAGEDOWN:
         top_=top_+pagesize<cnt?top_+pagesize:cnt-pagesize;
         scrollBy(0,(top_-top0)*itmheight/cols);
         idx=index_+pagesize<cnt?index_+pagesize:cnt-1;
         setIndex(idx);
         return true;
    default:return INHERITED::onKeyUp(k);
    }
    return true;
}

void GridView::setIndex(int idx){
    RECT rect;
    if(idx!=index_){
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

void GridView::getFocusRect(RECT&r)const{
    ListItem*itm=getItem(index_);
    if(itm==NULL)
        INHERITED::getFocusRect(r);
    else{ 
       int cols,rows;
       getItemSize(&r.width,&r.height);
       cols=getWidth()/r.width;
       rows=getHeight()/r.height;
       r.y=((index_-top_)/cols)*r.height;
       r.x=((index_-top_)%cols)*r.width;
       r.offset(getX()-getScrollX(),getY()-getScrollY());
    }
}

void GridView::onDraw(GraphContext&canvas){
    RECT rect=getClientRect();
    INHERITED::onDraw(canvas);
    RECT r=rect;
    if(hasFlag(BORDER))r.inflate(-2,-2);
    canvas.set_font_size(getFontSize());
    int idx=0;
    int item_width,item_height;
    getItemSize(&item_width,&item_height);
    r.height=item_height;
    r.width=item_width;

    for(auto itm:list_){//.begin();itm<list_.end();itm++,idx++){
        itm->rect=r;
        if(r.bottom()>getScrollY()){
           item_painter_(*this,*itm,idx==index_,canvas);
        }idx++;
        if(r.x+item_width<getWidth())r.x+=item_width;
        else{r.x=1;r.y+=item_height;}
        if(r.y-getScrollY()>=getHeight())break;
    }
    SCROLLINFO sb;
    sb.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
    sb.nPage=idx-top_;
    sb.nMin=0;
    sb.nMax=list_.size();
    sb.nPos=top_;
    if(hasFlag(SCROLLBARS_VERTICAL)&&sb.nMax>sb.nPage){ 
       setScrollInfo(SB_VERT,&sb);
       drawScrollBar(canvas,SB_VERT);
    }
}

}//namespace:
