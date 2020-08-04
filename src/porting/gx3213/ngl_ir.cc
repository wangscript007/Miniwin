#include <ngl_input.h>
#include <map>
#include <iostream>
#include <fstream>
#include <ngl_log.h>
#include <ngl_msgq.h>
#include <ngl_timer.h>
#include <string.h>
#include "gxcore.h"
#include "gxos/gxcore_os_core.h"
#include <sys/ioctl.h>
#include "bsp_dev_panel.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <ngl_os.h>

NGL_MODULE(INPUT);

typedef struct{
   int ir_fd;
   int pnl_fd;
   int pipe[2];
}IRDEV;

static IRDEV dev={0,0,0};

#define REGIST_KEY(x) {dev.name2key[#x]=x ;dev.key2name[x]=#x;}

INT nglInputInit(){
    NGLOG_DEBUG("cplusplus=%d",__cplusplus);
    if(dev.pipe[0]>0)return 0;
 
    dev.ir_fd=GxCore_Open("/dev/gxirr0","r");//O_RDONLY);
    dev.pnl_fd=GxCore_Open("/dev/gxpanel0","r");

    pipe(dev.pipe);
}

INT nglInputGetDeviceInfo(int device,INPUTDEVICEINFO*devinfo){
    devinfo->vendor='g'<<8|'x';
    if(device==dev.ir_fd){
        strcpy(devinfo->name,"gxirr0");
        devinfo->source=1<<EV_KEY|1<<EV_SYN;
        devinfo->product='i'<<8|'r';
    }else if(device==dev.pnl_fd){
        strcpy(devinfo->name,"gxpanel0");
        devinfo->source=1<<EV_KEY|1<<EV_SYN;
        devinfo->product='p'<<8|'l';
    }
    switch(device){
    case INJECTDEV_PTR:
         strcpy(devinfo->name,"Mouse-Inject");
         devinfo->vendor=INJECTDEV_PTR>>16;
         devinfo->product=INJECTDEV_PTR&0xFF;
         devinfo->source=(1<<EV_ABS)|(1<<EV_KEY)|(1<<EV_SYN);
         break;
    case INJECTDEV_KEY:
         strcpy(devinfo->name,"Keyboard-Inject");
         devinfo->vendor=INJECTDEV_KEY>>16;
         devinfo->product=INJECTDEV_KEY&0xFF;
         devinfo->source=(1<<EV_KEY)|(1<<EV_SYN);
         break;
    default:break;
    }
    return 0;
}

INT nglInputGetEvents(INPUTEVENT*outevents,UINT count,DWORD timeout){
    fd_set fds;
    struct timeval tv;
    int rc,maxfd=dev.ir_fd;
    INPUTEVENT*evt=outevents;
    if(dev.pnl_fd>maxfd)maxfd=dev.pnl_fd;
    if(dev.pipe[0]>maxfd)maxfd=dev.pipe[0];

    FD_ZERO(&fds);
    FD_SET(dev.ir_fd, &fds);
    FD_SET(dev.pnl_fd, &fds);
    FD_SET(dev.pipe[0],&fds);
    tv.tv_sec = timeout/1000;
    tv.tv_usec =(timeout%1000)*1000;
    bzero(outevents,sizeof(INPUTEVENT)*count);
    if (select(maxfd+ 1, &fds, NULL, NULL, &tv) > 0){
        int isgxdev=0,code;
        evt->type=EV_KEY;
        gettimeofday((struct timeval*)&evt->tv_sec,NULL);
        if (FD_ISSET(dev.ir_fd, &fds)){
            rc=GxCore_Read(dev.ir_fd, &code, sizeof(UINT), 1);
            isgxdev=1;
        }else if (FD_ISSET(dev.pnl_fd, &fds)){
            rc=GxCore_Read(dev.pnl_fd,&code, sizeof(UINT), 1);
            isgxdev=1;
        }else if(FD_ISSET(dev.pipe[0],&fds)){
            rc=read(dev.pipe[0],evt,sizeof(INPUTEVENT));
            evt->value=0;evt++;
        }
        NGLOG_VERBOSE_IF(evt->type==EV_KEY,"KEY:%x,%x[%x] readbytes=%d gxdev=%d",
                evt->type,code,evt->code,rc,isgxdev);
        if(isgxdev){
            evt->code=code;
            evt->device=dev.ir_fd;
            evt->value=1;
            evt[1]=*evt;
            evt[1].value=0;
            evt+=2;
        }
        return evt-outevents;
    }
    return 0;
}

INT nglInputInjectEvents(const INPUTEVENT*es,UINT count,DWORD timeout){
    const char*evtnames[]={"SYN","KEY","REL","ABS","MSC","SW"};
    struct timeval tv;
    INPUTEVENT*evts=(INPUTEVENT*)es;
    gettimeofday(&tv,NULL);
    for(int i=0;i<count;i++){
        evts[i].tv_sec=tv.tv_sec;
        evts[i].tv_usec=tv.tv_usec+i*10;
    }
    int rc=write(dev.pipe[1],evts,count*sizeof(INPUTEVENT));
    NGLOG_VERBOSE_IF(count&&evts[0].type<EV_SW,"pipe=%d %s,%x,%x write=%d",dev.pipe,
           evtnames[evts[0].type],evts[0].code,evts[0].value,rc);
    return count;
}

