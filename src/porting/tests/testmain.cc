#include<gtest/gtest.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_graph.h>
#include <ngl_misc.h>
#include <math.h>

class HALEnvironment: public testing::Environment{
  public:
    void SetUp(){
        nglSysInit();
        nglGraphInit();
    };
    void TearDown(){
        //sleep(5);
    }
};

int main(int argc, char*argv[])
{
    nglLogParseModules(argc,(const char**)argv);
    testing::AddGlobalTestEnvironment(new HALEnvironment);
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
