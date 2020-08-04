#include<assets.h>
#include<algorithm>
#include<ngl_types.h>
#include<ngl_log.h>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/memorystream.h>
#include <rapidjson/error/en.h>
#include <ziparchive.h>
#include <iostream>

NGL_MODULE(Assets);

namespace nglui{

Assets::Assets(const std::string&pakpath){
    pak=new ZIPArchive(pakpath);
    std::vector<std::string>entries;
    pak->getEntries(entries);
    NGLOG_DEBUG("entries.count=%d",entries.size());
}

Assets::~Assets(){
    images.clear();
    strings.clear();
    delete pak;
    NGLOG_DEBUG("%p Destroied",this);
}

std::unique_ptr<std::istream>Assets::getInputStream(const std::string&fname){
     return pak->getInputStream(fname);      
} 

void Assets::loadStrings(const std::string&lan){
     std::vector<std::string>fnames;
     pak->getEntries(fnames);
     for(auto f:fnames){
         rapidjson::Document d;
         std::string flan;
         size_t dpos=f.rfind("."); 
         if(f.compare(0,8,"strings/")||dpos==std::string::npos)continue;
         flan=f.substr(dpos-lan.length(),lan.length());
         if(flan!=lan)continue;
         std::unique_ptr<std::istream>zipis=pak->getInputStream(f);

         NGLOG_DEBUG("load strings from %s",f.c_str());
         rapidjson::IStreamWrapper ims(*zipis);
         d.ParseStream(ims);
         NGLOG_ERROR_IF(d.HasParseError(),"%s Error %s at %d",f.c_str(),GetParseError_En(d.GetParseError()),d.GetErrorOffset());
         if(d.HasParseError()) continue;
         for (rapidjson::Value::MemberIterator m = d.MemberBegin(); m != d.MemberEnd(); ++m){
             strings[m->name.GetString()]=m->value.GetString();
             NGLOG_VERBOSE("%s:%s",m->name.GetString(),m->value.GetString());
         }
     }
}

RefPtr<ImageSurface>Assets::loadImage(std::istream&is){
     return ImageSurface::create_from_stream(is);//bmp/png/jpg/svg is supported
}

RefPtr<ImageSurface>Assets::loadImage(const std::string&resid,bool cache){
     size_t capacity=0;
     std::string resname="drawable/"+resid;
     std::map<const std::string, RefPtr<ImageSurface> >::iterator it = images.end();
     it=std::find_if(images.begin(),images.end(),[&resname](const std::map<std::string,RefPtr<ImageSurface>>::value_type&itr){
         return itr.first == resname;
     });
     std::for_each(images.begin(),images.end(),[&capacity](const std::map<std::string,RefPtr<ImageSurface>>::value_type&it){
          RefPtr<ImageSurface>img=it.second;
          size_t picsize=img->get_width()*img->get_height()*4;
          capacity+=picsize;
          NGLOG_VERBOSE("%dx%d %dK[%s]",img->get_width(),img->get_height(),picsize/1024,it.first.c_str());
     });
     NGLOG_VERBOSE("image cache size=%dK",capacity/1024);
     if(it!=images.end()){
         RefPtr<ImageSurface>refimg(it->second);
         return refimg;
     }
     std::shared_ptr<std::istream>zipis(pak->getInputStream(resname));
     RefPtr<ImageSurface>img;
     if(!zipis->good())return img;

     img=loadImage(*zipis);
     if(cache)
         images.insert(std::map<const std::string,RefPtr<ImageSurface> >::value_type(resname,img));
     NGLOG_VERBOSE("image %s size=%dx%d",resid.c_str(),img->get_width(),img->get_height());
     return img;
}

const std::string Assets::getString(const std::string& id,const std::string&lan){
     if((!lan.empty())&&(osdlanguage!=lan)){
         loadStrings(lan);
     }
     auto itr=strings.find(id);
     if(itr!=strings.end()&&!itr->second.empty()){
          return itr->second;
     }
     return id;
}


}//namespace

