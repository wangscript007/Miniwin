#define _Nullable
#define _Nonnull
#include "zipstream.h"
#include <zip.h>
#include <ngl_types.h>
#include <ngl_log.h>

NGL_MODULE(ZIPSTREAM);

namespace nglui {

ZipStreamBuf::ZipStreamBuf()
    : _select(false)
    , _zipfile(NULL)
    , _buffer(NULL) {
}

ZipStreamBuf::~ZipStreamBuf() {
    close();
}

ZipStreamBuf* ZipStreamBuf::select(void* zfile) {
    if (is_open()||zfile==NULL) 
        return NULL;
    _zipfile = zfile;
    // allocate buffer
    _buffer = new char_type[BUFFER_SIZE];
    setg(_buffer, _buffer, _buffer);
    return this;
}

ZipStreamBuf* ZipStreamBuf::close() {
    if (!is_open())
        return NULL;

    if (_buffer) {
        delete[] _buffer;
        _buffer = NULL;
        setg(0, 0, 0);
    }

    if ( zip_fclose((zip_file_t*)_zipfile) != ZIP_ER_OK)
        return NULL;
    _zipfile = NULL;

    return this;
}

std::streamsize ZipStreamBuf::showmanyc(){
    NGLOG_DEBUG("");
    if (!this->is_open() || !(io_mode & std::ios_base::in))
       return -1;
    // Make sure get area is in use
    if (this->gptr() && (this->gptr() < this->egptr()))
        return std::streamsize(this->egptr() - this->gptr());
    else
        return 0;
}

std::streambuf::int_type ZipStreamBuf::underflow() {//Unbuffered get
    if (!is_open())
        return traits_type::eof();

    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    int n = zip_fread((zip_file_t*)_zipfile, _buffer, BUFFER_SIZE);
    if (n <= 0) {
        setg(_buffer, _buffer, _buffer);
        return traits_type::eof();
    }
    setg(_buffer, _buffer, _buffer + n);

    return gptr()==egptr()?traits_type::eof():traits_type::to_int_type(*gptr());
}

//std::streambuf::int_type ZipStreamBuf::uflow(){return 0;}

std::streambuf::pos_type  ZipStreamBuf::seekoff(std::streambuf::off_type off, std::ios_base::seekdir way,
    std::ios_base::openmode mode/*ios_base::in | ios_base::out*/){
    NGLOG_DEBUG("_zipfile=%p  off=%lld way=%d",_zipfile,off,way);
    int whence=SEEK_SET;
    switch(way){
    case std::ios_base::beg: whence=SEEK_SET; break;
    case std::ios_base::cur: whence=SEEK_CUR; break;
    case std::ios_base::end: whence=SEEK_END; break; 
    }
    int rc=zip_fseek((zip_file_t*)_zipfile,off,whence);//only worked for uncompressed data
    off=zip_ftell((zip_file_t*)_zipfile);
    setg(_buffer, _buffer, _buffer);
    NGLOG_DEBUG("zip_fseek=%d pos=%lld",rc,off);
    return off;
}

std::streambuf::pos_type  ZipStreamBuf::seekpos(std::streambuf::pos_type pos, 
    std::ios_base::openmode mode/*ios_base::in | ios_base::out*/){
    NGLOG_DEBUG("pos=%lld",pos);
    return pos;
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
