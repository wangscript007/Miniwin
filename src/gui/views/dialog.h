#ifndef __DIALOG_H__
#define __DIALOG_H__
#include<windows.h>
#include<button.h>
#include<listview.h>
namespace nglui{

class Dialog{
public:
   enum{
       /** The identifier for the positive button. */
       BUTTON_POSITIVE = -1,
       /** The identifier for the negative button. */
       BUTTON_NEGATIVE = -2,
       /** The identifier for the neutral button. */
       BUTTON_NEUTRAL  = -3
   };
   DECLARE_UIEVENT(void,ContentListener,Window*);
protected:
   Window*window;/*window used for container*/
   std::string title;
   std::string message;
   std::map<int,Button*>buttons;
   TextView*messageView;
   ListView*listview;
   ContentListener onCreateContent;
   void measureSize(int&w,int&);
   void createContent();
public:
   Dialog();
   void setContentCreateListener(ContentListener ls);
   virtual Dialog& setTitle(const std::string&);
   virtual Dialog& setMessage(const std::string&);
   virtual Button* getButton(int whichButton);
   virtual Dialog& setButton(int whichButton,const std::string&,View::ClickListener onClick);
   Dialog& setPositiveButton(const std::string&,View::ClickListener onClick);
   Dialog& setNegativeButton(const std::string&,View::ClickListener onClick); 
   Dialog& setNeutralButton(const std::string&,View::ClickListener onClick);
   virtual Dialog& setSingleChoiceItems(const std::vector<std::string>&items,int checkeditem,AbsListView::ItemClickListener onItemClick);
   void show();
};

}/*namespace*/
#endif
