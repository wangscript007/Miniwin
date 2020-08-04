#include <abslistview.h>
#include <ngl_log.h>
#include <algorithm>
NGL_MODULE(ABSLISTVIEW);

namespace nglui{

AbsListView::AbsListView(int w,int h)
:AbsListView(std::string(),w,h){
}

AbsListView::AbsListView(const std::string& txt,int w,int h)
:INHERITED(txt,w,h){
   setFlag(FOCUSABLE);
   setFlag(ENABLED);
   index_=-1;
   top_=0;
   item_size.set(-1,-1);
   item_select_listener=nullptr;
   item_click_listener=nullptr;
   item_painter_=DefaultPainter;// nullptr;
}


void AbsListView::DefaultPainter(AbsListView&lv,const ListItem&itm,int state,GraphContext&canvas){
    canvas.set_color(state?0xFFFF0000:lv.getBgColor());
    canvas.rectangle(itm.rect);
    canvas.fill();
    canvas.set_color(lv.getFgColor());
    canvas.draw_text(itm.rect,itm.getText(),DT_LEFT|DT_VCENTER);
}

void AbsListView::setItemPainter(ItemPainter painter){
    item_painter_=painter;
}

void AbsListView::setItemSelectListener( ItemSelectListener listener){
    item_select_listener=listener;    
}

void AbsListView::setItemClickListener(ItemClickListener listener){
    item_click_listener=listener;
}

void AbsListView::setItemSize(int w,int h){
    if( ((w!=-1) && (w!=item_size.x)) || ((h!=-1) && (h!=item_size.y))){
       if(-1!=w)item_size.x=w;
       if(-1!=h)item_size.y=h;
       invalidate(nullptr);
    }
}

void AbsListView::sort(ItemCompare cmp,bool reverse){
    //std::sort myb caused crash :( if function cmp has logical error.
    ListItem*cur=getItem(index_);
    std::sort(list_.begin(),list_.end(),
         [&](std::shared_ptr<ListItem>a, std::shared_ptr<ListItem> b)->bool{
                bool rc=cmp(*a,*b);
                return (reverse==false)?rc:(!rc);
         });
    if(cur)index_=find(*cur);
}

int AbsListView::find(const ListItem&a)const{
    for(auto i=list_.begin();i!=list_.end();i++){
       if(*(*i)==a)return i-list_.begin();
    }
    return -1;
}

void AbsListView::setIndex(int idx){
    if( index_!=idx && idx>=0 && idx<list_.size() ){
        index_=idx;
        if(item_select_listener!=nullptr)
            item_select_listener(*this,*getItem(index_),index_);
    }
}

void AbsListView::setTop(int idx){
    if(top_!=idx){
         top_=idx; 
         invalidate(nullptr);
    }
}

int AbsListView::getIndex()const{
    return index_;
}

int AbsListView::getTop()const{
    return top_;
}

unsigned int AbsListView::getItemCount()const{
    return list_.size();
}

AbsListView::ListItem* AbsListView::getItem(int idx)const{
    if(idx>=0&&idx<list_.size()){
        return list_[idx].get();
    }
    return nullptr;
}

void AbsListView::addItem(AbsListView::ListItem*item){
    std::shared_ptr<AbsListView::ListItem>p(item);
    addItem(p);
}

void AbsListView::addItem(std::shared_ptr<ListItem>itm){
    list_.push_back(itm);
    invalidate(nullptr); 
}

void AbsListView::removeItem(int idx){
    if(idx==index_){
        if(idx==0)index_==0;
        if(idx==list_.size()-1);index_=list_.size()-2;//do nothingindex_--;redraw++;
        if(item_select_listener!=nullptr)
            item_select_listener(*this,*getItem(index_),index_);
    }else if(index_>idx){//idx<=index_
        if(idx>0)index_--;
    }
    if(idx<top_&&idx>0){top_--;}
    list_.erase(list_.begin()+idx);
    invalidate(nullptr);
}

void AbsListView::removeItem(AbsListView::ListItem*itm){
    size_t idx=0;
    for(auto itr=list_.begin();itr!=list_.end();itr++,idx++){
        if(itr->get()==itm){
           removeItem(idx);//list_.erase(itr);
           break;
        }
    }
}

void AbsListView::clearAllItems(){
    list_.erase(list_.begin(),list_.end());
    index_=-1;top_=0;
    scrollTo(0,0);
    invalidate(nullptr);
}


AbsListView::ListItem::ListItem(const std::string&txt,int id)
  :text_(txt){
  id_=id;
  rect.set(0,0,0,0);
}

AbsListView::ListItem::~ListItem(){
}

const std::string& AbsListView::ListItem::getText()const{
   return text_;
}

void AbsListView::ListItem::setText(const std::string&txt){
   text_=txt;   
}

bool AbsListView::onKeyUp(KeyEvent&key){
    switch(key.getKeyCode()){
    case KEY_ENTER:
         if(item_click_listener&&index_>=0&&index_<list_.size())
             item_click_listener(*this,*getItem(index_),index_);
         return true;
    default:return View::onKeyUp(key);
    }
}

}//namespace
