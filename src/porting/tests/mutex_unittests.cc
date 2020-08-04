#include <stdio.h>
#include <sys/time.h>
#include <gtest/gtest.h>
extern "C"{
#include <ngl_os.h>
}

class OSMUTEX:public testing::Test{
   
   public :
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

TEST_F(OSMUTEX,_Create_1){
   NGLMutex mutex;
   ASSERT_EQ(0,nglCreateMutex(&mutex));
   ASSERT_EQ(0,nglDeleteMutex(mutex));
}

TEST_F(OSMUTEX,Create_Error){
   NGLMutex mtx;
   ASSERT_NE(0,nglCreateMutex(NULL));
}

TEST_F(OSMUTEX,Delete_1){
   NGLMutex mtx;
   ASSERT_EQ(0,nglCreateMutex(&mtx));
   ASSERT_EQ(0,nglDeleteMutex(mtx));
}

TEST_F(OSMUTEX,Delete_Error_1){
   ASSERT_NE(0,nglDeleteMutex(NULL));
}

TEST_F(OSMUTEX,Lock_1){
   NGLMutex mtx;
   ASSERT_EQ(0,nglCreateMutex(&mtx));
   ASSERT_EQ(0,nglLockMutex(mtx));
   ASSERT_EQ(0,nglDeleteMutex(mtx));
}

TEST_F(OSMUTEX,Lock_Error){
   ASSERT_NE(0,nglLockMutex(NULL));
}

TEST_F(OSMUTEX,Lock_RECURSIVE){//lock multi time in same thread must success
   NGLMutex mtx;
   ASSERT_EQ(0,nglCreateMutex(&mtx));
   ASSERT_EQ(0,nglLockMutex(mtx));
   ASSERT_EQ(0,nglLockMutex(mtx));
   ASSERT_EQ(0,nglUnlockMutex(mtx));   
   ASSERT_EQ(0,nglUnlockMutex(mtx));   
   ASSERT_EQ(0,nglDeleteMutex(mtx));
}

TEST_F(OSMUTEX,Lock_Unlock_1){
   NGLMutex mtx;
   ASSERT_EQ(0,nglCreateMutex(&mtx));
   ASSERT_EQ(0,nglLockMutex(mtx));
   ASSERT_EQ(0,nglUnlockMutex(mtx));
   ASSERT_EQ(0,nglDeleteMutex(mtx));
}
