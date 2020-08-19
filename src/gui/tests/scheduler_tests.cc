#include <gtest/gtest.h>
#include <windows.h>
#include <ngl_os.h>
#include <scheduler.h>

using namespace nglui;

class SCHEDULER:public testing::Test{

   public :
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};


TEST_F(SCHEDULER,Once){
   Scheduler sch;
   int count=0;
   sch.schedule([&count](){count++;},system_clock::now()+std::chrono::seconds(10));
   sleep(10);
   sch.check();
   ASSERT_EQ(count,1);
}

TEST_F(SCHEDULER,FromNow){
   Scheduler sch;
   int count=0;
   sch.scheduleFromNow([&count](){count++;},10);
   sleep(10);
   sch.check();
   ASSERT_EQ(count,1);
}

TEST_F(SCHEDULER,Every){
   Scheduler sch;
   int count=0;
   sch.scheduleEvery([&count](){count++;},5);
   for(int i=0;i<20;i++){
       sleep(1);
       sch.check();
   }
   ASSERT_EQ(count,4);
}

TEST_F(SCHEDULER,Hourly){
   Scheduler sch;
   int count=0;
   sch.scheduleHourly([&count](){count++;},system_clock::now());
   sleep(10);
   sch.check();
   ASSERT_EQ(count,0);
}

TEST_F(SCHEDULER,Hourly1){
   Scheduler sch;
   int count=0;
   sch.scheduleHourly([&count](){count++;},system_clock::now());
   sleep(3600);
   sch.check();
   ASSERT_EQ(count,1);
}

TEST_F(SCHEDULER,Daily){
   Scheduler sch;
   int count=0;
   sch.scheduleDaily([&count](){count++;},system_clock::now());
   sleep(10);
   sch.check();
   ASSERT_EQ(count,0);
}

TEST_F(SCHEDULER,Weekly){
   Scheduler sch;
   int count=0;
   sch.scheduleWeekly([&count](){count++;},system_clock::now());
   sleep(10);
   sch.check();
   ASSERT_EQ(count,0);
}
