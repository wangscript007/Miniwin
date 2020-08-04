#include <gtest/gtest.h>
#include <window.h>
#include <signal.h>
#include <execinfo.h>
#include <ngl_log.h>

class GUIEnvironment: public testing::Environment{
  public:
    void SetUp(){
       printf("GUIEnvironment Setup\r\n");
    }
    void TearDown(){
       printf("GUIEnvironment TearDown\r\n");
    }
};

int main(int argc,char*argv[])
{
    nglLogParseModules(argc,(const char**)argv);
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
