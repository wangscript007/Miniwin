#include <unistd.h>
#include <cctype> 
#include <iostream>
#include <fstream>
#include <algorithm>
#include <json/json.h>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/memorystream.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/error/en.h>
#include "rapidjson/filewritestream.h"

#if 0
int jsoncppsplit(int argc,char*argv[]){
    Json::Value root;
    Json::String errs;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::ifstream sin(argv[1]);
    bool rc=Json::parseFromStream(builder,sin, &root, &errs);//reader.parse(data,data+size,root);
    if(!root.isArray()){
        printf("invalid language file\r\n");
        return -1;
    }
    Json::Value jlans;
    for(int i=0;i<root.size();i++){
         Json::Value v=root[i];
         Json::Value::Members mems=v.getMemberNames();
         if(!v.isMember("valuename")||mems.size()<2){
             printf("invalid language format\r\n");
             continue;
         }
         std::string strname=v["valuename"].asString();
         std::transform(strname.begin(), strname.end(), strname.begin(), ::tolower);
         if(strname.empty())continue;
         for(int j=0;j<mems.size();j++){
              if(mems[j]==strname)continue;
              jlans[mems[j]][strname]=v[mems[j]];
         }
    }
    Json::Value::Members lannames=jlans.getMemberNames();
    for(int i=0;i<lannames.size();i++){
        if(lannames[i]=="valuename")continue;
        std::string ofname="strings-"+lannames[i]+".json";
        Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "None";
        std::transform(ofname.begin(), ofname.end(), ofname.begin(), ::tolower);
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        std::ofstream fout(ofname);
        writer->write(jlans[lannames[i]],&fout);
    }
}
#endif
int rapidsplit(int argc,char*argv[]){
    int format=0;
    using namespace rapidjson;
    rapidjson::Document d;
    int opt;
    char jsfilename[512];
    while ((opt = getopt(argc, argv, "f")) != -1){
       switch(opt){
       case 'f':format=1;break;
       }
    }
    strcpy(jsfilename,argv[optind]);
    std::ifstream fin(jsfilename);
    rapidjson::IStreamWrapper isw(fin);
    if(fin.good()){
        d.ParseStream(isw);
    }else printf("%s opened failed\r\n",jsfilename);

    if(d.HasParseError())
        printf("Error %s at %d\r\n",GetParseError_En(d.GetParseError()),d.GetErrorOffset());
    std::map<std::string,rapidjson::Value*>jlans;//(rapidjson::kArrayType);

    printf("%d isarray=%d format=%d\r\n",d.Size(),d.IsArray(),format);
    for(int i=0;i<d.Size();i++){
        rapidjson::Value &v=d[i];
        std::string strname=v["valuename"].GetString();

        if(strname.empty())      continue;

        std::transform(strname.begin(),strname.end(),strname.begin(), tolower);

        for(rapidjson::Value::ConstMemberIterator itr=v.MemberBegin();itr<v.MemberEnd();itr++){
            std::string value=itr->name.GetString();
            if(value.compare("valuename")==0)continue;
            if(jlans[value]==nullptr){
               jlans[value]=new rapidjson::Value(rapidjson::kObjectType);
            }
            rapidjson::Value jsn(strname.c_str(),strname.size(),d.GetAllocator());
            rapidjson::Value &jsv=v[value];
            jlans[value]->AddMember(jsn,jsv,d.GetAllocator());
            
        }
    }
    for(auto j=jlans.begin();j!=jlans.end();j++){
       rapidjson::StringBuffer buffer;
       char *ps,fname[512];
       sprintf(fname,"strings-%s.json",j->first.c_str());
       char writeBuffer[65536];
       for(ps=fname;*ps;ps++)*ps=tolower(*ps);
       FILE*fj=fopen(fname,"w");
       printf("%s has %d strings\r\n",fname,j->second->MemberCount());
       FileWriteStream os(fj, writeBuffer, sizeof(writeBuffer));
       if(format){
          rapidjson::PrettyWriter<FileWriteStream> writer(os);
          j->second->Accept(writer);
       }else{
          rapidjson::Writer<FileWriteStream> writer(os);
          j->second->Accept(writer);
       }
    }
    return 0;
}

int main(int argc,char*argv[]){
    return rapidsplit(argc,argv);
}
