#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<stdarg.h>
extern "C"{
#include <va_types.h>
#include <va_ctrl.h>
#include <va_os.h>
#include <va_sc.h>
#include <va_dmx.h>
#include <va_xnvm.h>
#include <va_ui.h>
DWORD VA_DSCR_Init();
}
#include <ngl_video.h>
#include <ngl_log.h>
#include <ngl_tuner.h>
#include <ngl_pvr.h>
#include <ngl_dmx.h>
#include <dvbepg.h>
#include <windows.h>
#include <CAEventSource.h>
NGL_MODULE(VA2NGL)

static SERVICELOCATOR lastplayed;
static HANDLE flt_pmt=0;
static HANDLE flt_cat=0;
static BYTE CATSection[1024];
static BYTE PMTSection[1024];
static UINT num_elements=0;
static ELEMENTSTREAM elements[16];
static  void VA_UIEvent(tVA_UI_Event *evt);

static void*ACSProc(void*p){
    int i,ret;
    const static BYTE serial[kVA_SERIAL_NUMBER_SIZE]={0x00,0x00,0x00,0x07,0x10,0x00,0x90,0x61,0x92,0x40,0x00,0x02};
    tVA_CTRL_ConfigurationParameters param;
    ViaccessEventSource via_events;
    memset(&param,0,sizeof(tVA_CTRL_ConfigurationParameters));
    memcpy(param.stStbInformation.stBoxIdentity,(void*)serial,kVA_SERIAL_NUMBER_SIZE);
    param.stStbInformation.wConstructorIdentifier=0x13C;//0x11B;
    param.stStbInformation.wModelIdentifier=0x01;
    param.uiNbAcs = kVA_SETUP_NBMAX_ACS ;//1
    param.uiNbSmartCard = kVA_SETUP_NBMAX_SC ;//1
    param.aAcs->stDemux.uiMaxNumberOfFilters = 16;
    param.aAcs->stDemux.uiMaxSizeOfFilter = 4096;
    param.aAcs->stDescrambler.uiMaxNumberOfChannels =4;
    NGLOG_DEBUG("VA_CTRL_Init calling...");
    VA_DSCR_Init();
    ret = VA_CTRL_Init(&param);
    NGLOG_DEBUG("VA_CTRL_Init=%d",ret);
    NGLOG_DEBUG("VA_CTRL_Start...");
    App::getInstance().addEventSource(&via_events,[](EventSource&e)->bool{
            tVA_UI_Event cae; 
            if(((ViaccessEventSource&)e).getEvent(cae)){
                VA_UIEvent(&cae);
            }
            return true;
        });
    VA_UI_AddEventListener([](tVA_UI_Event*e){
             ((ViaccessEventSource*)e->pUserArg)->addEvent(*e);
        },&via_events);
    for(i=0;i<3;i++)
        VA_CTRL_OpenAcsInstance(i,(tVA_CTRL_AcsMode)i);

    VA_CTRL_Start();
    NGLOG_DEBUG("VA_CTRL_Start...end");
}

static INT ACSStreamNOTIFY(DWORD dwStbStreamHandle, tVA_CTRL_StreamNotificationType eStreamNotificationType, tVA_CTRL_StreamNotificationInfo uStreamNotificationInfo){
    return kVA_OK;
}
static INT ACSHasStream(ELEMENTSTREAM*es,unsigned int numes,int pid){
    for(unsigned int i=0;i<numes;i++){
         if(es[i].pid==pid)return 1;
    }
    return 0;
}

static void SwitchProgram(BYTE*pmtbuf,UINT pmtlen){
    unsigned int i,rc;
    unsigned short num_new_elements;
    ELEMENTSTREAM new_elements[16];
    PMT pmt(pmtbuf,FALSE);
    PMT pmt0(PMTSection,FALSE);
    num_new_elements=pmt.getElements(new_elements,false);

    if(pmt0.getProgramNumber()==pmt.getProgramNumber()){//update program
       rc=VA_CTRL_PmtUpdated(0,pmtlen,pmtbuf);
       NGLOG_DEBUG("VA_CTRL_PmtUpdated=%d serviceid=%d pidcount=%d/%d",rc,pmt.getProgramNumber(),num_elements,num_new_elements);
       for(i=0;(i<num_elements);i++){//remove elements which not contained in new pmt
          if(ACSHasStream(new_elements,num_new_elements,elements[i].pid)==0){
             VA_CTRL_RemoveStream(elements[i].pid);
             NGLOG_DEBUG("VA_CTRL_RemoveStream[%d] %d",i,elements[i].pid);
          }else{
             VA_CTRL_UpdateStream(elements[i].pid,elements[i].pid);
             NGLOG_DEBUG("VA_CTRL_UpdateStream[%d] %d",i,elements[i].pid);
          }
       }
       for(i=0;i<num_new_elements;i++){
          if(ACSHasStream(elements,num_elements,new_elements[i].pid)==0){
             VA_CTRL_AddStream(0,new_elements[i].pid,new_elements[i].pid,ACSStreamNOTIFY);
             NGLOG_DEBUG("VA_CTRL_AddStream[%d] %d",i,new_elements[i].pid);
          }
       }
    }else{//switch program
       rc=VA_CTRL_SwitchOnProgram(0,pmtlen,pmtbuf);
       NGLOG_DEBUG("VA_CTRL_SwitchOnProgram=%d elements=%d/%d pmt=%p pmtlen=%d service=%d/%d",rc,num_elements,num_new_elements,
              pmt,pmtlen, pmt0.getProgramNumber(),pmt.getProgramNumber());
       for(i=0;i<num_new_elements;i++){//add new elements(of new pmt) which not contain in old pmt
          VA_CTRL_AddStream(0,new_elements[i].pid,new_elements[i].pid,ACSStreamNOTIFY);
          NGLOG_DEBUG("VA_CTRL_AddStream %d",new_elements[i].pid);
       }
    }
    memcpy(elements,new_elements,sizeof(new_elements));
    num_elements=num_new_elements;
}

static void SectionCBK(HANDLE dwVaFilterHandle,const BYTE *Buffer, unsigned int uiBufferLength,void *pUserData){
    PSITable si(Buffer,0);
    PSITable cat(CATSection,0);
    PSITable pmt(PMTSection,0);
    switch(Buffer[0]){
    case 1:/*CAT*/
         if(cat.version()!=si.version()){
            NGLOG_DUMP("CAT",Buffer,32);
            VA_CTRL_CatUpdated(0,uiBufferLength+3,(BYTE*)Buffer);
            memcpy(CATSection,Buffer,uiBufferLength);
         }
         break;
    case 2:/*PMT*/
         if(pmt.version()!=si.version()){
             NGLOG_DUMP("PMT",Buffer,32);
             SwitchProgram((BYTE*)Buffer,uiBufferLength+3);
             memcpy(PMTSection,Buffer,uiBufferLength);
         }
         break;
    default:break;
    }
}

static HANDLE CreateFilter(int pid,int num,...){
    va_list ap;
    HANDLE  hFilter;
    BYTE mask[16],value[16];
    int i,idx;
    char buffer[64];
    bzero(mask,sizeof(mask));
    bzero(value,sizeof(mask));
    bzero(buffer,sizeof(buffer));
    va_start(ap,num);
    for(i=0,idx=0;i<num;i++){
        mask[idx]=0xFF;
        value[idx]=(BYTE)va_arg(ap,int);
        idx+=((i==0)*2+1);
    }
    va_end(ap);
    for(i=0;i<idx;i++)sprintf(buffer+i*3,"%02x,",value[i]);
    if(num>1)num+=2;
    hFilter=nglAllocateSectionFilter(0,pid,SectionCBK,NULL,DMX_SECTION);
    nglSetSectionFilterParameters(hFilter,mask,value,num);//0-onshort ,1-eVA_DMX_Continuous);
    nglStartSectionFilter(hFilter);
    return hFilter;
}

static void CANOTIFY(UINT msg,const SERVICELOCATOR*svc,DWORD wp,ULONG lp,void*userdata){
    switch(msg){
    case MSG_SERVICE_CHANGE:
        {
            int pmtpid=DtvGetServerPmtPid(svc);
            nglStopSectionFilter(flt_pmt);
            nglFreeSectionFilter(flt_pmt);
            for(int i=0;i<num_elements;i++){
               VA_CTRL_RemoveStream(elements[i].pid);
               NGLOG_DEBUG("VA_CTRL_RemoveStream %d",elements[i].pid);
            }
            VA_CTRL_SwitchOffProgram(0);
            if(svc->netid!=lastplayed.netid||svc->tsid!=lastplayed.tsid||(svc->tpid!=lastplayed.tpid)){
                VA_CTRL_TsChanged(0);
                NGLOG_DEBUG("VA_CTRL_TsChanged");
                nglStopSectionFilter(flt_cat);
                nglFreeSectionFilter(flt_cat);
                flt_cat=CreateFilter(1,1,1);
            }
            flt_pmt=CreateFilter(pmtpid,3,0x02,(svc->sid>>8),(svc->sid&0xFF));
        }break;
    case MSG_PMT_CHANGE:
    case MSG_CAT_CHANGE:
    default:break;
    }
}

void  StartCA(){
    pthread_t tid;
    pthread_create(&tid,NULL,ACSProc,NULL);
    DtvRegisterNotify(CANOTIFY,NULL);
}

extern "C" INT GetCurrentServiceEcmPids(USHORT*espids){
    INT i,j,cnt,nes;
    USHORT*ppid=espids;
    USHORT tsecmpid=0x1FFF,tscaid=0xFFFF;
    BYTE buffer[1024];
    SERVICELOCATOR svc;
    ELEMENTSTREAM es[32];
    USHORT caids[16],ecmpids[16];
    PMT pmt(buffer,false);
    DtvGetCurrentService(&svc);
    DtvGetServicePmt(&svc,buffer);
    cnt=pmt.getCAECM(caids,ecmpids);
    for(i=0;i<cnt;i++){
         if(caids[i]==0x500){
             tscaid=caids[i];
             tsecmpid=ecmpids[i];
             break;
         }
    }
    nes=pmt.getElements(es,false);
    for(i=0;i<nes;i++){
        USHORT escaid=0xFFFF,esecmpid=0x1FFF;
        cnt=es[i].getCAECM(caids,ecmpids);
        if(tsecmpid!=0x1FFF||tscaid!=0xFFFF){//ts scrambled
            escaid=tscaid;esecmpid=tscaid;
        }
        if(cnt){
            escaid=caids[0];esecmpid=ecmpids[0];
        }
        if(escaid==tscaid||esecmpid==tscaid)
            *ppid++=es[i].pid; 
    }
    return ppid-espids;
}

void VA_ShowMessage(const std::string&txt){
    ToastWindow::makeWindow([&]()->ToastWindow*{
           ToastWindow*w=new ToastWindow(680,120);
           w->setLayout(new LinearLayout())->setPaddings(2);
           w->setFlag(View::Attr::ATTR_FOCUSABLE);
           TextView*t=new TextView(txt,670,68);
           t->setMultiLine(true);
           w->addChildView(t);
           Button*btn=new Button("OK",100,32);
           w->addChildView(btn);
           w->addChildView(new Button("Cancel",100,32));
           w->setPos(300,100);
           return w;
       });
}

#define CASE(x) case x :{NGLOG_DEBUG("%s 0x%x",#x,x);
#define ENDCASE break;}

static void VA_UI_RequestCBK(struct _stVA_UI_Request *req){
    switch(req->stRequestHeader.eType){
    CASE(eVA_UI_GET_VERSION_NAME_REQUEST);
         NGLOG_DEBUG("VersionName:[%s]",req->uRequest.stGetVersionNameRequest.pVersionName);
         ENDCASE;
    CASE(eVA_UI_GET_SMARTCARD_INFO_REQUEST);
         NGLOG_DEBUG("SmartcardInfo:todo");//,req->uRequest.stGetSmartcardInfoRequest.);
         ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_OK_EVENT);ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_EVENT);ENDCASE;
    CASE(eVA_UI_SET_TERMINAL_ACTIVATION_CODE_REQUEST);
         
         ENDCASE;//for SC1901
    default:NGLOG_DEBUG("todo:");break;
    }
}
static  void VA_UIEvent(tVA_UI_Event *evt){
    ViaccessEventSource*cas=(ViaccessEventSource*)evt->pUserArg;
    switch(evt->eEventType){//XXX_KO_EVENT will close XXX_OK_EVENT's message
    CASE(eVA_UI_DESCRAMBLING_OK_EVENT);         
        Toast::makeText("Stream descrambled");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_OK_RIGHT_OK_EVENT);
        Toast::makeText("Stream descrambled");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_OK_PREVIEW_EVENT); 
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_EVENT); 
        if(cas->svcstate==CASVC_INIT){
            cas->svcstate=CASVC_SCRAMBLED;
            VA_ShowMessage("program is scrambled");
        }
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_NO_RIGHT_EVENT);
        VA_ShowMessage("You do not have the rights to access the <program>");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_MISSING_KEY_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_IPPV_OFFER_EVENT); 
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED_EVENT);
        Toast::makeText("Purchase is not possible for the <Program>");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE_EVENT);
        cas->svcstate=CASVC_INIT;//Toast::makeText("Access to this <Program> has been forbidden by the operator.");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT_EVENT);
        Toast::makeText("You do not have sufficient credit to purchase this <Program>");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED_EVENT);
        Toast::makeText("You cannot view this <Program> because the viewing date has expired.");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED_EVENT);
        Toast::makeText("You cannot view this <specific text> because the maximum number of playbacks has been reached.");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH_EVENT);
        Toast::makeText("Terminal configuration is not accurate.\nPlease contact your operator.");
        ENDCASE;
    CASE(eVA_UI_DESCRAMBLING_KO_APPLET_MISSING_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_FREE_TO_AIR_EVENT);
        Toast::makeText("this event cancels messages about the smartcard status, until the next rightrestricting message");
        ENDCASE;
    CASE(eVA_UI_VIACCESS_NOT_APPLICABLE_EVENT);
        Toast::makeText("Not a VO scrambled program.");
        ENDCASE;
    CASE(eVA_UI_VIACCESS_SMARTCARD_NOT_FOUND_EVENT);
        Toast::makeText("Please insert the smartcard to access the program.");
        ENDCASE;
    CASE(eVA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED_EVENT);
        VA_ShowMessage("The terminal is not activated.\nPlease contact your operator.");
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_INSERTED_EVENT);
        Toast::makeText("Smart card inserted");
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_EXTRACTED_EVENT);
        Toast::makeText("Smart card extracted");
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_READY_EVENT);
        Toast::makeText("Smart card ready");
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_NEEDED_EVENT);
        Toast::makeText("Insert smart card please.");
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_FAILED_EVENT);
        Toast::makeText("Smart card error"); 
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_NOT_ACCEPTED_EVENT);
        Toast::makeText("invalid smart card");
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_INSUFFICIENT_MEMORY_EVENT);
        Toast::makeText("Smartcard memory is full.");
        ENDCASE;
    CASE(eVA_UI_SMARTCARD_INVALIDATED_EVENT);
        Toast::makeText("The smartcard has been invalidated by the operator.\nPlease contact your operator.");
        ENDCASE;
    CASE(eVA_UI_NEW_OPERATOR_MESSAGE_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_NEW_OPERATOR_DATA_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_CAMLOCK_ACTIVATED_OK_EVENT);
        Toast::makeText("");//close eVA_UI_CAMLOCK_ACTIVATED_KO_EVENT's dialog
        ENDCASE;//UC04
    CASE(eVA_UI_CAMLOCK_ACTIVATED_KO_EVENT);
        Toast::makeText("The inserted smartcard is not compliant with this STB!\nplease contact your operator.");
        ENDCASE;
    CASE(eVA_UI_CAMLOCK_ACTIVATED_NO_CARD_EVENT);
        Toast::makeText("Please insert your smartcard.");
        ENDCASE;
    CASE(eVA_UI_CAMLOCK_NOT_ACTIVATED_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_CAMLOCK_DISABLED_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_START_SECURITY_RESTRICTION_EVENT);
        Toast::makeText("A security restriction has been activate on this STB!\nPlease contact your operator");
        ENDCASE;
    CASE(eVA_UI_STOP_SECURITY_RESTRICTION_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_PAIRING_MISMATCH_EVENT);
        Toast::makeText("Wrong Card inserted. Please contact your operator.");
        ENDCASE;
    CASE(eVA_UI_OVERDRAFT_REACHED_EVENT);
        Toast::makeText("You have exceeded your credit overdraft.");
        ENDCASE;
    CASE(eVA_UI_THRESHOLD_REACHED_EVENT);
        Toast::makeText("You have reached your credit threshold.");
        ENDCASE;
    CASE(eVA_UI_RECORD_RIGHT_OK_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_TIMESHIFT_RIGHT_OK_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_RECORD_RIGHT_KO_EVENT);
        Toast::makeText("You do not have the rights to record this <Program>.");
        ENDCASE;
    CASE(eVA_UI_TIMESHIFT_RIGHT_KO_EVENT);
        Toast::makeText("You do not have timeshift rights.");
        ENDCASE;
    CASE(eVA_UI_RECORD_INFO_CHANGE_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_ORM_CONNECTION_OK_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_ORM_CONNECTION_KO_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_CW_SERVER_CONNECTION_OK_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_CW_SERVER_CONNECTION_KO_EVENT);
        Toast::makeText("");
		ENDCASE;
    CASE(eVA_UI_TERMINAL_NOT_ACTIVATED_EVENT);
        Toast::makeText("The terminal is not activated.Please contact your operator."); 
        ENDCASE;
    CASE(eVA_UI_TERMINAL_READY_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_TERMINAL_INSUFFICIENT_MEMORY_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_TERMINAL_CONFIGURATION_ERROR_EVENT);
        Toast::makeText("");
        ENDCASE;
    CASE(eVA_UI_TERMINAL_INVALIDATED_EVENT);
        Toast::makeText("The terminal has been invalidated by the operator.Please contact your operator.");
        ENDCASE;
    default:NGLOG_DEBUG("unknown event 0x%x",evt->eEventType);break;
    }
    NGLOG_DEBUG(" acs:%d scslot:%d streamhandle:%x IsScrambledByProgram:%d uiStreamsNumber:%d",evt->dwAcsId,evt->dwScSlot,
         evt->dwStbStreamHandle,evt->bIsScrambledByProgram,evt->uiStreamsNumber);
}

int SendUIRequest(int request_type,void*ret,void*userdata){
    tVA_UI_Request req;
    memset(&req,0,sizeof(req));
    req.stRequestHeader.eType=(tVA_UI_RequestType)request_type;
    req.stRequestHeader.pCallback=VA_UI_RequestCBK;
    req.stRequestHeader.pUserArg=userdata;
    return VA_UI_Request(&req);
}
  
