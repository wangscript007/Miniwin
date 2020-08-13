#ifndef __ZIP_STREAM_H__
#define __ZIP_STREAM_H__
#include <istream>

namespace nglui{

class ZipStreamBuf : public std::streambuf {
public:
  ZipStreamBuf();
  virtual ~ZipStreamBuf();

  bool is_open() const { return zipfile; }
  ZipStreamBuf* select(void* zfile);
  ZipStreamBuf* close();
  virtual pos_type  seekoff(off_type,std::ios_base::seekdir,std::ios_base::openmode /*__mode*/ = std::ios_base::in | std::ios_base::out)override;
  virtual pos_type  seekpos(pos_type,std::ios_base::openmode /*__mode*/ = std::ios_base::in | std::ios_base::out)override;
protected:
   int_type underflow()override;
private:
  static const unsigned BUFFER_SIZE = BUFSIZ;
  ZipStreamBuf(const ZipStreamBuf&);
  ZipStreamBuf& operator=(const ZipStreamBuf&);
  std::ios_base::openmode io_mode;
  bool _select;
  void* zipfile;
  bool own_file;
  char_type* buffer;
};

class ZipInputStream : public std::istream {
public:
  ZipInputStream(void*zfile);
  ZipStreamBuf* rdbuf() const { return const_cast<ZipStreamBuf*>(&_sb); }
  bool is_open() { return _sb.is_open(); }
  void close();
  void select(void*zfile);
private:
  ZipStreamBuf _sb;
};

struct MemoryBuf: std::streambuf {
    MemoryBuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};
struct MemoryInputStream: virtual MemoryBuf, std::istream {
    MemoryInputStream(char const* base, size_t size)
        : MemoryBuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};

}
#endif
