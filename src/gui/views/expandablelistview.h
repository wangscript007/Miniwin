#ifndef __EXPANDABLE_LIST_VIEW_H__
#define __EXPANDABLE_LIST_VIEW_H__

#include <abslistview.h>

namespace nglui{

class ExpandableListView:public AbsListView{
typedef AbsListView INHERITED;
protected:
   int horz;
   void getItemSize(int*w,int*h)const override;
public:
 class ExpandableListItem:public ListItem{
 protected:
    View*buddy;
    SIZE buddy_size;
 public:
    ExpandableListItem(const std::string&txt,View*_buddy=nullptr,int v=0):ListItem(txt,v){
       buddy=_buddy;
    }
    void setBuddy(View*v){
        buddy=v;
        v->clearFlag(VISIBLE);
    }
    void setBuddySize(int w,int h){
       buddy_size.set(w,h);
    }
    int getBuddyWidth(){return buddy_size.x;}
    int getBuddyHeight(){return buddy_size.y;}
    View*getBuddy(){return buddy;}
 };
public:
   ExpandableListView(int w,int h);
   virtual void onDraw(GraphContext&canvas)override;
   virtual bool onKeyUp(KeyEvent&k)override;
   virtual void setIndex(int idx)override; 
};

}


#endif

