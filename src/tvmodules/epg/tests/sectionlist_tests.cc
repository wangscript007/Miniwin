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
#include <lrucache.h>
#include <mutex>
namespace std{
    template<>
    struct hash<SERVICELOCATOR>{
        size_t operator()(SERVICELOCATOR const& s) const{
            return (s.netid<<32)|(s.tsid<<16)|s.sid;
        }
    };
}

class SECTIONLISTS:public testing::Test{
public:
   public :
   virtual void SetUp(){
   }
   virtual void TearDown(){
   }
};

TEST_F(SECTIONLISTS,list){
    BYTE buffer[1024];
    buffer[1]=3;buffer[2]=0xFF;
    PSITable table(buffer);
    SECTIONLIST sl;
    sl.push_back(table);
}

TEST_F(SECTIONLISTS,sort){
    BYTE buffer[1024];
    buffer[1]=3;buffer[2]=0xFF;
    SECTIONLIST sl;
    for(int i=255;i>=0;i--){
       buffer[6]=i;
       buffer[7]=255;
       PSITable table(buffer);
       sl.push_back(table);
       std::sort(sl.begin(),sl.end(),std::less<PSITable>());
    }
}

TEST_F(SECTIONLISTS,cache){
    BYTE buffer[1024];
    lru::Cache<SERVICELOCATOR,std::shared_ptr<SECTIONLIST>,std::mutex >cache(128,10);
    SERVICELOCATOR loc={1,1,1,0};
    buffer[1]=3;buffer[2]=0;
    for(int i=0;i<256;i++){
       std::shared_ptr<SECTIONLIST>lst(new SECTIONLIST());
       loc.sid=i+1;
       cache.insert(loc,lst); 
       for(int j=255;j>=0;j--){
           buffer[6]=j;
           buffer[7]=255;
           PSITable table(buffer);
           lst->push_back(table);
           //std::sort(lst->begin(),lst->end(),std::less<PSITable>());
       }
    }
    cache.empty();
}


