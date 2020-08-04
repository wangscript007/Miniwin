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
class FAVORITES:public testing::Test{
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
       nglAvInit();

       
       if(0== DtvLoadProgramsData("dvb_programs.dat")){
           tvutils::GetTuningParams(tp);
           nglTunerLock(0,&tp);
           memset(&notify,0,sizeof(notify));
           notify.FINISHTP=ONTP_FINISHED;
           handle=nglCreateEvent(0,1); 
           notify.userdata=handle;

           DtvSearch(&tp,1,&notify);
           ASSERT_EQ(0,nglWaitEvent(handle,5000));
       }
       FavInit(NULL);
       printf("FAVORITES::SetUpTestCase\r\n");
   }
   static void TearDownTestCase() {
       printf("FAVORITES::TearDownTestCase\r\n");
   }
   
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

TEST_F(FAVORITES,Count){
    ASSERT_GE(FavGetGroupCount(),4);
}

TEST_F(FAVORITES,System){//search single tp
    int ac=FavGetServiceCount(FAV_GROUP_AUDIO);
    int vc=FavGetServiceCount(FAV_GROUP_VIDEO);
    int avc=FavGetServiceCount(FAV_GROUP_AV);
    int all=FavGetServiceCount(FAV_GROUP_ALL);
    ASSERT_EQ(avc,ac+vc);
    ASSERT_GE(all,ac);
    ASSERT_GE(all,vc);
    ASSERT_GE(all,avc);
    printf("vc=%d ac=%d all=%d\r\n",vc,ac,avc);
}

TEST_F(FAVORITES,PlayVideo){
    SERVICELOCATOR svcs[128];
    int count=FavGetServices(FAV_GROUP_VIDEO,svcs,sizeof(svcs)/sizeof(svcs[0]));
    for(int i=0;i<count;i++){
       DtvPlay(svcs+i,nullptr);
       sleep(10);
    }
    printf("VideoService=%d\r\n",count);
}

TEST_F(FAVORITES,PlayAudio){
    SERVICELOCATOR svcs[128];
    int count=FavGetServices(FAV_GROUP_AUDIO,svcs,sizeof(svcs)/sizeof(svcs[0]));
    for(int i=0;i<count;i++){
       DtvPlay(svcs+i,nullptr);
       sleep(10);
    }
    printf("AudioService=%d\r\n",count);
}

