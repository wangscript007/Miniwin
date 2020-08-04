#include<gtest/gtest.h>
#include <ngl_types.h>
#include <ngl_log.h>
int main(int argc,char*argv[])
{
    nglLogParseModules(argc,(const char**)argv);
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
