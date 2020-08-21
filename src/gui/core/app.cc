#include <app.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ngl_os.h>
#include <ngl_misc.h>
#include <ngl_graph.h>
#include <windowmanager.h>
#include <assets.h>
#include <cairomm/surface.h>
#include <usbmanager.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <thread>
#include <ngl_input.h>
#include <inputeventsource.h>
#include <uieventsource.h>
#include <looper/GenericSignalSource.h>
#include <mutex>

NGL_MODULE(APP);

void spt_init(int argc, char *argv[]);
void setproctitle(const char *fmt, ...);
namespace nglui{

App*App::mInst=nullptr;

static struct option app_options[]={
   {"alpha"   ,required_argument,0,0},
   {"config"  ,required_argument,0,0},
   {"language",required_argument,0,0},
   {"record"  ,required_argument,0,0},
   {"monkey"  ,required_argument,0,0},
   {0,0,0,0}
};

App::App(int argc,const char*argv[],const struct option*extoptions){
    int option_index=-1,c=-1;
    std::string optstring;
    std::vector<struct option>all;
    nglLogParseModules(argc,argv);    
    assets=nullptr;
    mInst=this;
    if(argc&&argv){
       spt_init(argc,(char**)argv);
       setName(argv[0]);
    }
    NGLOG_DEBUG("App %s started",(argc&&argv)?argv[0]:"");
    nglSysInit();
    nglGraphInit();
    USBManager::getInstance().startMonitor();
    all.insert(all.begin(),app_options,app_options+(sizeof(app_options)/sizeof(option)-1));
    for(;extoptions&&extoptions->name;)all.push_back(*extoptions++);
    for(auto o:all){//::optional :must has arg 
        NGLOG_VERBOSE("%s hasarg=%d %c",o.name,o.has_arg,o.val);
        if(o.val==0)continue;
        optstring.append(1,o.val);
        if(o.has_arg)optstring.append(o.has_arg,':');
    }

    do{
        c=getopt_long_only(argc,(char*const*)argv,optstring.c_str(),all.data(),&option_index);
        NGLOG_VERBOSE_IF(c>=0,"option_index=%d  c=%c",option_index,c);
        if(c>0){
            for(int i=0;i<all.size();i++)if(all[i].val==c){option_index=i;break;}
            std::string key=all[option_index].name;
            args[key]=optarg?:"";
            NGLOG_DEBUG("args[%d]%s:%s",option_index,key.c_str(),optarg);
        }else if(c==0){
            std::string key=all[option_index].name;
            args[key]=optarg?:"";
            NGLOG_DEBUG("args[%d]%s:%s",option_index,key.c_str(),optarg);
        }
    }while(c>=0);

    setOpacity(getArgAsInt("alpha",255));

    addEventSource(new InputEventSource(getArg("record","")),[](EventSource&s){
        ((InputEventSource&)s).processKey();
        return true;
    });
    
    InputEventSource::play(getArg("monkey",""));

    SignalSource*sigsource=new GenericSignalSource(false);
    addEventSource(sigsource,[&](EventSource&s){
        NGLOG_INFO("Sig interrupt");
        looper.quit(0);
        return false;
    });

    sigsource->add(SIGINT);
    sigsource->add(SIGTERM);
    sigsource->add(SIGKILL);
}

App::~App(){
    delete assets;
    delete &WindowManager::getInstance();
    delete &GraphDevice::getInstance();
    NGLOG_DEBUG("%p Destroied",this);
}

App&App::getInstance(){
    return *mInst;
}

const std::string&App::getArg(const std::string&key,const std::string&def){
    auto itr=args.find(key);
    if(itr==args.end()||itr->second.empty())
        return def;
    return itr->second;
}

bool App::hasArg(const std::string&key){
    auto itr=args.find(key);
    return itr!=args.end();
}

void App::setArg(const std::string&key,const std::string&value){
    args[key]=value;
}

int App::getArgAsInt(const std::string&key,int def){
    auto itr=args.find(key);
    if(itr==args.end()||itr->second.empty())
        return def;
    const char*arg=args[key].c_str();
    return strtoul(arg,NULL,(strpbrk(arg,"xX")?16:10));
}

void App::setOpacity(unsigned char alpha){
    nglSurfaceSetOpacity(GraphDevice::getInstance().getPrimarySurface(),alpha);
    NGLOG_DEBUG("alpha=%d",alpha);
}

Assets*App::getAssets(){
    if(assets==nullptr){
        struct stat pakstat;
        std::string pakname=getName();
        pakname+=".pak";
        int rc=stat(pakname.c_str(),&pakstat);
        if(rc==0)
           assets=new Assets(pakname);
        NGLOG_WARN_IF(rc,"resource %s not found",pakname.c_str());
    }
    return assets;
}

RefPtr<ImageSurface>App::loadImage(std::istream&is){
    return assets->loadImage(is);
}

RefPtr<ImageSurface>App::loadImage(const std::string&resname,bool cache){
    if(assets)return assets->loadImage(resname,cache);
    return RefPtr<ImageSurface>(nullptr);
}

const std::string App::getString(const std::string&id,const std::string&lan){
    if(assets)return assets->getString(id,lan);
    return std::string();
}

std::unique_ptr<std::istream>App::getInputStream(const std::string&fname){
    return assets->getInputStream(fname);
}

int App::addEventSource(EventSource *source, EventHandler handler){
    return looper.add_event_source(source,handler);
}

int App::removeEventSource(EventSource*source){
    return looper.remove_event_source(source);
}

int App::exec(){
    return looper.run();
}

void App::exit(int code){
    looper.quit(code);
}

void App::setName(const std::string&appname){
    //setproctitle(appname.c_str());
    name=appname;
    getAssets();
}

const std::string& App::getName(){
    return name;
}

}

