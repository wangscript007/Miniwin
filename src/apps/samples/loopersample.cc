#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<windows.h>

class SockWindow:public Window{
private:
   int sender,receiver;
   int set_nonblocking(int sockfd) {
       int flag = fcntl(sockfd, F_GETFL, 0);
       if (flag < 0) {
           return -1;
       }
       if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) {
           return -1;
       }
       return 0;
   }
public:
   SockWindow(int x,int y,int w,int h):Window(x,y,w,h){}
   int initSocket(const char*ipaddr,int port){
       int s,len;
       struct sockaddr_in addr;
       int addr_len;

       sender=socket(AF_INET,SOCK_DGRAM,0);
       set_nonblocking(sender);
       len=sizeof(struct sockaddr_in);
       bzero(&addr,sizeof(addr));
       addr.sin_family=AF_INET;
       addr.sin_port=htons(port);
       addr.sin_addr.s_addr=htonl(INADDR_ANY);
       bind(sender,(const sockaddr*)&addr,sizeof(addr));

       receiver=socket(AF_INET,SOCK_DGRAM,0);
       set_nonblocking(receiver);
       len=sizeof(struct sockaddr_in);
       bzero(&addr,sizeof(addr));
       addr.sin_family=AF_INET;
       addr.sin_port=htons(port);
       addr.sin_addr.s_addr=inet_addr(ipaddr);
       return 0;
   }
};

int main(int argc,const char*argv[]){

    App app(argc,argv);
    SockWindow*w=new SockWindow(100,100,800,600);
    w->setLayout(new LinearLayout());
    w->addChildView(new TextView("HelloWorld",400,80));
    return app.exec();
}
