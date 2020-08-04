#include <ngl_types.h>
#include <ngl_dmx.h>
#include <ngl_log.h>
#include <ngl_os.h>
#include <ngl_misc.h>
#include <ngl_video.h>
#include <dvbepg.h>
#include <satellite.h>
#include <stdlib.h>
#include <windows.h>
#include <wchar.h>
#include <assert.h>
#include <time.h>
#include <pixman.h>
#include <teletextview.h>
#include <ccview.h>
#include <subtitleview.h>
#include <tvtestutils.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <dsmcc.h>
#include <unordered_map>
#include <bitset>

NGL_MODULE(Main);

static void ONTP_FINISHED(const TRANSPONDER*tp,int idx,int tpcount,void*userdata){
    if(tp==NULL){
       nglSetEvent((HANDLE)userdata);
    }
}

NGLTunerParam tp;
SEARCHNOTIFY notify;

void printusage(const char*name){
    printf("Usage:%s [options]\r\n",name);
    printf("   -h --help      :print help message\r\n");
    printf("   -s --search    :search programs\r\n");
    printf("   -p --parse     :parse PSI\r\n");
    printf("   -P --page     :teletext pageid\r\n");
    printf("   -v --verbose  :verbose message\r\n");
    printf("   -t --type<type>:which type must be ttx cc dvb default is ttx" );
}
static void ParseDSMCC(int pid);
static void ParseTS(const STREAMDB&ts,void*p);
static INT ParseService(const SERVICELOCATOR*loc,const DVBService*s,const BYTE*pmt,void*p);
int verbose=0;
typedef struct {
   FILE*fout;
   UINT groupsize;
   USHORT blocksize;
   std::unordered_map<int,DII::ModuleInfo>modules;
   std::unordered_map<std::string,FILE*>files;
   std::unordered_map<int,std::bitset<80000>>bits;
}DOWNLOAD;
static DOWNLOAD dsmccdld={nullptr,0};
int main(int argc,const char*argv[]){
    USHORT pmtpids[64];
    HANDLE handle=nglCreateEvent(0,true);
    struct option ttoptions[]={
       {"help"  ,no_argument,0,'h'},
       {"search",no_argument,0,'s'},
       {"type"  ,optional_argument,0,'t'},
       {"page"  ,optional_argument,0,'P'},
       {"data"  ,required_argument,0,'d'},
       {"parse"   ,no_argument,0,0},  
       {"pid"   ,required_argument,0,0},  
       {"verbose",no_argument,0,0},
       {"output",optional_argument,0,'o'},
       {nullptr,0,0,0}
    };
    App app(argc,argv,ttoptions);
    nglDmxInit();
    nglSysInit();
    nglTunerInit();
    nglDmxInit();
    DtvEpgInit();
    nglAvInit();

    dsmccdld.fout=fopen(app.getArg("output","./dsmcc.bin").c_str(),"wb+");

    if(app.hasArg("help")||argc==1){printusage(argv[0]);return 0;}

    verbose=app.hasArg("verbose");

    std::string file=app.getArg("data","dvb_programs.dat");
    if(app.hasArg("search")){
       tvutils::GetTuningParams(tp);
       nglTunerLock(0,&tp);
       memset(&notify,0,sizeof(notify));
       notify.FINISHTP=ONTP_FINISHED;
       handle=nglCreateEvent(0,1);
       notify.userdata=handle;

       DtvSearch(&tp,1,&notify);
       nglWaitEvent(handle,5000);
       DtvSaveProgramsData(file.c_str());
       NGLOG_DEBUG("======search finished========");
    }else{
       DtvLoadProgramsData(file.c_str());
    }
    FavInit(NULL);

    if(app.hasArg("parse")){
        DtvEnumTS(ParseTS,nullptr);
    }
    if(app.hasArg("pid")){
        ParseDSMCC(app.getArgAsInt("pid",2003));
    }

    Window*w=new Window(0,0,1280,720);
    std::string type=app.getArg("type","ttx");
    int pid=app.getArgAsInt("pid",0x194);
    NGLOG_DEBUG("pid=%d/%x type=%s",pid,pid,type.c_str()); 

    if(type.compare("ttx")==0){
        VBIView*tv=new TeletextView(800,600);
        w->addChildView(tv);
        tv->start(pid,false);
        tv->requestPage(app.getArgAsInt("page",0x100));
    }else if(type.compare("cc")==0){
        VBIView*tv=new CCView(800,600);
        w->addChildView(tv);
        tv->start(pid,false);
        tv->requestPage(app.getArgAsInt("page",0x100));
    }else if(type.compare("dvb")==0){
        SubtitleView*tv=new SubtitleView(1280,720);    
        w->addChildView(tv);
        tv->decode(pid,2,2);
    }else exit(0);
    app.exec();
}

static INT ParseService(const SERVICELOCATOR*loc,const DVBService*s,const BYTE*section,void*p){
    NGLOG_INFO("Service %d.%d.%d PMT.sections=%p[%02x]",loc->netid,loc->tsid,loc->sid,section,(section?section[0]:0xFF));
    PMT pmt(section,false);
    ELEMENTSTREAM es[32];
    TeletextItem txts[32];
    SubtitleItem subs[32];
    NGLOG_DEBUG("\r\n===========Parse PMT(sid:%d)============",pmt.getProgramNumber());
    int cnt=pmt.getElements(es,true);
    int txtcnt=pmt.getTeletexts(txts);
    for(int i=0;i<cnt;i++){
        NGLOG_DEBUG("Element[%d]:type[%2d]%d/0x%x",i,es[i].stream_type,es[i].pid,es[i].pid);
        es[i].dump(verbose);
    }
    NGLOG_DEBUG_IF(txtcnt,"======%d Teletxts ======",txtcnt);
    for(int i=0;i<txtcnt;i++)
       NGLOG_DEBUG("\t[%d]pid %d/0x%x type:%d mag:%d page:%x",i,txts[i].pid,txts[i].pid,
           txts[i].type,txts[i].magazine,txts[i].page_number);
    int subcnt=pmt.getSubtitles(subs);
    NGLOG_DEBUG_IF(subcnt,"====== %d subtitles=====",subcnt);
    for(int i=0;i<subcnt;i++)
       NGLOG_DEBUG("\t[%d]pid pageid=%d/%d",i,subs[i].pid,subs[i].composition_page_id,subs[i].ancillary_page_id); 
    return 1;
}

static void ParseTS(const STREAMDB&ts,void*p){
    DtvEnumTSService(ts,ParseService,p);
}

static void FilterCBK(HANDLE dwVaFilterHandle,const BYTE *pBuffer,UINT uiBufferLength, void *pUserData){ 
    DSMCC dsmcc(pBuffer,false);
    DSI dsi(pBuffer,false);
    DII dii(pBuffer,false);
    DDB ddb(pBuffer,false);
    DSMCC::Header hdr;
    switch(dsmcc.getMessageId()){
    case DSMCC::DownloadServerInitiate:
         {
            DSI::GroupInfoIndication infos[8];
            int count=dsi.getGroupCount();
            NGLOG_DEBUG("DSI.groupcount=%d transactionid=%x",count,dsi.getTransactionId());
            dsi.getGroups(infos);
            for(int i=0;i<count;i++){
                DSI::GroupInfoIndication *g=infos+i;
                dsmccdld.groupsize=g->GroupSize;
                NGLOG_DEBUG("\tGroup[%x].size=%d infolen=%d info=%s",g->GroupId,g->GroupSize,g->getLength(),g->getName().c_str());
            }
         }break;
    case DSMCC::DownloadInfoIndication:
         {
            DII::ModuleInfo infos[8];
            int count=dii.getModuleCount();
            NGLOG_DEBUG("DII.moduleCount=%d blocksize=%d transactionid=%x",count,dii.getBlockSize(),dii.getTransactionId());
            dii.getModules(infos);
            dsmccdld.blocksize=dii.getBlockSize();
            for(int i=0;i<count;i++){
               DII::ModuleInfo*m=infos+i;
               dsmccdld.modules[m->moduleId]=*m;
               dsmccdld.modules[m->moduleId].setDescriptor(*m);
               NGLOG_INFO("\tModule[%d].Size=%d pos=%d infolen=%d %s",m->moduleId,m->moduleSize,m->position,m->getLength(),m->getName().c_str());
               NGLOG_DUMP("DES:",*m,m->getLength());
            } 
         }break;
    case DSMCC::DownloadDataBlock:
         if(dsmccdld.blocksize){
            BYTE buf[4096];
            USHORT moduleid=ddb.extTableId();
            int bitindex=dsmccdld.modules[moduleid].position*256+ddb.getBlockNumber();
            std::string name=dsmccdld.modules[moduleid].getName();
            FILE*fout=dsmccdld.files[name];
            //NGLOG_DEBUG("module[%d]%s deslen=%d",moduleid,name.c_str(),dsmccdld.modules[moduleid].getLength());
            if((fout==nullptr)&&(name.empty()==false))
               dsmccdld.files[name]=fout=fopen(name.c_str(),"wb+");
            INT blksize=ddb.getBlockData(buf);
            NGLOG_VERBOSE("DDB downloadid=%x module[%d] Block[%d].Size=%d blocks=%d modindex=%d fout=%p[%s]",ddb.getTransactionId(),
               ddb.extTableId(),ddb.getBlockNumber(),blksize,dsmccdld.bits[moduleid].count(),bitindex,fout,name.c_str());
            if(fout&&dsmccdld.bits[moduleid].test(bitindex)==0){ 
               dsmccdld.bits[moduleid].set(bitindex);
               fseek(fout,bitindex*dsmccdld.blocksize,SEEK_SET);
               fwrite(buf,blksize,1,fout);
            }
            if(0&&dsmccdld.groupsize/dsmccdld.blocksize+1==dsmccdld.bits[moduleid].count()){
                fflush(fout);
                fclose(fout);
                dsmccdld.files[name]=nullptr;
            }
         }
         break;
    case DSMCC::DownloadCancel:
         break;
    default:break;
    }
}
static void ParseDSMCC(int pid){
    BYTE mask[8],value[8];
    HANDLE flt =nglAllocateSectionFilter(0,pid,FilterCBK,nullptr,DMX_SECTION);
    mask[0]=0xF8;
    value[0]=TBID_DSI;/*DSI/DII-->0x3B DDB-->0x3C*/
    nglSetSectionFilterParameters(flt,mask,value,1);
    nglStartSectionFilter(flt);
}
