#define _Nullable
#define _Nonnull
#include "zipstream.h"
#include <zip.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <cstdio>

NGL_MODULE(ZipStream);

namespace nglui {

ZipStreamBuf::ZipStreamBuf()
    : _select(false)
    , zipfile(NULL)
    , buffer(NULL) {
}

ZipStreamBuf::~ZipStreamBuf() {
    close();
}

ZipStreamBuf* ZipStreamBuf::select(void* zfile) {
    if (is_open()||zfile==NULL) 
        return NULL;
    this->zipfile = zfile;
    // allocate buffer
    buffer = new char_type[BUFSIZ];
    setg(buffer, buffer, buffer);
    return this;
}

ZipStreamBuf* ZipStreamBuf::close() {
    if (!is_open())
        return NULL;

    if (buffer) {
        delete[] buffer;
        buffer = NULL;
        setg(0, 0, 0);
    }

    if ( zip_fclose((zip_file_t*)zipfile) != ZIP_ER_OK)
        return NULL;
    zipfile = NULL;

    return this;
}

std::streambuf::int_type ZipStreamBuf::underflow() {//Unbuffered get
    if (!is_open())
        return traits_type::eof();

    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    int n = zip_fread((zip_file_t*)zipfile, buffer, BUFFER_SIZE);
    if (n <= 0) {
        setg(buffer,buffer,buffer);
        return traits_type::eof();
    }
    setg(buffer,buffer,buffer + n);

    return gptr()==egptr()?traits_type::eof():traits_type::to_int_type(*gptr());
}

std::streambuf::pos_type  ZipStreamBuf::seekoff(std::streambuf::off_type off, std::ios_base::seekdir way,
    std::ios_base::openmode mode/*ios_base::in | ios_base::out*/){
    NGLOG_VERBOSE("zipfile=%p  off=%lld way=%d",zipfile,off,way);
    int whence=SEEK_SET;
    switch(way){
    case std::ios_base::beg: whence=SEEK_SET; break;
    case std::ios_base::cur: whence=SEEK_CUR; break;
    case std::ios_base::end: whence=SEEK_END; break; 
    }
    int rc=zip_fseek((zip_file_t*)zipfile,off,whence);//only worked for uncompressed data
    off=zip_ftell((zip_file_t*)zipfile);
    setg(buffer,buffer,buffer);
    NGLOG_VERBOSE("zip_fseek=%d pos=%lld",rc,off);
    return off;
}

std::streambuf::pos_type  ZipStreamBuf::seekpos(std::streambuf::pos_type pos, 
    std::ios_base::openmode mode/*ios_base::in | ios_base::out*/){
    NGLOG_VERBOSE("pos=%lld",pos);
    return seekoff(pos,std::ios_base::beg,mode);
}

ZipInputStream::ZipInputStream(void*zfile) : std::istream(NULL) {
    init(&_sb);
    select(zfile);
}

void ZipInputStream::select(void*zfile) {
    if (!_sb.select(zfile)) {
        setstate(std::ios_base::failbit);
    } else {
        clear();
    }
}

void ZipInputStream::close() {
    if (!_sb.close()) {
        setstate(std::ios_base::failbit);
    }
}

}//namespace
