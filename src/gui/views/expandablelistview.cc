#include <expandablelistview.h>
#include <ngl_log.h>

#define MIN(x,y) ((x)>(y)?(y):(x))

NGL_MODULE(EXPANDABLELIST)

namespace nglui{

ExpandableListView::ExpandableListView(int w,int h):AbsListView(w,h){
   horz=0;
}

void ExpandableListView::getItemSize(int *w,int *h)const{
   if( nullptr!=w )
      *w=(item_size.x==-1)?getWidth():item_size.x;

   if( nullptr!=h)
      *h=(item_size.y==-1)?getFontSize()*3/2:item_size.y;
}

void ExpandableListView::onDraw(GraphContext&canvas){
    RECT rect=getClientRect();
    INHERITED::onDraw(canvas);
    RECT r;
    r.set(0,0,getWidth(),0);
    canvas.set_font_size(getFontSize());
    int idx=top_;
    int item_height,buddy_height=0;
    getItemSize(nullptr,&item_height);
    r.height=item_height;
    r.inflate(-1,0);
    if(index_>=0)
        buddy_height=dynamic_cast<ExpandableListItem*>(list_[index_].get())->getBuddyHeight();
    for(auto it=list_.begin()+idx;it<list_.end();it++,idx++){
        ExpandableListItem*itm=dynamic_cast<ExpandableListItem*>((*it).get());
        r.y=item_height*(idx-top_)+(idx>index_?buddy_height:0);
        itm->rect=r;
        item_painter_(*this,*itm,idx==index_,canvas);
        if(r.y>getHeight())break;
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
    if(hasFlag(BORDER)){
        canvas.set_color(getFgColor());
        canvas.rectangle(getClientRect());
        canvas.stroke();
    }
}

bool ExpandableListView::onKeyUp(KeyEvent&k){
    int pagesize,itmheight;
    int cnt=getItemCount();
    int idx=getIndex();
    ListItem*itm=getItem(idx);
    if(0==cnt)return false;
    getItemSize(nullptr,&itmheight);
    pagesize=getHeight()/itmheight;
    switch(k.getKeyCode()){
    case KEY_UP:
         if(idx==top_&&idx>0){
             top_--;
             invalidate(nullptr);
         }
         setIndex(idx>0?idx-1:0);
         break;
    case KEY_DOWN:
         if(itm&&itm->rect.y+itm->rect.height>=getHeight()){
             top_++;
             invalidate(nullptr);
         }
         if(idx<(int)(list_.size()-1))setIndex(idx+1);
         break;
    case KEY_PAGEUP:
         top_=top_>pagesize?top_-pagesize:0;
         idx=index_>pagesize?index_-pagesize:0;
         INHERITED::setIndex(idx);
         invalidate(nullptr);
         return true;
    case KEY_PAGEDOWN:
         top_=top_+pagesize<cnt?top_+pagesize:cnt-pagesize;
         idx=index_+pagesize<cnt?index_+pagesize:cnt-1;
         INHERITED::setIndex(idx);
         invalidate(nullptr);
         return true;
    case KEY_ENTER:
         return View::onKeyUp(k);
    default:return false;
    }
    return true;
}

void ExpandableListView::setIndex(int idx){
    int item_height;
    ExpandableListItem*itm=dynamic_cast<ExpandableListItem*>(getItem(index_));
    getItemSize(nullptr,&item_height);

    if(itm)
        itm->getBuddy()->clearFlag(VISIBLE);

    itm=dynamic_cast<ExpandableListItem*>(getItem(idx));
    if(itm){
        RECT ri=itm->rect;
        RECT r=itm->rect;
        r.width =itm->getBuddyWidth();
        r.height=itm->getBuddyHeight();
        r.x=0;
        r.y=(idx-top_+1)*item_height;
        itm->getBuddy()->setFlag(VISIBLE); 
        itm->getBuddy()->setBound(r);
        itm->getBuddy()->invalidate(nullptr);
    }
    RECT r=getClientRect();
    r.y=MIN(idx,index_)*item_height;
    invalidate(&r);
    if(idx>=0&&idx<list_.size())
       INHERITED::setIndex(idx);
}

}
