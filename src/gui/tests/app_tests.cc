#include <gtest/gtest.h>
#include <windows.h>
#include <ngl_os.h>

using namespace nglui;

class APP:public testing::Test{

   public :
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};


TEST_F(APP,EmptyArgs){
   App app(0,NULL);
}

TEST_F(APP,TwoArgs){
   const char*args[]={"arg1","--alpha"};
   //App app(2,args);
}
TEST_F(APP,exec){
   static const char*args[]={"arg1","alpha",NULL};
   App app(2,args);
   app.exec();
}
