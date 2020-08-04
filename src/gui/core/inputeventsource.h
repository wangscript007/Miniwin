#ifndef __INPUT_EVENT_SOURCE_H__
#define __INPUT_EVENT_SOURCE_H__
#include <ngl_input.h>
#include <looper/looper.h>
#include <queue>
#include <string>
#include <fstream>
#include <uievents.h>
#include <inputdevice.h>
#include <unordered_map>

#if ENABLED_GESTURE
namespace GRT{
   class GestureRecognitionPipeline;
}
#endif

namespace nglui{

class InputEventSource:public EventSource{
protected:
    std::ofstream frecord;
    std::queue<InputEvent*>events;
#if ENABLED_GESTURE
    std::unique_ptr<GRT::GestureRecognitionPipeline>pipeline;
#endif
    PooledInputEventFactory* eventPool;
    std::unordered_map<int,std::shared_ptr<InputDevice>>devices;
    std::shared_ptr<InputDevice>getdevice(int fd);
public:
    InputEventSource(const std::string&recordfile=std::string() );
    ~InputEventSource();
    bool initGesture(const std::string&fname);
    static void play(const std::string&);
    bool prepare(int& max_timeout);
    bool check(){return events.size()>0;}
    bool dispatch(EventHandler &func) { return func(*this); }
    bool is_file_source() const override final { return false; }
    int process(const INPUTEVENT*es,int count);
    bool processKey();
};
}
#endif
