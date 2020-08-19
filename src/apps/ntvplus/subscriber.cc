#include <subscriber.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <json/json.h>
#include <fstream>

NGL_MODULE(Subscribe);

namespace nglui{

void SubscribeItem::onTriggered(){
   NGLOG_INFO("%d.%d.%d::%d [%p][%s]",svc.netid,svc.tsid,svc.sid,eventid,this,name.c_str());
}

Subscriber*Subscriber::mInst=nullptr;
Subscriber*Subscriber::getInstance(){
    if(nullptr==mInst)
       mInst=new Subscriber();
    return mInst;
}
int Subscriber::addOnce(SubscribeItem&itm){
    auto p=items.insert(std::make_pair(itm.time,itm));
    schedule(std::bind(&SubscribeItem::onTriggered,&p.first->second),itm.time);
}

int Subscriber::addDaily(SubscribeItem&itm){
    auto p=items.insert(std::make_pair(itm.time,itm));
    scheduleDaily(std::bind(&SubscribeItem::onTriggered,&p.first->second),itm.time);
}

int Subscriber::addWeekly(SubscribeItem&itm){
    auto p=items.insert(std::make_pair(itm.time,itm));
    scheduleWeekly(std::bind(&SubscribeItem::onTriggered,&p.first->second),itm.time);
}

int Subscriber::load(const std::string&filename){
    Json::Value root;
    Json::String errs;
    Json::CharReaderBuilder builder;
    std::ifstream fin;
    fin.open(filename);
    bool rc=Json::parseFromStream(builder,fin, &root, &errs);
    Json::Value::Members svcs=root.getMemberNames();
    for(auto s:svcs){
       Json::Value evts=root[s];
       for(auto e:evts){
       }
    }
}

int Subscriber::save(const std::string&filename){
    Json::Value root(Json::arrayValue);
    Json::String errs;
    Json::StreamWriterBuilder builder;
    std::ofstream fout;
    fout.open(filename);
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    for(auto it=items.begin();it!=items.end();it++){
       std::ostringstream  oss;
       oss<<it->second.svc.netid<<"."<<it->second.svc.tsid<<"."<<it->second.svc.sid;
       std::string svc=oss.str();
       if(false==root.isMember(svc)){
          Json::Value jssvc(Json::arrayValue);
          root[svc].append(jssvc);
       }
       Json::Value jssvc=root[svc];
       Json::Value evt;
       evt["eventid"]=Json::Value(it->second.eventid);
       evt["name"]=Json::Value(it->second.name);
       root[svc].append(evt);
    }
    writer->write(root,&fout);
}

}
