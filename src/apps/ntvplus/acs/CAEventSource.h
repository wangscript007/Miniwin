#include<looper/EventLoop.h>
#include<va_ui.h>
#include<vector>

using namespace nglui;

enum{
   CASVC_INIT=0,
   CASVC_SCRAMBLED=1,
   CASVC_DESCRAMBLED=2
};
class ViaccessEventSource:public nglui::EventSource{
protected:
    std::vector<tVA_UI_Event>events;
public:
    DWORD svcstate;
public:
    ViaccessEventSource(){};
    bool prepare(int&) override { return events.size();}
    bool check(){
        return  events.size();
    }
    void addEvent(tVA_UI_Event&e){
        events.emplace(events.begin(),e);
    }
    bool getEvent(tVA_UI_Event&e){
        if(events.size()){
           e=events.back();
           events.pop_back();
           return true;
        }
        return false;
    }
    bool dispatch(EventHandler &func) { return func(*this); }
};

