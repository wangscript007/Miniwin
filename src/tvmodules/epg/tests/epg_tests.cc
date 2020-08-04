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

static void ONTP_FINISHED(const TRANSPONDER*tp,int idx,int tpcount,void*userdata){
    if(tp==NULL){
       nglSetEvent((HANDLE)userdata);
    }
}
class EPG:public testing::Test{
public:
   public :
   static void SetUpTestCase(){
       HANDLE handle;
       NGLTunerParam tp;
       SEARCHNOTIFY notify;
       nglSysInit();
       nglTunerInit();
       nglDmxInit();
       DtvEpgInit();

       tvutils::GetTuningParams(tp);
       nglTunerLock(0,&tp);
       memset(&notify,0,sizeof(notify));
       notify.FINISHTP=ONTP_FINISHED;
       handle=nglCreateEvent(0,1); 
       notify.userdata=handle;

       DtvSearch(&tp,1,&notify);
       ASSERT_EQ(0,nglWaitEvent(handle,5000));
       FavInit(NULL);
       printf("EPG::SetUpTestCase\r\n");
   }
   static void TearDownTestCase() {
       printf("EPG::TearDownTestCase\r\n");
   }
   
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

TEST_F(EPG,SERVICES){
    ASSERT_GT(FavGetServiceCount(FAV_GROUP_ALL),0);
}

TEST_F(EPG,EIT_PF){//search single tp
    int eventcount=0;
    SERVICELOCATOR svcs[128];
    int count=FavGetServices(FAV_GROUP_ALL,svcs,128);
    for(int i=0;i<count;i++){
        DVBEvent events[8];
        eventcount+=DtvGetPFEvent(svcs+i,events);
    }
    ASSERT_GT(eventcount,0);
}

TEST_F(EPG,EIT_SCHEDULE){//search multi tp
    int eventcount=0;
    SERVICELOCATOR svcs[128];
    int count=FavGetServices(FAV_GROUP_ALL,svcs,128);
    for(int i=0;i<count;i++){
        std::vector<DVBEvent> events;
        DtvGetEvents(svcs+i,events);
        eventcount+=events.size();
    }
    ASSERT_GT(eventcount,0);
}

TEST_F(EPG,GX){
    int eventcount=0;
    SERVICELOCATOR svcs[128];
    int count=FavGetServices(FAV_GROUP_ALL,svcs,128);
    for(int i=0;i<count;i++){
        DVBEvent events[8];
        eventcount+=DtvGetPFEvent(svcs+i,events);
    }
    //ASSERT_GT(eventcount,0);
    sleep(100000);
}



