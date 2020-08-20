#include <subscriber.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <json/json.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <toastwindow.h>
NGL_MODULE(Subscribe);

namespace nglui{

void SubscribeItem::onTriggered(){
    NGLOG_INFO("%d.%d.%d::%d [%p][%s]",svc.netid,svc.tsid,svc.sid,eventid,this,name.c_str());
    std::ostringstream oss;
    std::time_t tmTime = std::chrono::system_clock::to_time_t(time);
    oss<<name<<std::put_time(std::localtime(&tmTime), "%F %T");
    Toast::makeText(oss.str())->show();
    Subscriber::getInstance()->remove(time);
}

Subscriber*Subscriber::mInst=nullptr;
Subscriber*Subscriber::getInstance(){
    if(nullptr==mInst)
       mInst=new Subscriber();
    return mInst;
}

SubscribeItem Subscriber::FromEvent(const DVBEvent&e,const SERVICELOCATOR*svc){
    SubscribeItem itm;
    char name[256],desc[256];
    auto t64=std::chrono::seconds(e.start_time);
    if(svc)itm.svc=*svc;
    e.getShortName(name,desc);
    itm.eventid=e.event_id;
    itm.name=name;
    itm.time=time_point<system_clock,seconds>(t64);
    itm.type=0;
    return itm;
}

int Subscriber::add(SubscribeItem&itm){
    switch(itm.type){
    case 0:addOnce(itm);break;
    case 1:addDaily(itm);break;
    case 2:addWeekly(itm);break;
    default:return 0;
    }
    return 1;
}

int Subscriber::addOnce(SubscribeItem&itm){
    auto p=items.insert(std::make_pair(itm.time,itm));
    p.first->second.type=0;
    schedule(std::bind(&SubscribeItem::onTriggered,&p.first->second),itm.time);
}

int Subscriber::addDaily(SubscribeItem&itm){
    auto p=items.insert(std::make_pair(itm.time,itm));
     p.first->second.type=1;
    scheduleDaily(std::bind(&SubscribeItem::onTriggered,&p.first->second),itm.time);
}

int Subscriber::addWeekly(SubscribeItem&itm){
    auto p=items.insert(std::make_pair(itm.time,itm));
     p.first->second.type=2;
    scheduleWeekly(std::bind(&SubscribeItem::onTriggered,&p.first->second),itm.time);
}

int Subscriber::getItems(std::vector<SubscribeItem>&itms){
    for(auto i:items)
       itms.push_back(i.second);
    return itms.size();
}
const SubscribeItem*Subscriber::find(int64_t tm){/*find subscribe by tm(time_t)*/
    system_clock::time_point tp=time_point<system_clock,seconds>(seconds(tm));
    auto p=items.find(tp);
    return p==items.end()?nullptr:&p->second;
}

void Subscriber::remove(int64_t tm){
    system_clock::time_point tp=time_point<system_clock,seconds>(seconds(tm));
    items.erase(tp); 
}

void Subscriber::remove(system_clock::time_point&point){
    items.erase(point);
}

int Subscriber::load(const std::string&filename){
    Json::Value root;
    Json::String errs;
    Json::CharReaderBuilder builder;
    std::ifstream fin;
    int count=0;
    fin.open(filename);
    bool rc=Json::parseFromStream(builder,fin, &root, &errs);
    Json::Value::Members svcs=root.getMemberNames();
    NGLOG_DEBUG("svcs.size=%d",svcs.size());
    for(auto s:svcs){
       SubscribeItem itm; 
       Json::Value evts=root[s];
       itm.svc=SERVICELOCATORFromString(s.c_str());
       NGLOG_DEBUG("svc:%s=%d.%d.%d",s.c_str(),itm.svc.netid,itm.svc.tsid,itm.svc.sid);
       for(auto e:evts){
           auto t64=std::chrono::seconds(e["time"].asInt64());
           itm.eventid=e["eventid"].asInt();
           itm.type=e["type"].asInt();
           itm.time=time_point<system_clock,seconds>(t64); 
           itm.name=e["name"].asString();
           add(itm);
           count++;
       }
    }
    return count;
}

int Subscriber::save(const std::string&filename){
    Json::Value root;
    Json::String errs;
    Json::StreamWriterBuilder builder;
    std::ofstream fout;
    int comment=0;
    fout.open(filename);
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    for(auto it=items.begin();it!=items.end();it++){
       const std::string svc=SERVICELOCATOR2String(&it->second.svc);
       NGLOG_DEBUG("svc=%s",svc.c_str());
       if(false==root.isMember(svc)){
          Json::Value jssvc(Json::arrayValue);
          root[svc]=jssvc;
          if(0==comment)root[svc].setComment("Service Locator",Json::commentAfterOnSameLine);
       }
       Json::Value evt;
       evt["eventid"]=it->second.eventid;
       evt["type"]=it->second.type;
       evt["time"]=duration_cast<seconds>(it->second.time.time_since_epoch()).count();
       evt["name"]=it->second.name;
       if(0==comment)
          evt.setComment("type: 0-->runOnce,1-->Dialy,2-->Weekly",Json::commentAfterOnSameLine);
       root[svc].append(evt);
       comment++;
    }
    writer->write(root,&fout);
}
#ifdef DEBUG
void subscribe_test(){
    Subscriber&sc=*Subscriber::getInstance();
    SubscribeItem itm;
    itm.svc={1,2,3,4};
    itm.eventid=100;
    itm.name="test once";
    itm.time=system_clock::now()+seconds(10);
    sc.addOnce(itm);
    NGLOG_DEBUG("sc.size=%d",sc.size());

    itm.eventid++;
    itm.name="test daily";
    itm.time+=seconds(10); 
    sc.addDaily(itm);
    NGLOG_DEBUG("sc.size=%d",sc.size());

    itm.eventid++;
    itm.name="test weekly";
    itm.time+=seconds(10); 
    sc.addWeekly(itm);
    NGLOG_DEBUG("sc.size=%d",sc.size());
    sc.save("schedule.json");
    sc.clear();
    NGLOG_DEBUG("sc.size=%d",sc.size());
    sc.load("schedule.json");
    NGLOG_DEBUG("sc.size=%d",sc.size());
    sc.save("schedule1.json");
}
#endif

}
