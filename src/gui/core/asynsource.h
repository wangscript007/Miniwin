#ifndef __ASYNC_SOURCE_H__
#define __ASYNC_SOURCE_H__
#include <ngl_input.h>
#include <looper/looper.h>
#include <view.h>

namespace nglui{

DECLARE_UIEVENT(void,AsyncEvent,void);

//template<class F, class... Args>
class AsyncEventSource:public EventSource{
protected:
    class ThreadPool*pool;
public:
    AsyncEventSource();
    ~AsyncEventSource();
    bool prepare(int& max_timeout);
    bool check();
    bool dispatch(EventHandler &func);
    bool is_file_source() const override final { return false; }
    //void enqueue(F&& f, Args&&... args);
};

}
#endif
