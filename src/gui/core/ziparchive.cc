#include <ziparchive.h>
#include <zipstream.h>
#include <zip.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <chrono>
#include <ngl_types.h>
#include <ngl_log.h>
using namespace std::chrono;

NGL_MODULE(ZIPArchive);

namespace nglui{

ZIPArchive::ZIPArchive(const std::string&fname){
     int flags=0?(ZIP_CREATE|ZIP_TRUNCATE):(ZIP_CHECKCONS|ZIP_RDONLY);
     zip=zip_open(fname.c_str(),flags,nullptr);
     method=ZIP_CM_DEFAULT;
}

ZIPArchive::~ZIPArchive(){
    zip_close((zip_t*)zip);
}

int ZIPArchive::getEntries(std::vector<std::string>&entries){
    int num=zip_get_num_entries((zip_t*)zip,ZIP_FL_UNCHANGED);
    for(int i=0;i<num;i++){
        const char*name=zip_get_name((zip_t*)zip,i,0);
        entries.push_back(name);
    }
    return num;
}

std::unique_ptr<std::istream> ZIPArchive::getInputStream(const std::string&fname){
    zip_file_t*zfile=zip_fopen((zip_t*)zip,fname.c_str(),ZIP_RDONLY);//ZIP_FL_ENC_UTF_8
    NGLOG_VERBOSE("zfile=%p [%s",zfile,fname.c_str());
    std::unique_ptr<std::istream>zs(new ZipInputStream(zfile));
    return zs;
}

void ZIPArchive::remove(const std::string&fname){
    zip_int64_t idx=zip_name_locate((zip_t*)zip,fname.c_str(),0);
    zip_delete((zip_t*)zip,idx);
}

}

