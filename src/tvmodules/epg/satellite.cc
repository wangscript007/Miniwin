#include <satellite.h>
#include <vector>
#include <ngl_log.h>
#include <dvbepg.h>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include <stdio.h>
#include <diseqc.h>
#include <map>
#include <iostream>
#include <fstream>

NGL_MODULE(SATELLITE);

using namespace std;

typedef struct{
   SATELLITE sat;
   std::vector<TRANSPONDER>tps;
}MYSATELLITE;

static std::vector<MYSATELLITE>gSatellites;
typedef struct{
   const char*name; 
   int value;
}NAME2VAL;

static NAME2VAL  nvlnb[]  ={ {"lnb",0},{"none",0},{"vertical",1},{"horizontal",2},{NULL,0} };
static NAME2VAL  nv22k[]  ={ {"22k",0},{"off",0},{"on",1},{NULL,0} };
static NAME2VAL nvdiseqc[]={ {"diseqc",0},{"none",0},{"off",0},{"A",1},{"B",2},{"C",3},{"D",4},{NULL,0}};
static NAME2VAL nvpolar []={ {"polarity",0},{"none",0},{"off",0},{"horizontal",0},{"vertical",1},{"left",2},{"right",3},{NULL,0}}; 

static int  Name2Value(const std::string&key,const std::string&v){
    NAME2VAL* nvs[]={nvlnb,nv22k,nvdiseqc,nvpolar,nullptr};
    NAME2VAL*nv=NULL;
    for(int i=0;nvs[i]!=nullptr;i++){
         if(key.compare(nvs[i][0].name)==0){
             nv=nvs[i];break;
         }
    }
    for(int i=0;nv[i].name;i++){
         if(v.compare(nv[i].name)==0)
            return nv[i].value;
    }
    return 0;
}

static const char*Value2Name(const std::string key,int v){
    NAME2VAL* nvs[]={nvlnb,nv22k,nvdiseqc,nvpolar,nullptr};
    NAME2VAL*nv=NULL;
    for(int i=0;nvs[i]!=nullptr;i++){
         if(key.compare(nvs[i][0].name)==0){
             nv=nvs[i];break;
         }
    }
    for(int i=1;nv[i].name;i++){
         if(v==nv[i].value)return nv[i].name;
    }
    return NULL;
}

static int getValue(const rapidjson::Value&obj,const std::string&key,int default_value=0){
    char value[128];
    if(!obj.HasMember(key.c_str())){
        NGLOG_ERROR("jsonobject no field %s",key.c_str());
        return default_value;
    }
    if(obj[key].IsString()){
        int v=Name2Value(key,obj[key].GetString());
        NGLOG_VERBOSE("%s=%d",key.c_str(),v);
        return v;
    }else if(obj[key].IsInt()){
        return obj[key].GetInt();
    }else{
        NGLOG_DEBUG("invalid json data");
        return default_value;
    }
}

int LoadSatelliteFromDB(const char*fname){
    FILE*file;
    char buf[256];
    int done=0;
    std::ifstream fin(fname);
    
    NGLOG_DEBUG("load from %s",fname);
    rapidjson::IStreamWrapper isw(fin);
    rapidjson::Document d;
    if(!fin.good())return 0;
    d.ParseStream(isw);
    NGLOG_DEBUG("=========size=%d",d.Size());
    for(int i=0;i<d.Size();i++){
        SATELLITE sat;
        rapidjson::Value&js=d[i];
        memset(&sat,0,sizeof(SATELLITE));
        strncpy(sat.name,js["satellite"].GetString(),MAX_SATELLITE_NAME_LEN);
        sat.position=getValue(js,"position");
        sat.lnb=getValue(js,"lnb");
        sat.k22=getValue(js,"22k");
        sat.diseqc=getValue(js,"diseqc");
        sat.direction=0;
        rapidjson::Value& jstps=js["transponders"];
        AddSatellite(&sat);
        NGLOG_DEBUG("sat:%s lnb:%d diseqc:%d",sat.name,sat.lnb,sat.diseqc);
        for(int j=0;j<jstps.Size();j++){
             rapidjson::Value& jtp=jstps[j];
             TRANSPONDER tp;
             memset(&tp,0,sizeof(TRANSPONDER));
             tp.delivery_type=DELIVERY_S;
             tp.u.s.frequency=getValue(jtp,"frequency");
             tp.u.s.tpid=getValue(jtp,"transponder");
             tp.u.s.symbol_rate=getValue(jtp,"symbolrate");
             tp.u.s.polar=(NGLNimPolar)getValue(jtp,"polarity");//polarity
             NGLOG_DEBUG("\tfreq:%d symb:%d tpid:%d polar:%s",tp.u.s.frequency,tp.u.s.symbol_rate,
                    tp.u.s.tpid,Value2Name("polarity",tp.u.s.polar));
             if(jtp.HasMember("frequency"))
                  AddTp2Satellite(i,&tp);
        }
    }
    return 0;
}

int SaveSatellite2DB(const char*fname){
    rapidjson::Document d;
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
    d.SetArray();
    rapidjson::Value root(rapidjson::kArrayType);
    for(auto isat:gSatellites){
         SATELLITE& s=isat.sat;
         rapidjson::Value js(rapidjson::kObjectType);
         js["satellite"].SetString(s.name,allocator);
         js["position"]=s.position;
         js["lnb"].SetString(Value2Name("lnb",s.lnb),allocator);
         js["22k"].SetString(Value2Name("22k",s.k22),allocator);
         js["diseqc"].SetString(Value2Name("diseqc",s.diseqc),allocator);
         js["direction"]="east";
         for(auto it:isat.tps){
              TRANSPONDER&t=it;
              rapidjson::Value jt(rapidjson::kObjectType);
              jt["transponder"]=t.u.s.tpid;
              jt["frequency"]=t.u.s.frequency;
              jt["symbolrate"]=t.u.s.symbol_rate;
              jt["polarity"].SetString(Value2Name("polarity",t.u.s.polar),allocator);
              js["transponders"].PushBack(jt,allocator); 
         }   
         d.PushBack(js,allocator);
    } 
    std::ofstream fout(fname);
    rapidjson::OStreamWrapper out(fout);
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(out);
    d.Accept(writer);
    return 0; 
}

int GetSatelliteCount(){
    return gSatellites.size();
}

int GetSatelliteInfo(int idx,SATELLITE*sat){
    *sat=gSatellites[idx].sat;
    return 0;
}

int GetSatelliteTp(int satidx,TRANSPONDER*tps,int tp_count){
    int count=0;
    MYSATELLITE&sat=gSatellites[satidx];
    for(int i=0;i<sat.tps.size();i++){
        tps[i]=sat.tps[i];
    }
    return sat.tps.size();
}

int AddSatellite(SATELLITE*sat){
    MYSATELLITE msat;
    msat.sat=*sat;
    gSatellites.push_back(msat); 
    return gSatellites.size();    
}

int RemoveSatellite(int idx){
    std::vector<TRANSPONDER>&tps=gSatellites[idx].tps;
    for(auto t=tps.rbegin();t!=tps.rend();t++)
         RemoveTpFromSatellite(idx,(*t).u.s.tpid);
    gSatellites.erase(gSatellites.begin()+idx);
    return 0;
}

int AddTp2Satellite(int satid,const TRANSPONDER* tp){
    int tpid=-1;
    std::vector<TRANSPONDER>transponders;
    for(auto s:gSatellites){//copy satelitte's tp to var transponders ,and used to get id for new tp
        transponders.insert(transponders.end(),s.tps.begin(),s.tps.end());
    }
    for(int i=0;i<=transponders.size()+1;i++){
        int found=0;
        for(int j=0;(0==found) && (j<transponders.size());j++){
            if(transponders[j].u.s.tpid==i){
                found=1;
            }
        }
        if(!found){
             tpid=i;
             break;
        }
    }
    TRANSPONDER ntp=*tp;
    ntp.u.s.tpid=tpid;
    gSatellites[satid].tps.push_back(ntp);
    return tpid;
}

int RemoveTransponder(int tpid){
    for(auto s:gSatellites){
        std::vector<TRANSPONDER>&tps=s.tps;
        for(auto t=tps.begin();t!=tps.end();t++){
            if(t->u.s.tpid==tpid){
                 tps.erase(t);
                 DtvRemoveStreamByTP(tpid);
                 return NGL_OK;
            }
        } 
    }
    return NGL_ERROR;
}

int RemoveTpFromSatellite(int satid,int tpid){
    std::vector<TRANSPONDER>&tps=gSatellites[satid].tps;
    for(auto t=tps.begin();t!=tps.end();t++){
        if(t->u.s.tpid==tpid){
             tps.erase(t);
             DtvRemoveStreamByTP(tpid);
             return 0;
        }
    } 
    return -1;
}

int GetParamsByTP(int tpid,TRANSPONDER*tp,SATELLITE*sat){
    for(auto s:gSatellites){
       for(auto t=s.tps.begin();t!=s.tps.end();t++){
          if(t->u.s.tpid==tpid){
             if(tp)*tp=*t;
             if(sat)*sat=s.sat;
             return 0;
          }
       }
    }
    return -1;
}

int ConfigureTransponder(const TRANSPONDER*tp){
    SATELLITE sat;
    if(0==GetParamsByTP(tp->u.s.tpid,NULL,&sat)){
        nglTunerSetLNB(0,sat.lnb);
        nglTunerSet22K(0,sat.k22);
        diseqc_set_diseqc10(0,(TUNER_DISEQC10_ENUM)sat.diseqc,
            (TUNER_TP_POLARIZATION_ENUM)tp->u.s.polar,//TUNER_POL_HORIZONTAL,
            (sat.k22?TUNER_TONE_22K_ON:TUNER_TONE_22K_OFF)); 
        NGLOG_DEBUG("TP:%d freq:%d symb:%d lnb:%s 22k:%s diseqc:%s polar:%s",tp->u.s.tpid,tp->u.s.frequency,tp->u.s.symbol_rate,
            Value2Name("lnb",sat.lnb),Value2Name("22k",sat.k22),Value2Name("diseqc",sat.diseqc),Value2Name("polarity",tp->u.s.polar));
        return 0;
    }
    return -1;
}

int ConfigureTransponderById(int tpid){
    int rc;
    SATELLITE sat;
    TRANSPONDER tp;
    if(0==GetParamsByTP(tpid,&tp,&sat)){
        nglTunerSetLNB(0,sat.lnb);
        nglTunerSet22K(0,sat.k22);
        diseqc_set_diseqc10(0,(TUNER_DISEQC10_ENUM)sat.diseqc,
            (TUNER_TP_POLARIZATION_ENUM)tp.u.s.polar,//TUNER_POL_HORIZONTAL,
            (sat.k22?TUNER_TONE_22K_ON:TUNER_TONE_22K_OFF)); 
        NGLOG_DEBUG("TP:%d lnb:%s freq:%d symb:%d 22k:%s diseqc:%s polar:%s",tpid,tp.u.s.frequency,tp.u.s.symbol_rate,
            Value2Name("lnb",sat.lnb),Value2Name("22k",sat.k22),Value2Name("diseqc",sat.diseqc),Value2Name("polarity",tp.u.s.polar));
    }
    return -1;
}

