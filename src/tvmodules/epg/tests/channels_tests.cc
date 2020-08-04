#include <stdio.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include <list>
#include <ngl_dmx.h>
#include <ngl_video.h>
#include <ngl_graph.h>
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
       DtvSaveProgramsData("dvb_programs.dat");
    }
}
class CHANNELS:public testing::Test{
public:
   public :
   static void SetUpTestCase(){
       HANDLE handle;
       NGLTunerParam tp;
       SEARCHNOTIFY notify;
       nglSysInit();
       nglTunerInit();
       nglDmxInit();
       nglGraphInit();
       DtvEpgInit();
       nglAvInit();

       tvutils::GetTuningParams(tp);
       nglTunerLock(0,&tp);
       memset(&notify,0,sizeof(notify));
       notify.FINISHTP=ONTP_FINISHED;
       handle=nglCreateEvent(0,1); 
       notify.userdata=handle;

       DtvSearch(&tp,1,&notify);
       ASSERT_EQ(0,nglWaitEvent(handle,5000));
       FavInit(NULL);
       printf("CHANNELS::SetUpTestCase\r\n");
   }
   static void TearDownTestCase() {
       printf("CHANNELS::TearDownTestCase\r\n");
   }
   
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

TEST_F(CHANNELS,SERVICES){
    ASSERT_GT(FavGetServiceCount(FAV_GROUP_ALL),0);
}

TEST_F(CHANNELS,Play){//search single tp
    SERVICELOCATOR svcs[128];
    int count=FavGetServices(FAV_GROUP_ALL,svcs,128);
    for(int i=0;i<count;i++){
       ELEMENTSTREAM es[16];
       int cnt=DtvGetElementsByCategory(svcs+i,ST_AUDIO,es);
       if(cnt<=1){
           DtvPlay(svcs+i,NULL);
           sleep(20);
       }else for(int j=0;j<cnt;j++){
           DtvPlay(svcs+i,es[j].iso639lan);
           sleep(10);
       }
    }
    ASSERT_GT(count,0);
}

TEST_F(CHANNELS,PlayGlobalAlpha){
    SERVICELOCATOR svcs[128];
    int count=FavGetServices(FAV_GROUP_ALL,svcs,128);
    HANDLE mainsurface;
    nglCreateSurface(&mainsurface,1280,720,GPF_ARGB,1);
    nglFillRect(mainsurface,nullptr,0xFFFFFFFF);
    nglFlip(mainsurface);
    nglSurfaceSetOpacity(mainsurface,0x80);
    for(int i=0;i<count;i++){
       ELEMENTSTREAM es[16];
       int cnt=DtvGetElementsByCategory(svcs+i,ST_AUDIO,es);
       if(cnt<=1){
           DtvPlay(svcs+i,NULL);
           sleep(20);
       }else for(int j=0;j<cnt;j++){
           DtvPlay(svcs+i,es[j].iso639lan);
           sleep(10);
       }
       nglSurfaceSetOpacity(mainsurface,0x80-i*10);
    }
    nglDestroySurface(mainsurface);
}

TEST_F(CHANNELS,PlayUnderSurface){
    SERVICELOCATOR svcs[128];
    UINT*buffer,pitch;
    int count=FavGetServices(FAV_GROUP_ALL,svcs,128);
    HANDLE mainsurface;
    nglCreateSurface(&mainsurface,1280,720,GPF_ARGB,1);
    nglFillRect(mainsurface,nullptr,0);
    nglLockSurface(mainsurface,(void**)&buffer,&pitch);
    for(int j=0;j<10000;j++)
        ASSERT_EQ(buffer[j],0); 
    nglFlip(mainsurface);
    for(int i=0;i<count;i++){
       ELEMENTSTREAM es[16];
       int cnt=DtvGetElementsByCategory(svcs+i,ST_AUDIO,es);
       if(cnt<=1){
           DtvPlay(svcs+i,NULL);
           sleep(20);
       }else for(int j=0;j<cnt;j++){
           DtvPlay(svcs+i,es[j].iso639lan);
           sleep(10);
       }
    }
    nglDestroySurface(mainsurface);
}

TEST_F(CHANNELS,PlayInWindow){
    SERVICELOCATOR svcs[128];
    UINT*buffer,pitch;
    int count=FavGetServices(FAV_GROUP_ALL,svcs,128);
    HANDLE mainsurface;
    NGLRect rect={200,100,800,600};
    UINT colors[]={0xFFFFFFFF,0xFFFF0000,0xFF00FF00,0xFF0000FF};
    nglCreateSurface(&mainsurface,1280,720,GPF_ARGB,1);
    for(int i=0;i<count;i++){
       ELEMENTSTREAM es[16];
       int cnt=DtvGetElementsByCategory(svcs+i,ST_AUDIO,es);
       UINT color=colors[i%(sizeof(colors)/sizeof(UINT))];
       nglFillRect(mainsurface,nullptr,color);
       printf("FillRect with color %08x\r\n",color);
       color=colors[(i+1)%(sizeof(colors)/sizeof(UINT))];
       nglFillRect(mainsurface,&rect,(0xFFFFFF&color));
 
       nglLockSurface(mainsurface,(void**)&buffer,&pitch);

       for(int j=0;j<600-1;j++)  ASSERT_EQ(buffer[100*1280+210+j*1280]>>24,0);
       nglFlip(mainsurface);
       if(cnt<=1){
           DtvPlay(svcs+i,NULL);
           nglAvSetVideoWindow(0,nullptr,&rect);
           sleep(20);
       }else for(int j=0;j<cnt;j++){
           DtvPlay(svcs+i,es[j].iso639lan);
           nglAvSetVideoWindow(0,nullptr,&rect);
       }
       sleep(10);
    }
    nglDestroySurface(mainsurface);
}
