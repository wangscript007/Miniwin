#include<linux/input.h>
#include<ngl_input.h>
#include<map>
#include<iostream>
#include<fstream>
#include<vector>
#include<ngl_log.h>
#include<ngl_msgq.h>
#include<ngl_timer.h>
#include<string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
//#include <input_helpers.h>

NGL_MODULE(INPUT);

typedef struct{
   int maxfd;
   int nfd;
   int inotify;
   int pipe[2]; 
   int fds[128];
   int source[128];//source type
   std::map<int,int>keymap;
}INPUTDEVICE;

static INPUTDEVICE dev={0,0};

static int test_bit(int nr, uint32_t * addr){
    int mask;
    addr += nr >> 5;
    mask = 1 << (nr & 0x1f);
    return ((mask & *addr) != 0);
}
    
static int getfeatures(int fd){
   BYTE bitmask[EV_MAX+1];
   static char features[256];
   const char*evs[]={"SYN ","KEY ","REL ","ABS ","MSC ","SW ","LED ","SND ","REP ","FF ","PWR ","FFS ","MAX"};
   int rc;
   int source=0;
   memset(bitmask,0,EV_MAX+1);
   rc=ioctl(fd, EVIOCGBIT(0, EV_MAX),bitmask);
   features[0]=0;
   NGLOG_DEBUG_IF(rc<=0,"EVIOCGBIT %d",rc);
   for(int i=0;i<0x17/*EV_MAX*/;i++){
       if(test_bit(i,(unsigned int*)bitmask)&&(i<=5||i>0x10)){
           strcat(features,evs[(i<=5?i:i-10)]);
           source|=(1<<i);
       }
   }
   NGLOG_DEBUG("fd:%d feature:%s source=%x",fd,features,source);
   return source;
}

INT nglInputInit(){
    NGLOG_DEBUG("cplusplus=%d",__cplusplus);
    if(dev.pipe[0]>0)
        return 0;
    DIR*dir=opendir("/dev/input");
    struct dirent *entry;
    pipe(dev.pipe);
    dev.fds[dev.nfd++]=dev.pipe[0];
    dev.maxfd=dev.pipe[0];
    fcntl(dev.pipe[0],F_SETFL,O_NONBLOCK);
    dev.inotify=inotify_init();
    while(dir&&(entry=readdir(dir))){
        char fname[256]={0},name[256]={0};
        int fd,source=0;
        struct input_id id;
        sprintf(fname,"/dev/input/%s",entry->d_name);
        if(entry->d_type==DT_DIR)continue;
        if((fd=open(fname,O_RDWR/*|O_NONBLOCK*/))<0)continue;

        if(fd>dev.maxfd)dev.maxfd=fd;
        ioctl(fd, EVIOCGNAME(sizeof(name) - 1),name);
        dev.fds[dev.nfd]=fd;
        dev.source[dev.nfd++]=getfeatures(fd);
    }
    closedir(dir);
}

INT nglInputGetDeviceInfo(int device,INPUTDEVICEINFO*devinfo){
    struct input_id id;
    int rc1,rc2;
    memset(devinfo->name,0,sizeof(devinfo->name));
    rc1=ioctl(device, EVIOCGNAME(sizeof(devinfo->name) - 1),devinfo->name);
    rc2=ioctl(device, EVIOCGID, &id);
    NGLOG_DEBUG_IF(rc2,"fd=%d[%s] rc1=%d,rc2=%d ",device,devinfo->name,rc1,rc2);
    devinfo->source=getfeatures(device);
    devinfo->product=id.product;
    devinfo->vendor=id.vendor;
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

INT nglInputInjectEvents(const INPUTEVENT*es,UINT count,DWORD timeout){
    const char*evtnames[]={"SYN","KEY","REL","ABS","MSC","SW"};
    struct timeval tv;
    INPUTEVENT*events=(INPUTEVENT*)malloc(count*sizeof(INPUTEVENT));
    memcpy(events,es,count*sizeof(INPUTEVENT));
    gettimeofday(&tv,NULL);
    for(int i=0;i<count;i++){
        events[i].tv_sec=tv.tv_sec;
        events[i].tv_usec=tv.tv_usec;
    }
    if(dev.pipe[1]>0){
       int rc=write(dev.pipe[1],events,count*sizeof(INPUTEVENT));
       NGLOG_VERBOSE_IF(count&&(es->type<=EV_SW),"pipe=%d %s,%x,%x write=%d",dev.pipe[1],evtnames[es->type],es->code,es->value,rc);
    }
    free(events);
    return count;
}

INT nglInputGetEvents(INPUTEVENT*outevents,UINT max,DWORD timeout){
    int rc,ret=0;
    struct timeval tv;
    struct input_event events[64];
    INPUTEVENT*e=outevents;
    fd_set rfds;
    static const char*type2name[]={"SYN","KEY","REL","ABS","MSC","SW"};
    tv.tv_usec=(timeout%1000)*1000;//1000L*timeout;
    tv.tv_sec=timeout/1000;
    FD_ZERO(&rfds);
    for(int i=0;i<dev.nfd;i++){
        FD_SET(dev.fds[i],&rfds);
    }
    rc=select(dev.maxfd+1,&rfds,NULL,NULL,&tv);
    if(rc<0){
        NGLOG_DEBUG("select error");
        return NGL_ERROR;
    }
    for(int i=0;i<dev.nfd;i++){
        if(!FD_ISSET(dev.fds[i],&rfds))continue;
        if(dev.fds[i]!=dev.pipe[0]){
           rc=read(dev.fds[i],events, (max-ret)*sizeof(struct input_event));
           for(int j=0;j<rc/sizeof(struct input_event);j++,e++){
               *(struct input_event*)e=events[j];
               e->device=dev.fds[i];
               NGLOG_VERBOSE_IF(e->type<EV_SW,"fd:%d [%s]%x,%x,%x ",dev.fds[i],
                  type2name[e->type],e->type,e->code,e->value);
           }
        }else{//for pipe
           rc=read(dev.fds[i],e, (max-ret)*sizeof(INPUTEVENT));
           e+=rc/sizeof(INPUTEVENT);
        }
        NGLOG_VERBOSE_IF(rc,"fd %d read %d bytes ispipe=%d",dev.fds[i],rc,dev.fds[i]==dev.pipe[0]);
    }
    return e-outevents;
}

