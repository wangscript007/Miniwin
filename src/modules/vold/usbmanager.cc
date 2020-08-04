#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/un.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/types.h> 
#include <linux/netlink.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_os.h>
#include <usbmanager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <strstream>
#include <fstream>
#include <mntent.h>
#include <volumemanager.h>
#include <netlinkevent.h>

NGL_MODULE(USBMONITOR)

 
USBManager*USBManager::mInst=nullptr;

USBManager::USBManager(){
     NGLOG_DEBUG("=====USBManager::USBManager=====");   
}

size_t USBManager::getContent(const std::string&fname,char*buf,size_t bufsize){
     struct stat st;
     int rc=stat(fname.c_str(),&st);
     if(rc<0)return 0;
     FILE*f=fopen(fname.c_str(),"r");
     size_t rdsize=std::min(bufsize,(size_t)st.st_size);
     fread(buf,rdsize,1,f);
     fclose(f);
     buf[rdsize]=0;
     NGLOG_VERBOSE("%s  %s",fname.c_str(),buf);
     return st.st_size;
}

#define UEVENT_BUFFER_SIZE 2048 

static void USBMonitor(void*p){
    struct sockaddr_nl client;  
    struct timeval tv;  
    int  sockfd;  
    fd_set fds;  
    USBManager*usbmgr=(USBManager*)p;
    int buffersize = 1024;  
    sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);  
    memset(&client, 0, sizeof(client));  
    client.nl_family = AF_NETLINK;  
    client.nl_pid = getpid();  
    client.nl_groups = 1; /* receive broadcast message*/  
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));  
    bind(sockfd, (struct sockaddr*)&client, sizeof(client));
    while (1) {  
        char buf[UEVENT_BUFFER_SIZE] = { 0 };  
        FD_ZERO(&fds);  
        FD_SET(sockfd, &fds);  
        tv.tv_sec = 0;  
        tv.tv_usec = 100 * 1000;  
        int ret = select(sockfd + 1, &fds, NULL, NULL, &tv);  
        if(ret < 0)  
            continue;  
        if(!(ret > 0 && FD_ISSET(sockfd, &fds)))  
            continue;  
        /* receive data */  
        if(recv(sockfd, &buf, sizeof(buf), 0)>0){ 
            usbmgr->onRecvMessage(buf);
        }  
    }  
    close(sockfd);
}

USBManager&USBManager::getInstance(){
    if(nullptr==mInst){
       mInst=new USBManager();
    }
    return *mInst;
}
void USBManager::startMonitor(){
    static HANDLE threadid=0;
    NGLOG_DEBUG("");
    if(0==threadid)
        nglCreateThread(&threadid,0,0,USBMonitor,mInst);
}

void USBManager::onRecvMessage(const std::string&message){
    size_t pos=message.find("@");
    std::string action=message.substr(0,pos);
    std::string device=message.substr(pos+1);
    NGLOG_DEBUG("action:%s dev:%s",action.c_str(),device.c_str());
    /*if(action.compare("add")==0)//sys+device is the device node path
        onAddDevice(device);
    else
        onRemoveDevice(device);*/
    NetlinkEvent netevent;
    netevent.decode((char*)message.c_str(),message.length(),1);
    VolumeManager::Instance()->handleBlockEvent(&netevent);
}

static bool isBlockDevice(const std::string&dev){
    return dev.find("block")!=std::string::npos;
}

int USBManager::getDeviceInfo(const std::string&dev,BlockDevice&info){
    char buffer[256];
    int rc=getContent(dev+"/uevent",buffer,sizeof(buffer));
    if(rc<=0){
         return -1;
    }
    std::istrstream sin(buffer);
    std::string line;
    while(getline(sin,line)){
          size_t pos=line.find("=");
          if(std::string::npos==pos)continue;
          std::string key=line.substr(0,pos);
          std::string value=line.substr(pos+1);
          if(key.compare("DEVTYPE")==0)info.devtype=value;
          if(key.compare("DEVNAME")==0)info.name=value;
          //NGLOG_DEBUG("    %s:%s",key.c_str(),value.c_str());
    }
    return 0;
}

int USBManager::getMountPoint(const std::string&dev,std::string&path){
    int rc=-1;
    struct mntent *ent;
    FILE*f=fopen("/proc/mounts","r");//setmntent("/etc/mtab","r");//"/proc/mounts", "r");
    
    if(nullptr==f)return -1;
    ssize_t read;
    size_t len;
    char *line=NULL;
    while(getdelim(&line,&len,'\n',f)!=-1){//;//if(len>0)line[rc]=0;
         printf("%s",line);
    }printf("\r\n==================222\r\n");
    printf("====%s\r\n",line);
    //free(line);
    /*while(ent=getmntent(f)){
         NGLOG_DEBUG("name=%s  type:%s mountpoint:%s ",ent->mnt_fsname, ent->mnt_type,ent->mnt_dir);
         if(strstr(ent->mnt_fsname,dev.c_str())!=NULL){
             path=ent->mnt_dir;
             rc=0;
             break;
         }
    }*/
    //ent=getmntent(f);
    NGLOG_DEBUG("ent=%p",ent);
    endmntent(f);
    return rc;
}

void USBManager::onAddDevice(const std::string& dev){
    std::string devname="/sys"+dev;
    char buffer[256];
    std::string type;
    if(!isBlockDevice(dev))
         return;
    BlockDevice info;
    getDeviceInfo(devname,info);
    NGLOG_DEBUG("name:%s type:%s    %s\r\n",info.name.c_str(),info.devtype.c_str(),dev.c_str());
    getMountPoint(info.name,type);
    NGLOG_DEBUG("%s's mountpoint=%s",info.name.c_str(),type.c_str());
    /*getContent(devname+"/removable",buffer,sizeof(buffer)); 
    getContent(devname+"/size",buffer,sizeof(buffer)); 
    getContent(devname+"/uevent",buffer,sizeof(buffer)); */
}
void USBManager::onRemoveDevice(const std::string& dev ){
    std::string devname="/sys"+dev;
    if(!isBlockDevice(dev))return;

}

