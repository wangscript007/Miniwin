#ifndef __NET_SETTING_H__
#define __NET_SETTING_H__
int setNetworkIP(const char*sfname,const char*sipaddr);
int setNetworkMask(const char*sfname,const char*sipaddr);
int setNetworkBAddr(const char*sfname,const char*sipaddr);
int setNetworkMac(const char*sfname,const char*sipaddr);
int getNetworkGateway(const char*ifName,char *gateway);
int getNetworkInfo(const char*sfname,char*sipaddr,char*smask,char*sgetway,char*mac);
int setNetworkInfo(const char*sfname,char*sipaddr,char*smask,char*sgateway,char*smac);
int getNetworkInterface(std::vector<std::string>&nets);

#endif
