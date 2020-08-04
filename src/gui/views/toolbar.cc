#include<toolbar.h>
#include <ngl_log.h>
#include <app.h>

NGL_MODULE(ToolBar);

namespace nglui{

ToolBar::ToolBar(int w,int h)
 :INHERITED(w,h){
   setBgColor(0xFF000000);
   clearFlag(FOCUSABLE);
   setItemPainter(ToolBar::DefaultPainter);
}

ToolBar::Button::Button(const std::string&txt,int v)
    :AbsListView::ListItem(txt,v){
}

ToolBar::Button::Button(const std::string&img,const std::string&txt,int v)
    :AbsListView::ListItem(txt,v){
    image=App::getInstance().loadImage(img);
}

void ToolBar::Button::setImage(const std::string&res){
    image=App::getInstance().loadImage(res);
}

void ToolBar::addButton(const std::string&txt,int width,int id){
    ToolBar::Button *btn=new ToolBar::Button(txt,id);
    btn->rect.width=width>0?width:txt.size()*getFontSize();
    addItem(btn);
}

int ToolBar::getItemAtPos(int x,int y){
    for(int i=top_;i<list_.size();i++){
         ListItem*itm=getItem(i);
         if(itm->rect.intersect(x,y))
            return i;
    }
    return -1;
}
bool ToolBar::onTouchEvent(MotionEvent&event){
    int x=event.getX();
    int y=event.getY();
    int item_width,item_height,idx;
    int action=event.getActionMasked();
    getItemSize(&item_width,&item_height);
    switch(action){
    case MotionEvent::ACTION_DOWN:
         idx=getItemAtPos(x,y);
         if(idx>=0)setIndex(idx);
         break;
    case MotionEvent::ACTION_MOVE:
         break;
    case MotionEvent::ACTION_UP:
         break;
    }
}

void ToolBar::DefaultPainter(AbsListView&lv,const AbsListView::ListItem&itm,int state,GraphContext&canvas){
    ToolBar::Button&btn=(ToolBar::Button&)itm;
    RECT r=itm.rect;
    bool imgonleft=true;
    std::string txt=itm.getText();
    if(state){
        canvas.set_color(0xFFFF0000);
        canvas.rectangle(itm.rect);
        canvas.fill();
    }
    if(btn.image){
        if(btn.image->get_height()+lv.getFontSize()*4/3<r.height){
            imgonleft=false;
            r.height=itm.rect.height-lv.getFontSize()*2;
        }else
            r.width=btn.image->get_width();
        canvas.draw_image(btn.image,&r,nullptr,imgonleft?ST_CENTER_INSIDE:ST_FIT_START);
        if(imgonleft)r.offset(btn.image->get_width(),0);
        else{
            r.y+=r.height; 
            r.height=itm.rect.height-r.y;
        }
    }
    canvas.set_color(lv.getFgColor());
    canvas.draw_text(r,txt,imgonleft?(DT_LEFT|DT_VCENTER):(DT_CENTER|DT_TOP));
}

void ToolBar::addButton(const std::string&img,const std::string&txt,int width,int id){
    ToolBar::Button *btn=new ToolBar::Button(img,txt,id);
    btn->rect.width=width>0?width:txt.size()*getFontSize();
    addItem(btn);
}

void ToolBar::getItemSize(int *w,int *h)const{
   if( nullptr!=w )
      *w=(item_size.x==-1)?getWidth():item_size.x;

   if( nullptr!=h)
      *h=(item_size.y==-1)?getFontSize()*3/2:item_size.y;
}

bool ToolBar::onKeyUp(KeyEvent&k){
    int pagesize,itemwidth,cnt=getItemCount();
    int idx=getIndex();
    ListItem*itm=getItem(idx);
    if(0==cnt)return false;
    getItemSize(&itemwidth,nullptr);
    pagesize=getWidth()/itemwidth;
    if(idx==-1){
         setIndex(0);
         return true;
    }
    switch(k.getKeyCode()){
    case KEY_LEFT:
         if(idx==0)return false; 
         if(idx==top_&&idx>0){
             top_--;
             invalidate(nullptr);
         }
         setIndex(idx>0?idx-1:0);
         break;
    case KEY_RIGHT:
         if(idx>=list_.size()-1)return false;
         if(itm&&itm->rect.x+itm->rect.width>=getWidth()){
             top_++;
             invalidate(nullptr);
         }
         if(idx<(int)(list_.size()-1))setIndex(idx+1);
         break;
    case KEY_PAGEUP:
         if(top_==0)return false;
         top_=top_>pagesize?top_-pagesize:0;
         idx=index_>pagesize?index_-pagesize:0;
         INHERITED::setIndex(idx);
         invalidate(nullptr);
         return true;
    case KEY_PAGEDOWN:
         if(top_+pagesize>list_.size())return false;
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

void ToolBar::setIndex(int idx){
    ListItem*itm=getItem(index_);
    if(itm)invalidate(&itm->rect);
    itm=getItem(idx);
    if(itm)invalidate(&itm->rect);
    if(idx>=0&&idx<list_.size())
       INHERITED::setIndex(idx);
    if(getParent()){
       RECT rect;
       getFocusRect(rect);
       getParent()->sendMessage(WM_FOCUSRECT,(rect.x<<16|rect.y),(rect.width<<16|rect.height));
    }
}

void ToolBar::getFocusRect(RECT&r)const{
    ListItem*itm=getItem(index_);
    if(itm==NULL)
        INHERITED::getFocusRect(r);
    else{
       r=itm->rect;
       r.offset(getX(),getY()); 
    }
}

void ToolBar::onDraw(GraphContext&canvas){
    RECT rect=getClientRect();
    INHERITED::onDraw(canvas);
    RECT r;
    r.set(0,0,0,getHeight());
    canvas.set_font_size(getFontSize());
    int idx=top_;
    int item_width;
    getItemSize(&item_width,nullptr);
    r.width=item_width;
    r.inflate(0,-1);

    for(auto itm=list_.begin()+top_;itm<list_.end();itm++,idx++){
        Button*btn=dynamic_cast<Button*>((*itm).get());
        if(btn->rect.width==0&&item_size.x<=0){
            TextExtents ext;
            canvas.get_text_extents(btn->getText(),ext);
            r.width=ext.width+getFontSize()*2;
            if(btn->image)r.width+=btn->image->get_width();
        }else{ 
            r.width=(item_size.x<=0)?btn->rect.width:item_size.x;
        } 
        btn->rect=r;
        item_painter_(*this,*btn,idx==index_,canvas);
        r.offset(r.width,0);
        if(r.y>getWidth())break;
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

}//namespace:
