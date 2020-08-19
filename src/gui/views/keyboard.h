#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include<keyboardview.h>
#include<windows.h>
namespace nglui{

class Keyboard:public Window{
protected:
   KeyboardView*kv;
public:
   Keyboard(int x,int y,int w,int h);
   void setBuddy(EditText*buddy);
   virtual int setLanguage(const std::string&lan);
};

}//namespace
#endif
