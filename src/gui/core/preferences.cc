#include <preferences.h>
#include <iostream>
#include <strstream>
#include <fstream>
#include <ngl_types.h>
#include <ngl_log.h>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/memorystream.h>

NGL_MODULE(PREFERENCE);

namespace nglui{
Preferences::Preferences(){
   rapidjson::Document*d=new rapidjson::Document();
   d->SetObject();
   doc=d;
   update=0;
}

Preferences::~Preferences(){
   if(update&&!pref_file.empty())
       save(pref_file);
   if(doc)delete (rapidjson::Document*)doc;
   doc=nullptr;
}
void Preferences::load(const std::string&fname){

    rapidjson::Document&d=*(rapidjson::Document*)doc;
    std::ifstream fin(fname);
    rapidjson::IStreamWrapper isw(fin);
    if(fin.good()){
        d.ParseStream(isw);
        pref_file=fname;
    }
    NGLOG_VERBOSE("parse=%s",fname.c_str());
}
void Preferences::save(const std::string&fname){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
    std::ofstream fout(fname);
    rapidjson::OStreamWrapper out(fout);
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(out);
    d.Accept(writer);
}

bool Preferences::getBool(const std::string&section,const std::string&key,bool def){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.IsObject()||!d.HasMember(section))return def;
    if(!d[section].HasMember(key))return def;
    if(!d[section][key].IsBool())return def;
    return d[section][key].GetBool();
}

int Preferences::getInt(const std::string&section,const std::string&key,int def){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.IsObject()||!d.HasMember(section))return def;
    if(!d[section].HasMember(key))return def;
    if(!d[section][key].IsInt())return def;
    NGLOG_VERBOSE("%s.%s=%d",section.c_str(),key.c_str(),d[section][key].GetInt());
    return d[section][key].GetInt();
}

float Preferences::getFloat(const std::string&section,const std::string&key,float def){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.IsObject()||!d.HasMember(section))return def;
    if(!d[section].HasMember(key))return def;
    if(!d[section][key].IsDouble())return def;
    return d[section][key].GetFloat();
}

double Preferences::getDouble(const std::string&section,const std::string&key,double def){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.IsObject()||!d.HasMember(section))return def;
    if(!d[section].HasMember(key))return def;
    if(!d[section][key].IsDouble())return def;
    return d[section][key].GetDouble();
}

const char* Preferences::getString(const std::string&section,const std::string&key,const char*def){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.IsObject()||!d.HasMember(section))return def;
    if(!d[section].HasMember(key))return def;
    if(!d[section][key].IsString())return def;
    return d[section][key].GetString();
}

void Preferences::setValue(const std::string&section,const std::string&key,bool v){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.HasMember(section)){
       rapidjson::Value jsec(section.c_str(),section.size(),d.GetAllocator());
       rapidjson::Value sec(rapidjson::kObjectType);
       d.AddMember(jsec,sec,d.GetAllocator());
    }

    rapidjson::Value jk(key.c_str(),key.size(),d.GetAllocator());
    if(!d[section].HasMember(jk))
           d[section].AddMember(jk,v,d.GetAllocator());
    else
       d[section][jk].SetBool(v);
    update++;
}
void Preferences::setValue(const std::string&section,const std::string&key,int v){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.HasMember(section)){
       rapidjson::Value jsec(section.c_str(),section.size(),d.GetAllocator());
       rapidjson::Value sec(rapidjson::kObjectType);
       d.AddMember(jsec,sec,d.GetAllocator());
    }

    rapidjson::Value jk(key.c_str(),key.size(),d.GetAllocator());
    if(!d[section].HasMember(jk))
           d[section].AddMember(jk,v,d.GetAllocator());
    else
       d[section][jk].SetInt(v);
    NGLOG_DEBUG("%s %s %d",section.c_str(),key.c_str(),v);
    update++;
}
void Preferences::setValue(const std::string&section,const std::string&key,float v){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.HasMember(section)){
       rapidjson::Value jsec(section.c_str(),section.size(),d.GetAllocator());
       rapidjson::Value sec(rapidjson::kObjectType);
       d.AddMember(jsec,sec,d.GetAllocator());
    }

    rapidjson::Value jk(key.c_str(),key.size(),d.GetAllocator());
    if(!d[section].HasMember(jk))
           d[section].AddMember(jk,v,d.GetAllocator());
    else
       d[section][jk].SetFloat(v);

    update++;
}

void Preferences::setValue(const std::string&section,const std::string&key,const char*v){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.HasMember(section)){
       rapidjson::Value jsec(section.c_str(),section.size(),d.GetAllocator());
       rapidjson::Value sec(rapidjson::kObjectType);
       d.AddMember(jsec,sec,d.GetAllocator());
    }

    rapidjson::Value jk(key.c_str(),key.size(),d.GetAllocator());
    rapidjson::Value vv(v,strlen(v),d.GetAllocator());
    if(!d[section].HasMember(jk))
           d[section].AddMember(jk,vv,d.GetAllocator());
    else
       d[section][jk].SetString(v,strlen(v),d.GetAllocator());

    update++;
}
void Preferences::setValue(const std::string&section,const std::string&key,double v){
    rapidjson::Document&d=*(rapidjson::Document*)doc;
    if(!d.HasMember(section)){
       rapidjson::Value jsec(section.c_str(),section.size(),d.GetAllocator());
       rapidjson::Value sec(rapidjson::kObjectType);
       d.AddMember(jsec,sec,d.GetAllocator());
    }

    rapidjson::Value jk(key.c_str(),key.size(),d.GetAllocator());
    if(!d[section].HasMember(jk))
           d[section].AddMember(jk,v,d.GetAllocator());
    else
       d[section][jk].SetDouble(v);

    update++;
}

}//namespace
