#include <stdio.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include <list>
#include <ngl_dmx.h>
#include <ngl_video.h>
#include <ngl_tuner.h>
#include <dvbepg.h>
#include <satellite.h>
#include <tvtestutils.h>
#include <diseqc.h>
#include <tvtestutils.h>
#include <ngl_misc.h>

class SEARCH:public testing::Test{
public:
   std::vector<SERVICELOCATOR> svcs;
   NGLTunerParam tp;
   HANDLE handle;
   public :
   static void SetUpTestCase(){
       nglSysInit();
       nglTunerInit();
       nglDmxInit();
       DtvEpgInit();
       printf("SEARCH::SetUpTestCase\r\n");
   }
   static void TearDownTestCase() {
       printf("SEARCH::TearDownTestCase\r\n");
   }
   
   virtual void SetUp(){
      tvutils::GetTuningParams(tp);
      handle=nglCreateEvent(0,1);
      nglTunerLock(0,&tp);
      LoadSatelliteFromDB("satellites.xml");
   }
   virtual void TearDown(){
      nglDestroyEvent(handle);
   }
};

static INT SVC_CBK(const SERVICELOCATOR*loc,const DVBService*svc,const BYTE*pmt,void*userdata)
{
   char servicename[128],providername[128];
   svc->getServiceName(servicename,providername);
   printf("\tservice[%04d] %x.%x.%x %s:%s\r\n",(*(int*)userdata),loc->netid,loc->tsid,loc->sid,servicename,providername);
   std::vector<SERVICELOCATOR>*svcs=(std::vector<SERVICELOCATOR>*)userdata;
   svcs->push_back(*loc);
   return 1;
}

static SERVICELOCATOR svcs[64];
static USHORT lcn[64];
static int cnt=0;

static void ONTP_FINISHED(const TRANSPONDER*tp,int idx,int tpcount,void*userdata){
    if(tp==NULL){
       nglSetEvent((HANDLE)userdata);
       DtvSaveProgramsData("dvb_programs.dat");
    }
}

TEST_F(SEARCH,STP1){//search single tp
   INT count=0;
   SEARCHNOTIFY notify;
   
   //nglTunerSetLNB(0,1);nglTunerSet22K(0,1);
   ConfigureTransponder(&tp);
   memset(&notify,0,sizeof(SEARCHNOTIFY));
   notify.FINISHTP=ONTP_FINISHED;
   notify.userdata=handle;
   DtvSearch(&tp,1,&notify);
   ASSERT_TRUE(1);
   nglSleep(8000);
   DtvEnumService(SVC_CBK,&svcs);
   for(auto s:svcs){
      DtvPlay(&s,nullptr);
      nglSleep(10000);
   }
   ASSERT_TRUE(svcs.size()>0);
}
TEST_F(SEARCH,MTP){//search multi tp
   INT count=0;
   NGLTunerParam tps[2];
   SEARCHNOTIFY notify;
   //nglTunerSetLNB(0,1);nglTunerSet22K(0,1);
   ConfigureTransponder(&tp);
   memset(&notify,0,sizeof(SEARCHNOTIFY));
   notify.FINISHTP=ONTP_FINISHED;
   notify.userdata=handle;
   DtvSearch(tps,sizeof(tps)/sizeof(NGLTunerParam),&notify);
   ASSERT_TRUE(1);
   nglSleep(8000);
   DtvEnumService(SVC_CBK,&svcs);
   for(auto s:svcs){
      DtvPlay(&s,nullptr);
      nglSleep(10000);
   }
   ASSERT_TRUE(svcs.size()>0);
}

TEST_F(SEARCH,NIT){//search multi TP by NIT
   INT count=0;
   SEARCHNOTIFY notify;
   //nglTunerSetLNB(0,1);nglTunerSet22K(0,1);
   ConfigureTransponder(&tp);
   memset(&notify,0,sizeof(SEARCHNOTIFY));
   notify.FINISHTP=ONTP_FINISHED;
   notify.userdata=handle;
   DtvSearch(&tp,0,&notify);
   ASSERT_TRUE(1);
   nglSleep(8000);
   DtvEnumService(SVC_CBK,&svcs);
   for(auto s:svcs){
      DtvPlay(&s,nullptr);
      nglSleep(10000);
   }
   ASSERT_TRUE(svcs.size()>0);

}



