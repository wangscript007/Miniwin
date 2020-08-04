#include<stdio.h>
#include<va_types.h>
#include<va_ctrl.h>
#include<va_os.h>
#include<va_sc.h>
#include<va_dmx.h>
#include<va_xnvm.h>
#include<ngl_video.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include<pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <ngl_log.h>

NGL_MODULE(MAIN)

static DWORD CreateFilter(int pid,int tbid);

//(DWORD)flt,len,(BYTE*)section_data,NULL
static int GetPMTPids(BYTE*pmt,unsigned short *pids){
    int seclen=((pmt[1]&0x0F)<<8)|pmt[2];
    int cnt=0;
    BYTE*p=pmt+8;
    for(;p<pmt+seclen-1;p+=4){
        pids[0]=(p[0]<<8)|p[1]; //serviceid
        pids[1]=((p[2]&0x1F)<<8)|p[3]; //pmtpid
        if(pids[1]==0x10)continue;
        NGLOG_INFO("serviceid=0x%x/%d pmt_pid=0x%x/%d",pids[0],pids[0],pids[1],pids[1]);
        pids+=2;cnt++;
    }
    return cnt;
}
BYTE PMTSection[512];
static unsigned short pmtPids[64]={0,0};
static unsigned short elements[16]={0,0};
static int getElements(BYTE*pmt,UINT16 *pids){
    int deslen=((pmt[10]&0x0F)<<8)|pmt[11];
    int dlen,slen= (pmt[1]&0x0F)<<8|pmt[2];
    BYTE*pd=pmt+12+deslen;
    UINT16*pi=pids;
   
    {//pmt first loop program info descriptors
         BYTE*pi=pmt+12;
         int i,len=((pmt[10]&0x0F)<<8)|pmt[11];
         for(i=0;i<len;){
              int ecmpid=0;
              if(9==pi[0])ecmpid=(pi[i+4]&0x01F)<<8|pi[i+5];
              NGLOG_INFO("TAG:0x%x len=%d ecmpid=0x%x/%d",pi[i],pi[i+1],ecmpid,ecmpid);
              NGLOG_DUMP("DescriptorData",&pi[i],pi[i+1]+2);
              i+=pi[1]+2;
         }
    }
    for(;pd<pmt+slen-1;pi+=2){
       pi[0]=pd[0];
       pi[1]=(pd[1]&0x1F)<<8|pd[2];
       dlen=(pd[3]&0x0F)<<8|pd[4];
       pd+=dlen+5;
       NGLOG_INFO("\ttype=%d pid=%d",pi[0],pi[1]);
    }
    return pi-pids;
}
static INT SectionCBK(DWORD dwVaFilterHandle,UINT32 uiBufferLength,BYTE *pBuffer, void *pUserData)
{
    //NGLOG_DEBUG("SectionCBK filter=0x%x pBuffer=%p\r\n",dwVaFilterHandle,pBuffer );//tbid=%02X %02X %02X\r\n",pBuffer[0],pBuffer[1],pBuffer[2]);
    int i,prgcount;
    switch(pBuffer[0]){
    case 0: if(pmtPids[0]==0){
                 prgcount=GetPMTPids(pBuffer,pmtPids);
                 //CreateFilter(pmtPids[1],0x02);
            }
            //VA_DMX_StopSectionFilter(dwVaFilterHandle);
            break;//VA_CTRL_CatUpdated(0,uiBufferLength,pBuffer);break;
    case 1: VA_CTRL_CatUpdated(0,uiBufferLength,pBuffer);     
            VA_DMX_StopSectionFilter(dwVaFilterHandle);
            break;
    case 2: VA_CTRL_SwitchOffProgram(0);
            VA_CTRL_SwitchOnProgram(0,uiBufferLength,pBuffer);
            VA_DMX_StopSectionFilter(dwVaFilterHandle);
            prgcount=getElements(pBuffer,elements); 
            memcpy(PMTSection,pBuffer,uiBufferLength);
            //VA_CTRL_RemoveStream(0);
            for(i=0;i<prgcount;i+=2){
                VA_CTRL_AddStream(0,elements[i+1],elements[i+1],NULL);
            }
            nglAvPlay(0,elements[1],0,elements[3],0/*1AUI_DECA_STREAM_TYPE_MPEG2*/,elements[1]);
            break;
    }
}
static DWORD CreateFilter(int pid,int tbid){
    DWORD  hFilter;
    BYTE mask[8],value[8];
    mask[0]=0xFF;value[0]=tbid;
    hFilter=VA_DMX_AllocateSectionFilter(0,0,pid,SectionCBK);
    VA_DMX_SetSectionFilterParameters(hFilter,1,mask,value,eVA_DMX_Continuous);
    VA_DMX_StartSectionFilter(hFilter);
    return hFilter;
}
int ATOI(const char*p){
   int rc=0;
   while(*p++){
       if(!isdigit(*p))continue;
       rc=rc*10+((*p)-'0');
   }
   return rc;
}
UINT16 g_service_id;
void ProcessCommand(const char*command)
{
    UINT16 i,pid=0;
    INT sid=0;
    char*endptr;
    sid=ATOI(command);
    if(strstr(command,"play")!=NULL){
       DWORD hdl=nglPvrPlayerOpen(NULL);
       nglPvrPlayerPlay(hdl);
    }else if(strstr(command,"quit")!=NULL){
        exit(0);
    }
    if(sid==0)return;
    for(i=0;i<sizeof(pmtPids)/sizeof(UINT16);i+=2){
        if(pmtPids[i]==sid){
            pid=pmtPids[i+1];g_service_id=sid;
            NGLOG_INFO("Process Command cmd=%s sid=%d pid=%d",command,sid,pid);
            CreateFilter(pid,0x02);
            break;
        }
    }
}
static tVA_CTRL_AcsMode acsMode=eLIVE;

static void*ACSProc(void*p){
    int ret;
    tVA_CTRL_ConfigurationParameters param;
    memset(&param,0,sizeof(tVA_CTRL_ConfigurationParameters));
    param.uiNbAcs = kVA_SETUP_NBMAX_ACS ;//1
    param.uiNbSmartCard = kVA_SETUP_NBMAX_SC ;//1
    param.aAcs->stDemux.uiMaxNumberOfFilters = 16;
    param.aAcs->stDemux.uiMaxSizeOfFilter = 4096;
    param.aAcs->stDescrambler.uiMaxNumberOfChannels =4;
    //param.stStbInformation.
    NGLOG_DEBUG("VA_CTRL_Init calling...");
    ret = VA_CTRL_Init(&param);
    NGLOG_DEBUG("VA_CTRL_Init=%d",ret);
    NGLOG_DEBUG("VA_CTRL_Start...");
    for(int i=0;i<3;i++)VA_CTRL_OpenAcsInstance(i,i);
    CreateFilter(1,1);
    CreateFilter(0,0);
    VA_CTRL_Start();
    NGLOG_DEBUG("VA_CTRL_Start...end");
}

void recvSignal(int sig)  
{  
    NGLOG_ALERT("received signal %d !!!",sig);  
    exit(-1);
}  
int main(int argc,const char*argv[])
{
    int i;
    pthread_t tid;
    VA_SCHIP_GetChipsetId();
    nglLogParseModules(argc,argv);
    nglDscInit();
    nglDmxInit();
    nglAvInit();
    VA_DSCR_Init();
    if(argc>1)
      acsMode=(tVA_CTRL_AcsMode)atoi(argv[1]);
    pthread_create(&tid,NULL,ACSProc,NULL);
    while(1){sleep(1);}
}  
