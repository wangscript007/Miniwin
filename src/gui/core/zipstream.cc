#define _Nullable
#define _Nonnull
#include "zipstream.h"
#include <zip.h>
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

void ZipStreamBuf::seekg(std::streamoff off,int whence){
    zip_fseek((zip_file_t*)_zipfile,off,whence);
    setg(_buffer, _buffer, _buffer);
}

std::streambuf::int_type ZipStreamBuf::underflow() {
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

    return traits_type::to_int_type(*gptr());
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

std::istream& ZipInputStream::seekg (std::streamoff off, std::ios_base::seekdir way){
    int whence=SEEK_SET;
    switch(way){
    case std::ios_base::beg: whence=SEEK_SET; break;
    case std::ios_base::cur: whence=SEEK_CUR; break;
    case std::ios_base::end: whence=SEEK_END; break; 
    }
    rdbuf()->seekg(off,whence);
    return *this;
}

void ZipInputStream::close() {
    if (!_sb.close()) {
        setstate(std::ios_base::failbit);
    }
}

}//namespace
