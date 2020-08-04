#include <stdio.h>
#include <gtest/gtest.h>
#include <ngl_os.h>
#include <ngl_input.h>
#include <fcntl.h>

class INPUT:public testing::Test{
   public :
   virtual void SetUp(){
       nglInputInit();
       printf("inputevent.size=%d\r\n",sizeof(INPUTEVENT));
   }
   virtual void TearDown(){}
};

TEST_F(INPUT,GetDeviceInfo){
   INPUTDEVICEINFO info;
   nglInputGetDeviceInfo(INJECTDEV_KEY,&info);
   ASSERT_TRUE(info.source&(1<<EV_KEY));
   ASSERT_GT(strlen(info.name),0);
   nglInputGetDeviceInfo(INJECTDEV_PTR,&info);
   ASSERT_GT(strlen(info.name),0);
   ASSERT_TRUE(info.source&(1<<EV_ABS));
}

TEST_F(INPUT,InjectEvent){
   INPUTEVENT e;
   INPUTEVENT e2;
   e.type=EV_KEY;
   e.code=KEY_ENTER;
   e.device=INJECTDEV_KEY;
   ASSERT_EQ(1,nglInputInjectEvents(&e,1,0));
   ASSERT_EQ(1,nglInputGetEvents(&e2,1,1));
   ASSERT_EQ(e.type,e2.type);
   ASSERT_EQ(e.code,e2.code);
   ASSERT_EQ(e.value,e2.value);
}

TEST_F(INPUT,GetKey){
   int i=0;
   int rc=0;
   while(i++<100){
      INPUTEVENT keys[16];
      rc+=nglInputGetEvents(keys,16,500);
   }
   ASSERT_GT(rc,0);
}
