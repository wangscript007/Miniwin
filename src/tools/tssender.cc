#include<string.h>
#include<sys/time.h>
#include <getopt.h>
#include <stdlib.h>
#include <rtpsession.h>
#include <rtpudpv4transmitter.h>
#include <rtpipv4address.h>
#include <rtpsessionparams.h>
#include <rtperrors.h>
#include <rtplibraryversion.h>
#include <rtppacket.h>
#include <iostream>
#include <sys/time.h>
#include <sys/stat.h>
#include <unordered_set>
#if HAVE_NCURSES
#include <ncurses.h>
#error ncuses=============1
#endif

#include <unistd.h>

std::unordered_set<unsigned short >pids;
using namespace jrtplib;

int getpmtpids(unsigned char*ts){
    unsigned char*p,*pd=ts+4;
    int len=0;
    int rc=0;;
    if(ts[3]&0x20)//adapt_field
        pd+=(*pd)+1;//(issection?1:0);
    if( ts[1]&0x40)//ts_unit_start
        pd+=(*pd)+1;
    if(ts[3]&0x10){
       len=188-(pd-ts);
       printf("len1=%d  tbid=%d\r\n",len,pd[0]);
       len=((pd[1]&0x0f)<<8)|pd[2];
       printf("len=%d\r\n",len);
       for(p=pd+8;p<pd+len-1;p+=4,rc++){
           unsigned short pid=((p[2]&0x1F)<<8)|p[3];
           pids.emplace(pid);printf("pmtpid:%d/%x\r\n",pid,pid);
       }
    }
    return rc;
}

int gessTSPackSize(FILE*f){
    int tssize=0;
    unsigned char tss[204*4];
    fread(tss,188,4,f);
    for(unsigned char*p=tss;p-tss<204;p++){
        if( (p[0]==0x47) && (p[188]==0x47) && (p[376]==0x47) )
        {tssize= 188;break;}
        if( (p[0]==0x47) && (p[204]==0x47) && (p[408]==0x47) )
        {tssize= 204;break;}
    }
    fseek(f,0,SEEK_SET);
    return tssize;
}
int main(int argc,char*argv[]){
    static const char *usage="Usage:%s [options] filename\r\n"
               "\t-h print help message\r\n"
               "\t-a send all pid data\r\n"
               "\t-d ipaddress [optional] default is 127.0.0.1\r\n";
    
    int opt;
    int allpid=0;  
    char iptoaddr[128];
    char fname[512];
    int scrrow,scrcol;
    int TSPACK_SIZE=188;
    unsigned char buffer[188*7]={1,2,3,4,5,6,7,8};
    int ibuffer=0;
    unsigned short dvb_pids[]={0,1,2,0x10,0x11,0x12,0x13,0x14};
    const char progress[]={'-','\\','|','/'};
    strcpy(iptoaddr,"127.0.0.1");
    pids.insert(dvb_pids,dvb_pids+8); 
    while((opt=getopt(argc,argv,"ahd:p:i"))!=-1){
       printf("%c:optind=%d %s\r\n",opt,optind,optarg);
       switch(opt){
       case 'a':allpid=1;break;
       case 'h':printf(usage,argv[0])  ; return 0;
       case 'd':strcpy(iptoaddr,optarg); break;
       case 'i':strcpy(fname,optarg); break;
       default:printf("%s\r\n",optarg);break;
       }
    }  
    strcpy(fname,argv[optind]);
    printf("filename=%s %s allpid=%d\r\n",fname,iptoaddr,allpid);

    FILE*f=fopen(fname,"rb");
    int status;
    int last_cursorpos=0;
    off_t filesize=0;
    off_t loopers=0;
    struct stat filestat;
    struct timeval start;
    int64_t packetsend=0;
    RTPSession sess;
    uint16_t portbase = 6666;  
    uint16_t destport = 6664;
 
    uint32_t destip=inet_addr(iptoaddr); 
    TSPACK_SIZE=gessTSPackSize(f);
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    
    destip = ntohl(destip);
    stat(fname,&filestat);
    filesize=filestat.st_size;
	
    sessparams.SetOwnTimestampUnit(1.0/10.0);		
	
    sessparams.SetAcceptOwnPackets(true);
    transparams.SetPortbase(portbase);

    status = sess.Create(sessparams,&transparams);	
    if(status)std::cout<<"ERROR:"<<status<<RTPGetErrorString(status)<<std::endl;
	
    RTPIPv4Address addr(destip,destport);
	
    sess.SetDefaultPayloadType(96);
    sess.SetDefaultMark(false);
    sess.SetDefaultTimestampIncrement(100);

    status = sess.AddDestination(addr);
    if(status)std::cout<<"ERROR:"<<status<<RTPGetErrorString(status)<<std::endl;
#if HAVE_NCURSES
    initscr();
    getmaxyx(stdscr,scrrow,scrcol);
    WINDOW*win=newwin (6,scrcol,scrrow-6,0);//height,width,ypos,xpos
    wrefresh (win);
#endif
    gettimeofday(&start,NULL);
    int ppp=filesize/sizeof(buffer)/800;
    while(true){
        struct timeval now;
        int64_t dur;
        fpos_t filepos;
        double percent;
        unsigned short pid;
        unsigned char *ts=buffer+ibuffer;
        fread(ts,TSPACK_SIZE,1,f);
        pid=((ts[1]&0x1F)<<8)|ts[2];
        fgetpos(f,&filepos);
        percent=(double)ftell(f)/filesize;
        if(pid==0&&pids.size()<=8) getpmtpids(ts);
        if(pids.count(pid)||allpid){
           ibuffer+=188;//
           if(ibuffer>=sizeof(buffer)){
               status = sess.SendPacket(buffer,sizeof(buffer));
               ibuffer=0;
               if(status)std::cout<<"ERROR:"<<status<<RTPGetErrorString(status)<<std::endl;
               RTPTime::Wait(RTPTime(0.001));//second
               packetsend+=7;
           }
        }
        gettimeofday(&now,NULL);
        dur=now.tv_sec*1000+now.tv_usec/1000-start.tv_sec*1000-start.tv_usec/1000;
        if(dur%10000==0&&dur>10000){
            dur/=1000;
        }
        if(feof(f)){
           fseek(f,0,SEEK_SET);
#if HAVE_NCURSES
           wclear(win);
           wrefresh (win);
#endif
        }
        if(loopers++%ppp==0){
            static int chindex=0;
            int curpos=percent*(scrcol-4);
#if HAVE_NCURSES
            wborder (win, '|', '|', '-', '-', '+', '+', '+', '+');
            mvwprintw(win,1,2,"TSSender:%s ppp=%d",fname,ppp);
#endif
            if(last_cursorpos!=curpos){
#if HAVE_NCURSES
                mvwaddch(win,3,2+last_cursorpos,'*');
#endif
                last_cursorpos=curpos;
                chindex=0;
            }else {
#if HAVE_NCURSES
                mvwaddch(win,3,2+curpos,progress[chindex]);
#endif
                chindex=(chindex+1)%4;
            }
#if HAVE_NCURSES
            mvwprintw(win,1,scrcol-25,"SPEED:%ld kbps %3.2f%%",(long)(packetsend*188*8/dur),100.*percent);
            wrefresh (win);
#endif
        }
    }
    sess.BYEDestroy(RTPTime(10,0),0,0);
#if HAVE_NCURSES
    endwin();
#endif
    return 0;
}
