#ifndef __SUBSCRIBER_H__
#define __SUBSCRIBER_H__
#include <scheduler.h>
#include <dvbepg.h>
namespace nglui{
struct SubscribeItem{
   SERVICELOCATOR svc;
   system_clock::time_point time;
   UINT eventid;
   UINT duration;/*unint in seconds*/ 
   std::string name;
   void onTriggered();
};
class Subscriber:public Scheduler{
private:
    Subscriber():Scheduler(){}
    static Subscriber*mInst;
protected:
    std::map<system_clock::time_point,SubscribeItem>items;
public:
    static Subscriber*getInstance();
    int addOnce(SubscribeItem&);
    int addDaily(SubscribeItem&);
    int addWeekly(SubscribeItem&);
    int load(const std::string&filename);
    int save(const std::string&filename);
};
}
#endif
