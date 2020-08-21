#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include <string>
#include <map>
#include <istream>
#include <cairomm/surface.h>
#include <looper/looper.h>

struct option;
using namespace Cairo;

namespace nglui{

class App{
private:
    class Assets*assets;
    Assets*getAssets();
    std::map<std::string,std::string>args;
protected:
    static App*mInst;
    std::string name;
    EventLoop looper;
public:
     App(int argc=0,const char*argv[]=NULL,const option*extoptions=NULL);
     ~App();
     static App&getInstance();
     void setOpacity(unsigned char alpha);
     void setName(const std::string&appname);
     const std::string&getName();

     RefPtr<ImageSurface>loadImage(std::istream&);
     RefPtr<ImageSurface>loadImage(const std::string&resname,bool cache=true);
     const std::string getString(const std::string&id,const std::string&lan="");

     std::unique_ptr<std::istream>getInputStream(const std::string&fname);
     void setArg(const std::string&key,const std::string&value);
     bool hasArg(const std::string&key);
     const std::string&getArg(const std::string&key,const std::string&def="");

     int getArgAsInt(const std::string&key,int def);
     int addEventSource(EventSource *source, EventHandler handler);
     int removeEventSource(EventSource*source);
     virtual int exec();
     void exit(int code);
};

}//namespace
#endif
