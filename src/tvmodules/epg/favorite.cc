#include <favorite.h>
#include <si_table.h>
#include <tvdata.h>
#include <string>
#include <vector>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/memorystream.h>
#include <iostream>
#include <fstream>

NGL_MODULE(FAVORITE);

typedef struct FavGroup{
   int id;
   std::string grpname;
   std::vector<SERVICELOCATOR>services;
public:
   FavGroup(int id_,const char*name){
      id=id_;
      if(name&&strlen(name))
          grpname=name;
   }
}FAVGROUP; 

static std::vector<FAVGROUP>favgroups;

int FavInit(const char*favpath){
   NGLOG_DEBUG("sizeof SERVICELOCATOR=%d",sizeof(SERVICELOCATOR));
   NGLOG_DEBUG("todo load favorites' data from storage");
   FavAddGroupWithID(FAV_GROUP_ALL,"ALL"); 
   FavAddGroupWithID(FAV_GROUP_AV,"Audio&Video"); 
   FavAddGroupWithID(FAV_GROUP_VIDEO,"Video"); 
   FavAddGroupWithID(FAV_GROUP_AUDIO,"Audio"); 
   DtvCreateSystemGroups();
   DtvCreateGroupByBAT();
   if(favpath)
       FavLoadData(favpath);
   return favgroups.size();
}

int FavSaveData(const char*fname){
    rapidjson::Document d;
    std::ofstream fout(fname);
    d.SetArray();
    int idx=0;
    for(auto g:favgroups){
        if(g.id&(GROUP_SYSTEM|GROUP_BAT))continue;
        d[idx]["id"]=g.id;
        d[idx]["name"].SetString(g.grpname.c_str(),d.GetAllocator());
        for(int j=0;j<g.services.size();j++){
             SERVICELOCATOR& s=g.services[j];
             rapidjson::Value js(rapidjson::kObjectType);
             js["netid"]=s.netid;
             js["tsid"]=s.tsid;
             js["sid"]=s.sid;
             d[idx]["services"].PushBack(js,d.GetAllocator());
        }idx++;
    }
    rapidjson::OStreamWrapper out(fout);
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(out);
    d.Accept(writer);
    return idx;
}

int FavLoadData(const char*fname){
    rapidjson::Document d;
    std::ifstream fin(fname);
    rapidjson::IStreamWrapper isw(fin);
    if(!fin.good())
        return 0;
    d.ParseStream(isw);
    NGLOG_DEBUG("json.parse= %s",fname);
    if(!d.IsArray()){
        NGLOG_ERROR("%s not found or format is error",fname);
        return NGL_ERROR;
    }
    for(int i=0;i<d.Size();i++){
        rapidjson::Value& g=d[i];
        UINT favid=g["id"].GetInt();
        if( favid&(GROUP_SYSTEM|GROUP_BAT) ){
             NGLOG_DEBUG("invalid favid %x",favid);
             continue;
        }
        FavAddGroupWithID(favid,g["name"].GetString());
        for(int j=0;j<g["services"].Size();j++){
             SERVICELOCATOR l;
             rapidjson::Value& v=g["services"][j];
             l.netid=v["netid"].GetInt();
             l.tsid=v["tsid"].GetInt();
             l.sid=v["sid"].GetInt();
             FavAddService(favid,&l);
        }
    }
    NGLOG_DEBUG("load %d favorites",d.Size());
    return d.Size();
}

int FavGetGroupCount(){
    return favgroups.size();
}

int FavAddGroup(const char*name){
    bool name_exist=false;
    int favid=0;
    for(auto g:favgroups){
        if(g.grpname.compare(name)==0){
            name_exist=true;
        }
        if( (g.id&(GROUP_SYSTEM|GROUP_BAT))==0){
           favid++;
        }
    } 
    if(name_exist)return NGL_ERROR;
    FAVGROUP grp(favid,name);
    favgroups.push_back(grp);
    return favid;
}

int FavAddGroupWithID(UINT id,const char*name){//only used for module owner
    FAVGROUP grp(id,name);
    for(auto fav:favgroups){
         if(fav.id==id)
            return NGL_ERROR;
    }
    favgroups.push_back(grp);
    return 0;
}

static FAVGROUP*FindFavGroupById(UINT favid){
    for(int i=0;i<favgroups.size();i++){
         if(favgroups[i].id==favid)
            return &favgroups[i];
    }
    return NULL;
}

int FavRemoveGroup(UINT favid){
    for(auto g=favgroups.begin();g!=favgroups.end();g++){
        if((*g).id==favid){
            favgroups.erase(g);
            return 0;
        }
    }
    return NGL_ERROR;
}

int FavGetGroupInfo(int idx,UINT *favid,char*name){
    if(idx<0||idx>=favgroups.size())
        return NGL_ERROR;
    *favid=favgroups[idx].id;
    strcpy(name,favgroups[idx].grpname.c_str());
    return  0;
}

int FavGetGroupName(UINT favid,char*name){
    FAVGROUP*grp=(FAVGROUP*)FindFavGroupById(favid);
    if(grp)strcpy(name,grp->grpname.c_str());
    return grp?NGL_OK:NGL_ERROR;
}

UINT FavGetServiceCount(UINT favid){
    FAVGROUP*grp=(FAVGROUP*)FindFavGroupById(favid);
    return grp==NULL?0:grp->services.size();
}

int FavGetServices(UINT favid,SERVICELOCATOR*svcs,int maxitem){
    FAVGROUP*grp=(FAVGROUP*)FindFavGroupById(favid);
    for(int i=0;grp && (i<grp->services.size()) && (i<maxitem);i++){
         svcs[i]=grp->services[i];
    }
    NGLOG_VERBOSE("Group %x has %d services",favid,grp->services.size());
    return grp==nullptr?0:grp->services.size();
}

int FavGetService(UINT favid,SERVICELOCATOR*svc,int idx){
    FAVGROUP*grp=(FAVGROUP*)FindFavGroupById(favid);
    if(grp&& (idx<grp->services.size()) && (idx>=0) ){
        *svc=grp->services[idx]; 
        return NGL_OK;
    }
    return NGL_ERROR;
}

int FavAddService(UINT favid,const SERVICELOCATOR*svc){
    FAVGROUP*grp=(FAVGROUP*)FindFavGroupById(favid);
    for(int i=0;i<grp->services.size();i++){
        if(grp->services[i]==*svc)return NGL_ERROR;
    }
    NGLOG_VERBOSE("Add %d,%d,%d to Group %x",svc->netid,svc->tsid,svc->sid,favid);
    grp->services.push_back(*svc);
    return 0;
}

int FavRemoveService(UINT favid,const SERVICELOCATOR*svc){
    FAVGROUP*grp=(FAVGROUP*)FindFavGroupById(favid);
    for(auto it=grp->services.begin();it!=grp->services.end();it++){
         if((*it)==*svc){
             grp->services.erase(it);
             break;
         }
    }
    return 0;
}

int FavClearService(UINT favid){
    FAVGROUP*grp=(FAVGROUP*)FindFavGroupById(favid);
    grp->services.clear();
    return 0;
}

