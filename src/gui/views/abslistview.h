#ifndef __UI_ABSLIST_H__
#define __UI_ABSLIST_H__
#include <widget.h>
namespace nglui{

class AbsListView:public Widget{
public:
   class ListItem{
   private:
     std::string text_;
     int id_;
   public:
     RECT rect;//used to store item's rectangle(last draw rect)
   public:
     ListItem(const std::string&txt,int id=0);
     virtual ~ListItem();
     virtual const std::string& getText()const;
     virtual void setText(const std::string&);
     virtual void setId(int v){id_=v;}
     virtual int getId()const{return id_;}
     virtual bool operator==(const ListItem & b)const{return id_==b.id_;}
   };
   DECLARE_UIEVENT(void,ItemSelectListener,AbsListView&,const ListItem&,int);
   DECLARE_UIEVENT(void,ItemClickListener,AbsListView&,const ListItem&,int);
   DECLARE_UIEVENT(void,ItemPainter,AbsListView&,const ListItem&,int state,GraphContext&);
   DECLARE_UIEVENT( int,ItemCompare,const ListItem&a,const ListItem&b);
   static void DefaultPainter(AbsListView&,const ListItem&,int state,GraphContext&);
protected:
   int index_;
   int top_;
   SIZE item_size;
   ItemPainter item_painter_;
   ItemSelectListener item_select_listener;
   ItemClickListener item_click_listener;
   std::vector< std::shared_ptr<ListItem> >list_;
   virtual void getItemSize(int*w,int*h)const=0;
public:
   AbsListView(int w,int h);
   AbsListView(const std::string&txt,int w,int h);
   virtual void sort(ItemCompare ,bool reverse=false);
   virtual int find(const ListItem&a)const;
   virtual int getIndex()const;
   virtual int getTop()const;
   virtual void setIndex(int idx);
   virtual void setTop(int idx);//the first visibal item
   virtual unsigned int getItemCount()const;
   virtual void setItemSize(int w,int h);
   virtual void setItemPainter(ItemPainter painter);
   virtual void setItemSelectListener( ItemSelectListener lis);
   virtual void setItemClickListener(ItemClickListener lis);
   virtual ListItem* getItem(int idx)const;
   virtual void addItem(ListItem*itm);
   virtual void add(ListItem*itm);
   virtual void addItem(std::shared_ptr<ListItem>itm);
   virtual void remove(int idx);
   virtual void remove(ListItem*itm);
   virtual void clear();
   virtual bool onKeyUp(KeyEvent&k)override;
   typedef Widget INHERITED;

};
}//namespace
#endif
