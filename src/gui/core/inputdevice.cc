#include <ngl_input.h>
#include <inputdevice.h>
#include <ngl_log.h>
#include <chrono>
#include <string.h>
#include <fstream>

NGL_MODULE(InputDevice);

namespace nglui{

#define NOW std::chrono::steady_clock::now().time_since_epoch().count()

InputDevice::InputDevice(int fdev):fd(fdev),listener(nullptr){
   INPUTDEVICEINFO info;
   nglInputGetDeviceInfo(fdev,&info);
   deviceName=info.name;
   product=info.product;
   vendor=info.vendor;
   source=info.source;
   NGLOG_DEBUG("device %d source=%x vid/pid=%x/%x name=%s",fdev,source,vendor,product,info.name);
}
int InputDevice::isvalid_event(const INPUTEVENT*e){
    return ((1<<e->type)&source)==(1<<e->type);
}

#define SEP " \t,;:"
KeyDevice::KeyDevice(int fd)
   :InputDevice(fd){
   msckey=0;
   std::ifstream fin;
   std::string fname=deviceName+".map";
   char line[256];

   fin.open(fname);
   if(!fin.is_open()){
       char name[32];
       sprintf(name,"%x-%x.map",vendor,product);
       fin.open(name);
   }
   NGLOG_ERROR_IF(!fin.is_open(),"Keymap %s or %x-%x.map not found!!!",fname.c_str(),vendor,product);

   while(fin.is_open()&&fin.getline(line,256)){//read keymap format as KEY_NAME ,key_code
       int idx=0;
       char *p,*ps[32];
       p=strtok(line,SEP);
       while(p&&idx<32){
           ps[idx++]=p;
           p=strtok(NULL,SEP);
       }
       if(idx<2||NULL==strstr(ps[0],"KEY"))continue;
       int base=strncasecmp(ps[1],"0x",2)==0?16:10;
       long key=strtoll(ps[1],NULL,base);
       std::string kname(ps[0]);
       int uikey=KeyEvent::getKeyCodeFromLabel(kname.c_str());
       NGLOG_VERBOSE("keymap[%s],0x%x-->%x",kname.c_str(),key,uikey);
       keymap[key]=uikey;
   }
}

int KeyDevice::putrawdata(const INPUTEVENT*e){
    int code=e->code;
    if(!isvalid_event(e)){
         NGLOG_DEBUG("invalid event type %x source=%x",e->type,source);
         return -1;
    }
    switch(e->type){
    case EV_KEY:
        if(keymap.find(e->code)!=keymap.end())code=keymap[e->code];
        key.initialize(fd,source,(e->value?KeyEvent::ACTION_DOWN:KeyEvent::ACTION_UP)/*action*/,0/*flags*/,code,0/*scancode*/,
             0/*metaState*/,1/*repeatCount*/, downtime,NOW/*eventtime*/);
        NGLOG_VERBOSE("fd[%d] keycode:%08x->%04x[%s] action=%d",fd,e->code,code,KeyEvent::getLabel(code),e->value);
        if(listener)listener(key); 
        break;
    case EV_SYN:
        NGLOG_VERBOSE("fd[%d] keycode=%d action=%d",fd,e->value,e->code);
        break;
    }
    return 0;
}

TouchDevice::TouchDevice(int fd):InputDevice(fd){
    memset(coords,0,sizeof(coords));
    memset(ptprops,0,sizeof(ptprops));
    memset(buttonstats,0,sizeof(buttonstats));
    memset(&mt,0,sizeof(mt));
}

int TouchDevice::putrawdata(const INPUTEVENT*e){
    if(!isvalid_event(e))return -1;
    return 0;
}

int MouseDevice::putrawdata(const INPUTEVENT*e){
    BYTE btnmap[]={ MotionEvent::BUTTON_PRIMARY  ,/*BTN_LEFT*/
                    MotionEvent::BUTTON_SECONDARY,/*BTN_RIGHT*/
                    MotionEvent::BUTTON_TERTIARY/*BTN_MIDDLE*/ ,0,0};
    int act_btn=e->value-BTN_MOUSE;
    if(!isvalid_event(e))return -1;
    switch(e->type){
    case EV_KEY:
         downtime=NOW;
         buttonstats[act_btn]=e->code;
         NGLOG_VERBOSE("Key %x /%d btn=%d",e->value,e->code,btnmap[act_btn]);
         mt.setAction(e->code?MotionEvent::ACTION_DOWN:MotionEvent::ACTION_UP);
         mt.setActionButton(btnmap[act_btn]);
         if(listener)listener(mt);
         if(e->code==0)mt.setActionButton(0);
         break;
    case EV_ABS:
         coords->setAxisValue(e->code,e->value);
         mt.setAction(MotionEvent::ACTION_MOVE);
         NGLOG_VERBOSE("ABS[%d]=%d",e->code,e->value);
         break;
    case EV_SYN:
         mt.initialize(fd,source,mt.getAction()/*action*/,mt.getActionButton()/*actionbutton*/,
                0/*flags*/,  0/*edgeFlags*/,0/*metaState*/,0/*buttonState*/,
                0/*xOffset*/,0/*yOffset*/,0/*xPrecision*/,0/*yPrecision*/,
                downtime,NOW,1/*pointerCount*/,ptprops,coords);
         NGLOG_VERBOSE("MouseDevice::SYN");
         if(listener)listener(mt);
         break;
    }
    return 0;
}

}
