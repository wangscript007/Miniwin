#ifndef __USB_DEVICE_MANAGER_H__
#define __USB_DEVICE_MANAGER_H__
#include<vector>
#include<string>
#include<map>

typedef struct{
    std::string name;
    size_t size;
}PARTITION;

class BlockDevice{
public:
   std::string nodepath;
   std::string name;
   int vid;
   int pid;
   std::string devtype;
   std::string mountpoint;
   std::vector<PARTITION>partitions;
   int removeable;
};

class USBManager{
private:
    int sockfd;
    static USBManager*mInst;
    std::map<std::string,BlockDevice*>devices;
    USBManager();
    size_t getContent(const std::string&fname,char*buf,size_t);
protected:
    int getDeviceInfo(const std::string&dev,BlockDevice&info);
    int getMountPoint(const std::string&dev,std::string&path);
public:
    static USBManager&getInstance();
    void startMonitor();
    
    void onRecvMessage(const std::string&);
    void onAddDevice(const std::string&);
    void onRemoveDevice(const std::string&);
};

#endif
