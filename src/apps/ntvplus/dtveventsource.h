#ifndef __DTV_EVENT_SOURCE_H__
#define __DTV_EVENT_SOURCE_H__
#include<looper/looper.h>
#include<map>
#include<unordered_map>
#include<unordered_set>
#include<dvbepg.h>
namespace nglui{
class DtvEventSource:public EventSource{
private:
    std::unordered_map<int64_t,int>events;
    std::unordered_set<class View*>views;
    std::map<int,int>msgmap;
    static DtvEventSource*mInst;
    DtvEventSource();
    void dispatchMessage(DWORD msg,DWORD wp,ULONG lp);
public:
    static DtvEventSource*getInstance();
    //bool is_idle_source() const override {return true;}/*idle will caused crash*/
    void push_event(const SERVICELOCATOR&svc,int msg);
    bool dispatch(EventHandler &func)override;
    bool prepare(int&)override;
    bool check()override;/*check schedulered item*/
    void process();
    void registe(View*v);
    void unregiste(View*v);
};
}/*name space*/
#endif
