#ifndef __SHCEDULE_H__
#include<chrono>
#include<functional>
#include<map>
#include <looper/looper.h>

using namespace std::chrono;
namespace nglui{

class Scheduler:public EventSource{
public:
typedef std::function<void(void)> Function;
private:
protected:
    std::map<system_clock::time_point,Function>taskQueue;
public:
    void schedule(Function f,system_clock::time_point t);
    void scheduleFromNow(Function f, int64_t deltaSeconds);
    void scheduleEvery(Function f, int64_t deltaSeconds);
    void scheduleHourly(Function f,system_clock::time_point t);
    void scheduleDaily(Function f,system_clock::time_point t);
    void scheduleWeekly(Function f,system_clock::time_point t);
    void scheduleMonthly(Function f,system_clock::time_point t);/*TODO*/

    void remove(system_clock::time_point t);
    bool dispatch(nglui::EventHandler &func)override{ return func(*this);}
    bool check()override;/*check schedulered item*/
};

}
#endif

