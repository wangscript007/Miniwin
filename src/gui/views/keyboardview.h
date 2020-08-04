#ifndef __KEYBOARD_VIEW_H__
#define __KEYBOARD_VIEW_H__

#include<view.h>
#include<widget.h>
#include<vector>
#include<edittext.h>
namespace nglui{

class KeyboardView:public View{
public:
typedef struct{
   UINT unicode;
   UINT upcode;
   USHORT x;
   USHORT y;
   USHORT width;
   USHORT height;
   std::string text;
}KEY;
enum{
   KC_NONE,
   KC_CAPS,
   KC_LEFT,
   KC_RIGHT,
   KC_INSERT,
   KC_DELETE,
   KC_BKSPACE,
   KC_RETURN,
   KC_LANS //used to switch language
}CTRL_KEY;
private:
   int kx,ky;
   INT kcol,krow;
   std::vector<std::vector<int>>kbd_lines;
protected:
   std::vector<KEY>keys;
   UINT key_bg_color;
   UINT key_index;
   EditText*m_editbox;
public:
   KeyboardView(int w,int h);
   void addKey(UINT code=0,UINT up=0,USHORT w=40,USHORT h=40,const char*txt=nullptr);
   void addNewKeyLine(UINT line_height=0);
   void setKeyBgColor(UINT cl);
   UINT getKeyBgColor();
   void setKeyIndex(UINT idx);
   void setBuddy(EditText*edt);
   EditText*getBuddy();
   virtual void onDraw(GraphContext&canvas);
   virtual bool onKeyUp(KeyEvent&k);
};
}//namespace
#endif
