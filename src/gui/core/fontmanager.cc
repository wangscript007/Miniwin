#include <fontmanager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <canvas.h>
NGL_MODULE(FONTMANAGER);

namespace nglui{
    //http://www.verysource.com/code/5938830_1/GUI_TTF.c.html
    FontManager*FontManager::mInst=nullptr;

    FT_Error FontManager::FaceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface){
        FontManager*fm=(FontManager*)request_data;
        Canvas::LOGFONT*lf=(Canvas::LOGFONT*)face_id;
        std::string fntfile=fm->families.begin()->second;
        if(lf&&strlen(lf->lfFaceName)){
            std::map<std::string,std::string>::iterator it=fm->families.find(lf->lfFaceName);///usr/lib/fonts/droid_chn.ttf
            if(it!=fm->families.end())
                fntfile=it->second;
        }
        return FT_New_Face(fm->library,fntfile.c_str(), 0, aface);
    }

    FontManager::FontManager(){
        FT_Init_FreeType(&library);
        FTC_Manager_New(library,8,0,0,FaceRequester,(FT_Pointer)this, &cache_manager);
        FTC_ImageCache_New(cache_manager,&img_cache);
        FTC_CMapCache_New(cache_manager, &cmap_cache);
        FTC_SBitCache_New(cache_manager, &sbit_cache);
    }
     
    FontManager&FontManager::getInstance(){
        if(mInst==nullptr)
            mInst=new FontManager();
        return *mInst;
    }

    int FontManager::loadFonts(const char*path){
        int rc=0;
        struct stat st;
        if(stat(path,&st)==-1)
            return 0;
        if( S_ISREG(st.st_mode) ){
             FT_Face face; 
             if( FT_New_Face(library,path,0,&face)!=FT_Err_Ok)
                return 0;
             families[face->family_name]=path;
             NGLOG_DEBUG("font[%s]-->%s",face->family_name,path);
             FT_Done_Face(face);
             return 1;
        }
        DIR *dir=opendir(path);
        struct dirent *entry;
        while(dir&&(entry=readdir(dir))){
            std::string s=path;
            if(entry->d_name[0]=='.')
               continue;
            s.append("/");
            s.append(entry->d_name);
            rc+=loadFonts(s.c_str());
        }
        if(dir)
            closedir(dir);
        return rc;  
    }

    int FontManager::loadFonts(const char**dirs,int count){
        int rc=0;
        for(int i=0;i<count;i++){
            FT_Face face;
            if(FT_New_Face(library,dirs[i],0,&face)==FT_Err_Ok){
                families[face->family_name]=dirs[i];
                FT_Done_Face(face);
                rc++;
            }
        }
        return rc;
    }
   int FontManager::getFamilies(std::vector<std::string>&fms){
       for(auto f:families)
          fms.push_back(f.first);
       return fms.size();
   }

   std::string FontManager::getFontFile(const std::string &family){
       return families[family];
   }

    FT_Error FontManager::getFace(FTC_FaceID face_id,FT_Face*aface){
        FT_Error err;
        FT_Size size;
        FTC_ScalerRec_ scaler;
        Canvas::LOGFONT*lf=(Canvas::LOGFONT*)face_id;
        err=FTC_Manager_LookupFace(cache_manager,face_id,aface);
        scaler.face_id = face_id;
        scaler.width   = lf->lfWidth;
        scaler.height  = lf->lfHeight;
        scaler.pixel   = 1;
        err=FTC_Manager_LookupSize(cache_manager, &scaler, &size);
        *aface=size->face;
        return err;
    }
    unsigned int FontManager::getGraphIndex(FT_Face face,unsigned int c){
        for (unsigned i=0; i<face->num_charmaps; i++){  
           if(face->charmaps[i]->encoding == FT_ENCODING_UNICODE){  
              return  FTC_CMapCache_Lookup(cmap_cache,(FTC_FaceID)nullptr, i, c);  
           }   
        }  
        return FTC_CMapCache_Lookup(cmap_cache,(FTC_FaceID)nullptr, 0, c); 
    }

    FT_Error FontManager::getCharBitmap(FTC_FaceID face_id,unsigned int gidx,FTC_SBit* sbit){
        FTC_ScalerRec_ scaler;
        FTC_Node node;
        Canvas::LOGFONT*lf=(Canvas::LOGFONT*)face_id;
        scaler.face_id = face_id;
        scaler.width   = lf->lfWidth;
        scaler.height  = lf->lfHeight;
        scaler.pixel   = 1;
        
        return FTC_SBitCache_LookupScaler(sbit_cache,&scaler,FT_LOAD_RENDER,gidx,sbit,&node);
    }
}//namespace
