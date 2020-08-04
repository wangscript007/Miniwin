#ifndef __INPUT_DEVICE_H__
#define __INPUT_DEVICE_H__
#include <uievents.h>
#include <functional>
#include <map>
namespace nglui{

class InputDevice{
public:
   typedef std::function<void(const InputEvent&)>EventListener;
protected:
   int fd;
   int vendor;
   int product;
   int source;
   EventListener listener;
   std::string deviceName;
   std::map<int,int>keymap;
   int isvalid_event(const INPUTEVENT*e);
public:
   InputDevice(int fdev);
   virtual int putrawdata(const INPUTEVENT*){}//PENDING need more rawevent OK,wecan getevent now
   void setEventConsumeListener(EventListener ls){listener=ls;}
   int getSource(){return source;}
   int getVendor(){return vendor;}
   int getProduct(){return product;}
   const std::string&getName(){return deviceName;}
};

class KeyDevice:public InputDevice{
protected:
   int msckey;
   KeyEvent key;
   nsecs_t downtime;
public:
   KeyDevice(int fd);
   virtual int putrawdata(const INPUTEVENT*);
};

class TouchDevice:public InputDevice{
protected:
   MotionEvent mt;
   nsecs_t downtime;
   BYTE buttonstats[16];
   PointerCoords coords[32];
   PointerProperties ptprops[32];
public:
   TouchDevice(int fd);
   virtual int putrawdata(const INPUTEVENT*);
};

class MouseDevice:public TouchDevice{
public:
   MouseDevice(int fd):TouchDevice(fd){}
   virtual int putrawdata(const INPUTEVENT*);
};
}//namespace
#endif 
