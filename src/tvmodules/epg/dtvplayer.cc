#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_os.h>
#include <ngl_msgq.h>
#include <ngl_video.h>
#include <si_table.h>
#include <dvbepg.h>
#include <ctype.h> 
#include <ngl_dmx.h>

NGL_MODULE(DTVPLAYER);

#define MSG_PLAY         1
#define MSG_PMTUPDATE    2
#define MSG_SWITCH_TRACK 3

typedef struct{
    UINT msgid;
    UINT lParam;
    SERVICELOCATOR loc;
    char lan[3];
}MSGPLAY;

typedef struct {
   DTV_NOTIFY fun;
   void*userdata;
}DTVNOTIFY;

static HANDLE msgQPlayer=0;
static SERVICELOCATOR cur_svc;
static std::vector<DTVNOTIFY>gNotifies;

extern void DtvNotify(UINT,const SERVICELOCATOR*,DWORD wp,ULONG lp);
extern HANDLE CreateFilter(USHORT pid,FILTER_NOTIFY cbk,void*param,bool start,int masklen,...);

static INT PlayService(BYTE*pmtbuff,const char*lan){
    USHORT pcr;
    INT vi=-1,ai=-1;
    PMT pmt(pmtbuff,false);
    ELEMENTSTREAM es[32];
    INT cnt=pmt.getElements(es);
    pcr=pmt.pcrPid();
    for(int i=0;i<cnt;i++){
       char*pc=es[i].iso639lan;
       NGLOG_DEBUG("\t pid=%d type=%d %s",es[i].pid,es[i].getType(),pc);
       switch(es[i].getCategory()){
       case ST_VIDEO:vi=i;break;
       case ST_AUDIO:
           if(-1==ai)
               ai=i;
           if((ai>=0)&&lan[0]&&(0==memcmp(es[i].iso639lan,lan,3)))ai=i;
           break;
       default:break;
       }
    }
    if(vi<0&&ai<0)return -1;
    else if(vi<0) nglAvPlay(0,0x1FFF,DECV_INVALID,es[ai].pid,es[ai].getType(),pcr);
    else if(ai<0) nglAvPlay(0,es[vi].pid,es[vi].getType(),0x1FFF,DECA_INVALID,pcr);
    else          nglAvPlay(0,es[vi].pid,es[vi].getType(),es[ai].pid,es[ai].getType(),pcr);
}

static void SectionMonitor(HANDLE filter,const BYTE *Buffer,UINT BufferLength, void *UserData){
     PMT p1((BYTE*)UserData,false);
     PMT p2(Buffer,false);
     if( (p1.tableId()==p2.tableId()) &&((p1.version()!=p2.version())||(p1.crc32()!=p2.crc32()))){
         memcpy((BYTE*)UserData,Buffer,BufferLength);
         if(Buffer[0]==TBID_PMT){
             MSGPLAY msg={MSG_PMTUPDATE};
             nglMsgQSend(msgQPlayer,&msg,sizeof(MSGPLAY),1000);
             DtvNotify(MSG_PMT_CHANGE,&cur_svc,BufferLength,(ULONG)Buffer);
         }else
             DtvNotify(MSG_CAT_CHANGE,&cur_svc,BufferLength,(ULONG)Buffer);
     }
}

static void PlayProc(void*param){
    MSGPLAY msg;//0xFFFF  is an invalid serviceid
    HANDLE flt_pmt=0;
    HANDLE flt_cat=0;
    BYTE PMT[1024];
    BYTE CAT[1024];
    int pmtpid;
    char lan[4];
    flt_cat=CreateFilter(PID_CAT,SectionMonitor,CAT,true,1,0xFF01);
    while(1){
        if(NGL_OK!=nglMsgQReceive(msgQPlayer,&msg,sizeof(MSGPLAY),1000))
            continue;
         switch(msg.msgid){
         case MSG_PLAY:
            if(msg.loc.sid!=cur_svc.sid){
                memcpy(lan,msg.lan,3);
                DtvTuneByService(&msg.loc);
                DtvGetServicePmt(&msg.loc,PMT);
                pmtpid=msg.lParam;
                if(flt_pmt){
                    nglStopSectionFilter(flt_pmt);
                    nglFreeSectionFilter(flt_pmt);
                }
                cur_svc=msg.loc; 
                NGLOG_INFO("PLAY %d.%d.%d %s%c%c%c",cur_svc.netid,cur_svc.tsid,cur_svc.sid,(lan[0]?"lan=":""),lan[0],lan[1],lan[2]);
                flt_pmt=CreateFilter((USHORT)pmtpid,SectionMonitor,PMT,true,3,0xFF02,(0xFF00|(cur_svc.sid>>8)),(0xFF00|(cur_svc.sid&0xFF)));
                PlayService(PMT,msg.lan);
                DtvNotify(MSG_SERVICE_CHANGE,&cur_svc,0,0);
            }break;
        case MSG_PMTUPDATE:
            NGLOG_DEBUG("PMTUPDATE %d.%d.%d",cur_svc.netid,cur_svc.tsid,cur_svc.sid);
            UpdateStreamData(&cur_svc,PMT);
            PlayService(PMT,lan);
            break;
        case MSG_SWITCH_TRACK:
            break;
        default:break;
        }
    }
}

static void Init(){
    if(0==msgQPlayer){
        HANDLE thid;
        memset(&cur_svc,0,sizeof(cur_svc));
        msgQPlayer=nglMsgQCreate(10,sizeof(MSGPLAY));
        nglCreateThread(&thid,0,4096,PlayProc,NULL);
    }
}

void DtvGetCurrentService(SERVICELOCATOR*sloc){
    if(sloc)*sloc=cur_svc;
}

INT DtvPlay(SERVICELOCATOR*loc,const char*lan){
    MSGPLAY msg;
    INT pmtpid=PID_INVALID;
    NGASSERT(loc);
    NGLOG_VERBOSE("PLAY %d.%d.%d",loc->netid,loc->tsid,loc->sid);
    DtvGetServiceItem(loc,SKI_PMTPID,&pmtpid);
    msg.msgid=MSG_PLAY;
    msg.lParam=pmtpid;
    msg.loc=*loc;
    memset(msg.lan,0,3);
    if(lan)memcpy(msg.lan,lan,3);
    Init();
    nglMsgQSend(msgQPlayer,&msg,sizeof(MSGPLAY),100);
}

DWORD DtvRegisterNotify(DTV_NOTIFY notify,void*userdata){
     DTVNOTIFY n={notify,userdata};
     for(int i=0;i<gNotifies.size();i++){
          if(nullptr==gNotifies[i].fun){
               gNotifies[i].fun=notify;
               gNotifies[i].userdata=userdata;
               return i;
          }
     }
     gNotifies.push_back(n);
     NGLOG_VERBOSE("notify=%p",notify);
     return gNotifies.size()-1;
}

void DtvUnregisterNotify(DWORD hdl){
     if(hdl<gNotifies.size()){
          gNotifies[hdl].fun=nullptr;
          gNotifies[hdl].userdata=nullptr;
     }
}

void DtvNotify(UINT msg,const SERVICELOCATOR*svc,DWORD wp,ULONG lp){
     for(auto n=gNotifies.rbegin();n!=gNotifies.rend();n++){
          if((*n).fun!=nullptr)
            (*n).fun(msg,svc,wp,lp,(*n).userdata);
     }
}

