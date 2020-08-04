#include <stdio.h>
#include <gtest/gtest.h>
#include <ngl_os.h>
#include <ngl_dmx.h>
#include <ngl_video.h>
#include <ngl_tuner.h>
#include <tvtestutils.h>
#include <ngl_misc.h>

class AV:public testing::Test{
   public :
   static void SetUpTestCase(){
       nglSysInit();
       nglTunerInit();
       nglDmxInit();
       nglAvInit();
       nglGraphInit();
   }
   static void TearDownTestCase(){
       sleep(20);
   }
   virtual void SetUp(){
       NGLTunerParam tp;//TRANSPONDER tp;
       tvutils::GetTuningParams(tp);
       nglTunerLock(0,&tp);
   }
   virtual void TearDown(){}
};

TEST_F(AV,Play){
   nglAvPlay(0,512,DECV_MPEG,650,DECA_MPEG1,128);
   nglSleep(10000);
}

TEST_F(AV,PlayGlobalAlpha){
   HANDLE mainsurface;
   nglCreateSurface(&mainsurface,1280,720,GPF_ARGB,1);
   nglFillRect(mainsurface,nullptr,0xFFFFFFFF);
   nglFlip(mainsurface);
   nglSurfaceSetOpacity(mainsurface,0x80);
   for(int i=255;i>0;i-=20){
      nglSurfaceSetOpacity(mainsurface,i);
      sleep(1);
   }
   nglDestroySurface(mainsurface);
}

TEST_F(AV,PlayUnderSurface){
   HANDLE mainsurface;
   nglAvPlay(0,512,DECV_MPEG,650,DECA_MPEG1,128);
   nglCreateSurface(&mainsurface,1280,720,GPF_ARGB,1);
   nglSurfaceSetOpacity(mainsurface,0x80);
   for(int i=255;i>0;i-=20){
      nglFillRect(mainsurface,nullptr,0x00FFFFFF|(i<<24));
      nglFlip(mainsurface);
      sleep(1);
   }
   nglDestroySurface(mainsurface);
}

TEST_F(AV,PlayInWindow){
   HANDLE mainsurface;
   NGLRect rect={200,200,800,400};
   nglCreateSurface(&mainsurface,1280,720,GPF_ARGB,1);
   nglSurfaceSetOpacity(mainsurface,0x80);
   nglAvPlay(0,512,DECV_MPEG,650,DECA_MPEG1,128);
   for(int i=255;i>0;i-=20){
      nglFillRect(mainsurface,&rect,0x00FFFFFF|(i<<24));
      nglFlip(mainsurface);
      sleep(1);
   }
   nglDestroySurface(mainsurface);
}

