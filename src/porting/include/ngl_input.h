#ifndef __NGL_IR_H__
#define __NGL_IR_H__

#include <ngl_types.h>

#ifndef EV_SYN
#include <eventcodes.h>
#endif

#define  KEY_VOLUP   KEY_VOLUMEUP
#define  KEY_VOLDOWN KEY_VOLUMEDOWN
#define  KEY_CHUP    KEY_F11
#define  KEY_CHDOWN  KEY_F12

NGL_BEGIN_DECLS

#define SOURCE_CLASS_BUTTON   0x01 //for Keyboard
#define SOURCE_CLASS_POINTER  0x02 //for Touch,Mouse
#define SOURCE_CLASS_POSITION 0x04 
#define SOURCE_CLASS_JOYSTICK 0x08
typedef struct{
    unsigned int tv_sec;
    unsigned int tv_usec;
    unsigned short type;
    unsigned short code;
    unsigned int value;
    int device;
}INPUTEVENT;//reference to linux input_event

#define MAX_DEVICE_NAME 64
#define INJECTDEV_KEY 0x101080
#define INJECTDEV_PTR 0x101081
typedef struct{
    int source;
    USHORT bustype;
    USHORT vendor;
    USHORT product;
    USHORT version;
    char name[MAX_DEVICE_NAME];
}INPUTDEVICEINFO;

INT nglInputInit();
INT nglInputGetEvents(INPUTEVENT*events,UINT maxevent,DWORD timeout);
INT nglInputInjectEvents(const INPUTEVENT*events,UINT count,DWORD timeout);
INT nglInputGetDeviceInfo(int device,INPUTDEVICEINFO*);

NGL_END_DECLS

#endif

