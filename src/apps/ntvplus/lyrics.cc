#include <lyrics.h>
#include <assets.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <id3/getid3.h>
#include <id3/id3v2.h>
#include <id3/getid3v2.h>
#include <id3/getlyr3.h>
#include <ngl_types.h>
#include <ngl_log.h>
#include <istream>
#include <iconv.h>
#ifdef HAVE_CODECVT
#include <locale>
#include <codecvt>
#endif

NGL_MODULE(Lyrics);

using namespace tag;
namespace priv{

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
int code_convert(const char*from_charset,const char*to_charset,const char*inbuf,size_t inlen,char*outbuf,size_t outlen){
         char **pin = (char**)&inbuf;
         char **pout = &outbuf;
         iconv_t cd = iconv_open(to_charset,from_charset);
         if (cd==0) return -1;
         memset(outbuf,0,outlen);
         iconv(cd, pin, &inlen,pout, &outlen);
         iconv_close(cd);
         return 0;

}

inline static const char* UCS(){
    union { unsigned short bom; unsigned char byte; } endian_test;
    endian_test.bom = 0xFFFE;
    if(sizeof(wchar_t) == 4 && endian_test.byte == 0xFE)
        return "UCS-4LE";
    else if(sizeof(wchar_t) == 2 && endian_test.byte == 0xFE)
        return "UCS-2LE";
    else if(sizeof(wchar_t) == 4 && endian_test.byte != 0xFE)
         return "UCS-4BE";
    else if(sizeof(wchar_t) == 2 && endian_test.byte != 0xFE)
        return "UCS-2BE";
    else
        return "ASCII";
}

const std::string W2S(const stredit::function::result&w){
#ifdef HAVE_CODECVT
    std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;
    return strCnv.to_bytes(w);
#else
    char u8bytes[256];
    code_convert(UCS(),"UTF-8",(const char*)w.data(),w.length()*sizeof(wchar_t),u8bytes,sizeof(u8bytes));
    return u8bytes;
#endif
//  return charset::conv<char>::encode(w.data(),w.length()*sizeof(wchar_t));

}

const char *membrk0(const char *buf, size_t size, int wide){
    const char* const end = buf + size - wide;
    const int step = 1+wide;
    for( ; buf < end; buf += step) {
        if(!buf[0] && !buf[wide])
            return buf;
    }
    return 0;
}
}

namespace nglui{

Lyrics::Lyrics(){
   NGLOG_DEBUG("wchar_t=%d",sizeof(wchar_t));
}
Lyrics::Lyrics(const std::string& text){
   url=text;
   hasid3=false;
   parseLyrics(text);
   parseID3(text); 
   getImage(text);
   NGLOG_DEBUG("wchar_t=%d",sizeof(wchar_t));
}


static const char* picture_types[] = {
    "other",    "icon",    "other_icon",    "front_cover",    "back_cover",
    "leaflet",    "media",    "lead_artist",    "artist",    "conductor",
    "band",    "composer",    "lyricist",    "location",    "recording",
    "performance",    "screencap",    "red_herring",    "illustration",
    "logotype",    "studio_logotype" 
};

int Lyrics::loadImages(){
    void *tag = ID3_readf(url.c_str(), 0);
    ID3FRAME f;
    int counter=0;
    NGLOG_DEBUG("%s",url.c_str());
    if(tag==nullptr)return 0;
    if(ID3_start(f,tag) >= 2) {
        while(ID3_frame(f)) {
            if(strcmp(f->ID, "APIC") == 0) {
                /* see ID3v2.3 4.15 -- 'Attached Picture' for reference */
                char wide = f->data[0] == 1 || f->data[0] == 2;
                const char *mime_type = f->data+1;
                const char *type = (const char*)memchr(mime_type, 0, f->size-(2+wide));
                const char *descr, *blob;
                if(!type || (type[1]&0xFFu) > sizeof picture_types/sizeof *picture_types) {
                     NGLOG_DEBUG("%s has an incorrect ID3v2 tag!\n",url.c_str());
                     continue;
                } else {
                     ++type;          /* skip terminator */
                }
                descr = type+1;
                blob = priv::membrk0(descr, f->size-(descr-f->data), wide);
                if(!blob) {
                     NGLOG_DEBUG("%s has an incorrect ID3v2 tag!\n",url.c_str());
                     continue;
                } else {
                     blob += 1+wide;  /* skip terminator */
                }
                priv::MemoryInputStream stm(blob,f->size-(blob-f->data));
                counter++;
                RefPtr<ImageSurface>image=Assets::loadImage(stm);
                images[picture_types[type[1]&0xFF]]=image;
                NGLOG_DEBUG("%s:imagesize=%dx%d",picture_types[type[1]&0xFF],image->get_width(),image->get_height());
           }
       }//while
       ID3_free(tag);
   }//if
   return counter;
}

RefPtr<ImageSurface>Lyrics::getImage(const std::string&name)const{
   if(name.empty()&&images.size())
      return images.begin()->second; 
   if(images.size())return images.at(name);
}

Lyrics::operator bool()const{
    return hasid3;
}

int Lyrics::parseID3(const std::string&txt){
    struct stat stbuf;
    if(stat(txt.c_str(),&stbuf)!=0)return 0;

    tag::read::ID3v2 v2(txt.c_str());
    tag::read::ID3 v1(txt.c_str());

    tag::metadata*id3tag=nullptr;
    if(v2)
       id3tag=&v2;
    else if(v1)
       id3tag=&v1;

    if(id3tag){
       hasid3=true;
       id3title=priv::W2S((*id3tag)[tag::title]); 
       id3artist=priv::W2S((*id3tag)[tag::artist]); 
       id3album=priv::W2S((*id3tag)[tag::album]); 
       id3year=priv::W2S((*id3tag)[tag::year]); 
       id3comment=priv::W2S((*id3tag)[tag::cmnt]); 
       id3track=priv::W2S((*id3tag)[tag::track]); 
       id3genre=priv::W2S((*id3tag)[tag::genre]); 
    }
    NGLOG_DEBUG("title:%s\r\nartist:%s\r\nalbum:%s\r\nyear:%s\r\ngenre:%s",id3title.c_str(),
        id3artist.c_str(),id3album.c_str(),id3year.c_str(),id3genre.c_str());
    NGLOG_DUMP("TITLE:",(const BYTE*)id3title.c_str(),id3title.size());
}

const std::string& Lyrics::getTitle()const{
    return id3title;
}

const std::string& Lyrics::getArtist()const{
    return id3artist;
}

const std::string& Lyrics::getAlbum()const{
    return id3album;
}

const std::string& Lyrics::getYear()const{
    return id3year;
}

const std::string& Lyrics::getComment()const{
    return id3comment;
}

const std::string& Lyrics::getTrack()const{
    return id3track;
}

const std::string& Lyrics::getGenre()const{
    return id3genre;
}

int Lyrics::parseLyrics(const std::string&txt){
    struct stat stbuf;
    std::string lyricstxt=txt;
    if(stat(txt.c_str(),&stbuf)==0){
        tag::read::Lyrics3 ly(txt.c_str());
        NGLOG_DEBUG("getlyrics from %s %d",txt.c_str(),(bool)ly);
        if(ly){
           auto lst=ly.listing();//typedef std::vector< std::pair<std::string, value_string> > array
           for(auto l:lst)
               NGLOG_DEBUG("%s::%s",l.first.c_str(),priv::W2S(l.second).c_str());
           return lst.size();
        }
    }

    void *tag = ID3_readf(txt.c_str(), 0);
    ID3FRAME f;
    NGLOG_DEBUG("%s",txt.c_str());
    if(tag==nullptr)return 0;
    if(ID3_start(f,tag) >= 2) {
        int counter=0;
        while(ID3_frame(f)) {
        }//while
    }
    ID3_free(tag);
    return 0;
}

void Lyrics::setUrl(const std::string&txt){
    url=txt;
    parseLyrics(txt);
    parseID3(txt);
    loadImages();
}

}//namespace
