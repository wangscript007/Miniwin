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
#include <utils.h>

class UTF:public testing::Test{
public:
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

TEST_F(UTF,Ansi){
   const char *ansi="Hello world!";
   char utf[32];
   int len=strlen(ansi);
   ASSERT_EQ(len,ToUtf8(ansi,len,utf));
   ASSERT_STREQ(ansi,utf);
   ASSERT_EQ(0,ToUtf8(ansi,0,utf)); 
   utf[0]=0;
   ASSERT_EQ(1,ToUtf8(ansi,1,utf)); 
   ASSERT_EQ(*ansi,utf[0]);
}

TEST_F(UTF,ISOxxx_1000xx){//search single tp
   const char*hello="Hello World!";
   char ansi[128]={0x10,0,1};
   char utf[32];
   int iso[]={5,6,7,8,9,11,13,14,15};
   int len=strlen(hello);
   memcpy(ansi+3,hello,len);
   ASSERT_EQ(0,ToUtf8(ansi,3,utf));
   for(int i=0;i<sizeof(iso)/sizeof(iso[0]);i++){
      ansi[2]=iso[i];
      for(int j=1;j<len;j++){
         ASSERT_EQ(j,ToUtf8(ansi,j+3,utf));
         ASSERT_EQ(0,memcmp(utf,ansi+3,j));
      }
   }
}

TEST_F(UTF,ISO8859_xx){//search single tp
   const char*hello="Hello World!";
   char ansi[128];
   char utf[32];
   int iso[]={1,2,3,4,5,7,9,0xa,0xb};
   int len=strlen(hello);
   memcpy(ansi+1,hello,len);
   for(int i=0;i<sizeof(iso)/sizeof(iso[0]);i++){
      ansi[0]=iso[i];
      ASSERT_EQ(0,ToUtf8(ansi,1,utf));
      for(int j=1;j<len;j++){
         ASSERT_EQ(j,ToUtf8(ansi,j+1,utf));
         ASSERT_EQ(0,memcmp(utf,ansi+1,j));
      }
   }
}

TEST_F(UTF,GB2312){//search multi tp
   const char *ansi="Hello world!";
   char utf[32];
   int len=strlen(ansi);
   ASSERT_EQ(len,ToUtf8(ansi,len,utf));
   ASSERT_STREQ(ansi,utf);
   ASSERT_EQ(0,ToUtf8(ansi,0,utf)); 
   utf[0]=0;
   ASSERT_EQ(1,ToUtf8(ansi,1,utf)); 
   ASSERT_EQ(*ansi,utf[0]);
}




