#ifndef __UIEVENT_SOURCE_H__
#define __UIEVENT_SOURCE_H__
#include <inputeventsource.h>
#include<looper/looper.h>
#include <list>
#include <unordered_set>

namespace nglui{
class View;
typedef struct MSG_{
   View*view;
   DWORD msgid;
   DWORD wParam;
   ULONG lParam;
   ULONG time;
}MSG;

class UIEventSource:public EventSource{
private:
    std::list<MSG>normal_msgs;
    std::unordered_set<View*>invalidates;
    std::list<MSG>delayed_msgs;
    bool hasDelayedMessage();
    bool popMessage();
public:
    UIEventSource();
    ~UIEventSource();
    void reset();
    int getEvents();
    bool prepare(int&) override { return getEvents();}
    bool check(){
        return  getEvents();
    }
    bool dispatch(EventHandler &func) { return func(*this); }
    bool processEvents();
    bool hasMessage(const View*,DWORD msgid);
    void removeMessage(const View*,DWORD msgid);
    void sendMessage(View*,DWORD msgid,DWORD wp,ULONG lp,DWORD delayedtime=0);
};

}//end namespace

#endif
