#include <lz4stream.h>
#include <exception>
#include <functional>
#include <lz4frame.h>
#include <cstdio>

//ref:https://github.com/degski/lz4_stream
//ref:https://github.com/djp952/io-compression/

class LZ4InputBuffer : public std::streambuf {
public:
    LZ4InputBuffer ( std::istream & source );
    ~LZ4InputBuffer ( );

    int_type underflow ( ) override;
    LZ4InputBuffer ( const LZ4InputBuffer & ) = delete;
    LZ4InputBuffer & operator= ( const LZ4InputBuffer & ) = delete;

private:
    std::istream & source_;
    std::array<char, 64 * 1024> src_buf_;
    std::array<char, 64 * 1024> dest_buf_;
    std::size_t offset_;
    std::size_t src_buf_size_;
    LZ4F_decompressionContext_t ctx_;
};

LZ4InputBuffer::LZ4InputBuffer ( std::istream & source ) :
    source_ ( source ), src_buf_ ( ), dest_buf_ ( ), offset_ ( 0 ), src_buf_size_ ( 0 ), ctx_ ( nullptr ) {
    std::size_t ret = LZ4F_createDecompressionContext ( &ctx_, LZ4F_VERSION );
    if ( LZ4F_isError ( ret ) != 0 )
        throw std::runtime_error ( std::string ( "Failed to create LZ4 decompression context: " ) + LZ4F_getErrorName ( ret ) );
    setg ( &src_buf_.front ( ), &src_buf_.front ( ), &src_buf_.front ( ) );
}

std::streambuf::int_type LZ4InputBuffer::underflow ( ) {
    if ( offset_ == src_buf_size_ ) {
        source_.read ( &src_buf_.front ( ), src_buf_.size ( ) );
        src_buf_size_ = static_cast<std::size_t> ( source_.gcount ( ) );
        offset_       = 0;
    }
    if ( src_buf_size_ == 0 )
        return traits_type::eof ( );
    std::size_t src_size  = src_buf_size_ - offset_;
    std::size_t dest_size = dest_buf_.size ( );
    std::size_t ret = LZ4F_decompress ( ctx_, &dest_buf_.front ( ), &dest_size, &src_buf_.front ( ) + offset_, &src_size, nullptr );
    offset_ += src_size;
    if ( LZ4F_isError ( ret ) != 0 )
        throw std::runtime_error ( std::string ( "LZ4 decompression failed: " ) + LZ4F_getErrorName ( ret ) );
    if ( dest_size == 0 )
        return traits_type::eof ( );
    setg ( &dest_buf_.front ( ), &dest_buf_.front ( ), &dest_buf_.front ( ) + dest_size );
    return traits_type::to_int_type ( *gptr ( ) );
}

LZ4InputBuffer::~LZ4InputBuffer(){
    LZ4F_freeDecompressionContext( ctx_ );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

class LZ4OutputBuffer : public std::streambuf {
public:
    LZ4OutputBuffer ( std::ostream & sink, const int compression_level_ );
    ~LZ4OutputBuffer ( );

    LZ4OutputBuffer ( const LZ4OutputBuffer & ) = delete;
    LZ4OutputBuffer & operator= ( const LZ4OutputBuffer & ) = delete;
    void close ( );

private:
    int_type overflow ( int_type ch ) override;
    int_type sync ( ) override;
    void compressAndWrite ( );
    void writeHeader ( );
    void writeFooter ( );

    std::ostream & sink_;
    std::array<char, 256> src_buf_;
    std::vector<char> dest_buf_;
    LZ4F_compressionContext_t ctx_;
    LZ4F_preferences_t preferences_;
    bool closed_;
};

LZ4OutputBuffer::LZ4OutputBuffer ( std::ostream & sink, const int compression_level_ ) :
    sink_ ( sink ), src_buf_{}, preferences_{}, closed_ ( false ) {
    char * base = &src_buf_.front ( );
    setp ( base, base + src_buf_.size ( ) - 1 );
    std::size_t ret = LZ4F_createCompressionContext ( &ctx_, LZ4F_VERSION );
    if ( LZ4F_isError ( ret ) != 0 )
        throw std::runtime_error ( std::string ( "Failed to create LZ4 compression context: " ) + LZ4F_getErrorName ( ret ) );
    preferences_.compressionLevel = compression_level_;
    // TODO: No need to recalculate the dest_buf_ size on each construction
    dest_buf_.resize ( LZ4F_compressBound ( src_buf_.size ( ), &preferences_ ) );
    writeHeader ( );
}

LZ4OutputBuffer::~LZ4OutputBuffer(){ 
    close ( ); 
}

std::streambuf::int_type LZ4OutputBuffer::overflow ( int_type ch ) {
    assert ( std::less_equal<char *> ( ) ( pptr ( ), epptr ( ) ) );
    *pptr ( ) = static_cast<LZ4OutputStream::char_type> ( ch );
    pbump ( 1 );
    compressAndWrite ( );
    return ch;
}

std::streambuf::int_type LZ4OutputBuffer::sync ( ) {
    compressAndWrite ( );
    return 0;
}

void LZ4OutputBuffer::compressAndWrite ( ) {
    assert ( !closed_ );
    std::ptrdiff_t orig_size = pptr ( ) - pbase ( );
    pbump ( -orig_size );
    std::size_t comp_size = LZ4F_compressUpdate ( ctx_, &dest_buf_.front ( ), dest_buf_.size ( ), pbase ( ), orig_size, nullptr );
    sink_.write ( &dest_buf_.front ( ), comp_size );
}

void LZ4OutputBuffer::writeHeader ( ) {
    assert ( !closed_ );
    std::size_t ret = LZ4F_compressBegin ( ctx_, &dest_buf_.front ( ), dest_buf_.size ( ), &preferences_ );
    if ( LZ4F_isError ( ret ) != 0 )
        throw std::runtime_error ( std::string ( "Failed to start LZ4 compression: " ) + LZ4F_getErrorName ( ret ) );
    sink_.write ( &dest_buf_.front ( ), ret );
}

void LZ4OutputBuffer::writeFooter ( ) {
    assert ( !closed_ );
    std::size_t ret = LZ4F_compressEnd ( ctx_, &dest_buf_.front ( ), dest_buf_.size ( ), nullptr );
    if ( LZ4F_isError ( ret ) != 0 )
        throw std::runtime_error ( std::string ( "Failed to end LZ4 compression: " ) + LZ4F_getErrorName ( ret ) );
    sink_.write ( &dest_buf_.front ( ), ret );
}

void LZ4OutputBuffer::close ( ) {
    if ( closed_ )
        return;
    sync ( );
    writeFooter ( );
    LZ4F_freeCompressionContext ( ctx_ );
    closed_ = true;
}

////////////////////////////////////////////////////////////////////////////////
LZ4InputStream::LZ4InputStream ( std::istream & source ) :
    std::istream ( new LZ4InputBuffer ( source ) ), buffer_ ( static_cast<LZ4InputBuffer *> ( rdbuf ( ) ) ) {
    assert ( buffer_ );
}

LZ4OutputStream::LZ4OutputStream(std::ostream & sink, const int compression_level_) :
    std::ostream ( new LZ4OutputBuffer ( sink, compression_level_ ) ),
    buffer_ ( static_cast<LZ4OutputBuffer *> ( rdbuf ( ) ) ) {
    assert ( buffer_ );
}

LZ4OutputStream::~LZ4OutputStream ( ) {
    close ( );
    delete buffer_;
}
void LZ4OutputStream::close ( ) { 
    buffer_->close ( ); 
}
