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
   int type;/*0-->once,1-->daily,2-->weekly...*/
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
    static SubscribeItem FromEvent(const DVBEvent&,const SERVICELOCATOR*svc=nullptr);
    int add(SubscribeItem&);
    int addOnce(SubscribeItem&);
    int addDaily(SubscribeItem&);
    int addWeekly(SubscribeItem&);
    const SubscribeItem*find(int64_t tm);/*find subscribe by tm(time_t)*/
    void remove(int64_t tm);/*remove subscribe by tm(time_t)*/
    void remove(system_clock::time_point&point);
    size_t size(){return items.size();}
    void clear(){items.clear();}
    int load(const std::string&filename);
    int save(const std::string&filename);
    int getItems(std::vector<SubscribeItem>&itms);
};
}
#endif
