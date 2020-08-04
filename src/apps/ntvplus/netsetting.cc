#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/route.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/rtnetlink.h> 
#include <unistd.h>
#include <vector>
#include <string>
#include <ngl_types.h>
#include <ngl_log.h>
//ref:https://www.cnblogs.com/lidabo/p/5344740.html
NGL_MODULE(NET)

#define BUFSIZE 8192

struct route_info{
    u_int dstAddr;
    u_int srcAddr;
    u_int gateWay;
    char ifName[IF_NAMESIZE];
};

int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;
    do{
        //收到内核的应答
        if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0) {
            perror("SOCK READ: ");
            return -1;
        }
        nlHdr = (struct nlmsghdr *)bufPtr;
        //检查header是否有效
        if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR)){
            perror("Error in recieved packet");
            return -1;
        }
        if(nlHdr->nlmsg_type == NLMSG_DONE) {
            break;
        }else{
            bufPtr += readLen;
            msgLen += readLen;
        }
        if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) {
            break;
        }
    } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
    return msgLen;
}

//分析返回的路由信息
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway, const char *ifName)
{
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;
    struct in_addr dst;
    struct in_addr gate;

    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
    // If the route is not for AF_INET or does not belong to main routing table
    //then return.
    if((rtMsg->rtm_family != AF_INET) ){//|| (rtMsg->rtm_table != RT_TABLE_MAIN)){
        printf("%s:%d error %d,%d \r\n",__FUNCTION__,__LINE__,(rtMsg->rtm_family != AF_INET),(rtMsg->rtm_table != RT_TABLE_MAIN));
        return;
    }
    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);
    for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
        switch(rtAttr->rta_type) {
        case RTA_OIF:    if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);break;
        case RTA_GATEWAY:rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);            break;
        case RTA_PREFSRC:rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);            break;
        case RTA_DST:    rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);            break;
        }
    }
    dst.s_addr = rtInfo->dstAddr;
    printf("dst:%s\r\n",(char *)inet_ntoa(dst));
    if (strstr((char *)inet_ntoa(dst), "0.0.0.0")){
        if(strcmp(ifName,rtInfo->ifName)==0){
            gate.s_addr = rtInfo->gateWay;
            sprintf(gateway, "%s", (char *)inet_ntoa(gate));
        }
        gate.s_addr = rtInfo->srcAddr;
        gate.s_addr = rtInfo->dstAddr;
        printf("###  %s:%s  %s\r\n",ifName,rtInfo->ifName,(char *)inet_ntoa(gate));
    }
    return;
}

int getNetworkGateway(const char*ifName,char *gateway)
{
    struct nlmsghdr *nlMsg;
    struct rtmsg *rtMsg;
    struct route_info *rtInfo;
    char msgBuf[BUFSIZE];

    int sock, len, msgSeq = 0;

    if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0){
        perror("Socket Creation: ");
        return -1;
    }
    memset(msgBuf, 0, BUFSIZE);

    nlMsg = (struct nlmsghdr *)msgBuf;
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

    nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
    nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
    nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.

    if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
        NGLOG_ERROR("Write To Socket Failed…");
        return -1;
    }

    if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
        NGLOG_ERROR("Read From Socket Failed…");
        return -1;
    }
    rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
    for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
        memset(rtInfo, 0, sizeof(struct route_info));
        parseRoutes(nlMsg, rtInfo, gateway, ifName);
    }
    free(rtInfo);
    close(sock);
    return 0;
}

int getNetworkInfo(const char*sfname,char*sipaddr,char*smask,char*sgateway,char*smac) {
    struct sockaddr_in *sin;
    struct ifreq ifr;
    char ip[32],mac[32];
    char netmask[32];
    char broadcast[32];
    int socket_fd;
    if((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
         perror("socket");
         return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, sfname);
    memset(&sin, 0, sizeof(sin));
//获取IP地址
    if(ioctl(socket_fd, SIOCGIFADDR, &ifr) != -1){
        sin = (struct sockaddr_in *)&ifr.ifr_addr;
        strcpy(ip, inet_ntoa(sin->sin_addr));
        if(sipaddr)strcpy(sipaddr,ip);
        NGLOG_VERBOSE("IP address is %s", ip);
    }
/*获取广播地址
    if(ioctl(socket_fd, SIOCGIFBRDADDR, &ifr) != -1){
         sin = (struct sockaddr_in *)&ifr.ifr_broadaddr;
         strcpy(broadcast, inet_ntoa(sin->sin_addr));
         if(sbroadcast)strcpy(sbroadcast,broadcast);
         NGLOG_VERBOSE("Broadcast is %s", broadcast);
    }*/
//获取子网掩码
    if(ioctl(socket_fd, SIOCGIFNETMASK, &ifr) != -1){
        sin = (struct sockaddr_in *)&ifr.ifr_broadaddr;
        strcpy(netmask, inet_ntoa(sin->sin_addr));
        if(smask)strcpy(smask,netmask);
        NGLOG_VERBOSE("Net-mask is %s", netmask);
    }
//获取硬件MAC地址
    if(ioctl(socket_fd, SIOCGIFHWADDR, &ifr) != -1){
        sin = (struct sockaddr_in *)&ifr.ifr_netmask;
        unsigned char*b=(unsigned char*)ifr.ifr_netmask.sa_data;
        sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",b[0],b[1],b[2],b[3],b[4],b[5]);
        if(smask)strcpy(smac,mac);
        NGLOG_VERBOSE("Mac address is %s", mac);
    }
    close(socket_fd);
    char buff[256]={0};
    getNetworkGateway(sfname,buff);
    if(sgateway)strcpy(sgateway,buff);
    NGLOG_DEBUG("interface:%s getway: %s",sfname, buff);
    return 0;
}
int setNetworkIP(const char*sfname,const char*sipaddr) {
    struct sockaddr_in *sin;
    struct ifreq ifr;
    int rc,socket_fd;
    if((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
         perror("socket");
         return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, sfname);
    memset(&sin, 0, sizeof(sin));
//设置IP地址
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    inet_aton(sipaddr, &(sin->sin_addr));
    rc=ioctl( socket_fd, SIOCSIFADDR, &ifr);
    close(socket_fd);
    printf("%s:%d ioctrl=%d ipaddr=%s\r\n",__FUNCTION__,__LINE__,rc,sipaddr);
    return rc;
}
int setNetworkMask(const char*sfname,const char*smask){
    struct sockaddr_in *sin;
    struct ifreq ifr;
    int rc,socket_fd;
    if((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
         perror("socket");
         return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, sfname);
    memset(&sin, 0, sizeof(sin));
//设置IP地址
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    inet_aton(smask, &(sin->sin_addr));
    rc=ioctl( socket_fd,SIOCSIFNETMASK, &ifr);
    close(socket_fd);
    printf("%s:%d ioctrl=%d\r\n",__FUNCTION__,__LINE__,rc);
    return rc;
}
int setNetworkBAddr(const char*sfname,const char*saddr){
    struct sockaddr_in *sin;
    struct ifreq ifr;
    int rc,socket_fd;
    if((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
         perror("socket");
         return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, sfname);
    memset(&sin, 0, sizeof(sin));
//设置IP地址
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    inet_aton(saddr, &(sin->sin_addr));
    rc=ioctl( socket_fd,SIOCSIFBRDADDR, &ifr);
    close(socket_fd);
    printf("%s:%d ioctrl=%d\r\n",__FUNCTION__,__LINE__,rc);
    return rc;
}

int setNetworkMac(const char*sfname,const char*smac){
    struct sockaddr_in *sin;
    struct ifreq ifr;
    int rc,socket_fd;
    if((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
         perror("socket");
         return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, sfname);
    memset(&sin, 0, sizeof(sin));

    ifr.ifr_addr.sa_family = ARPHRD_ETHER;
    strncpy(ifr.ifr_name, (const char *)sfname, IFNAMSIZ - 1 );
    memcpy((unsigned char *)ifr.ifr_hwaddr.sa_data, smac, 6);
    
    rc=ioctl(socket_fd, SIOCSIFHWADDR, &ifr);
    close(socket_fd);
    printf("%s:%d ioctrl=%d\r\n",__FUNCTION__,__LINE__,rc);
}

int getNetworkInterface(std::vector<std::string>&nets){
    int i=0;
    int sockfd;
    struct ifconf ifconf;
    char buf[512];
    struct ifreq *ifreq;

    //初始化ifconf
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0){
        perror("socket");
        return -1;
    }  
    ioctl(sockfd, SIOCGIFCONF, &ifconf);    //获取所有接口信息

    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)buf;  
    for(i=(ifconf.ifc_len/sizeof(struct ifreq)); i>0; i--){
        // if(ifreq->ifr_flags == AF_INET){            //for ipv4
        NGLOG_VERBOSE("name = [%s]",ifreq->ifr_name);
        nets.insert(nets.begin(),std::string(ifreq->ifr_name));
        NGLOG_VERBOSE("local addr = [%s]", inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
        ifreq++;
        //  }
    }
    close(sockfd);
    return nets.size();
}
