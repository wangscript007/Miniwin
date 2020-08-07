#ifndef __LZ4STREAM_H__
#define __LZ4STREAM_H__
#include <cassert>
#include <array>
#include <iostream>
#include <memory>
#include <streambuf>
#include <vector>

class LZ4OutputStream : public std::ostream {
private:
    class LZ4OutputBuffer * buffer_;
public:

    LZ4OutputStream ( std::ostream & sink, const int compression_level_ = 4 );
    ~LZ4OutputStream ();
    void close ( );
};

class LZ4InputStream : public std::istream {
private:
    class LZ4InputBuffer * buffer_;
public:
    LZ4InputStream ( std::istream & source ); 
    ~LZ4InputStream ( ) { delete buffer_; }
};
#endif
