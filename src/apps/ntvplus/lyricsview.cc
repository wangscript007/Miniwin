#include <lyricsview.h>
#include <assets.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <id3/getid3.h>
#include <id3/id3v2.h>
#include <id3/getid3v2.h>
#include <id3/getlyr3.h>
#include <ngl_log.h>
#include <istream>

NGL_MODULE(LyricsView);

namespace nglui{

LyricsView::LyricsView(const std::string& text):TextView(text){
}

LyricsView::LyricsView(const std::string& text, int width, int height)
:TextView(text,width,height){
}

RefPtr<ImageSurface> LyricsView::getImage(const std::string&picname){
    return id3.getImage(picname);
}

const std::string LyricsView::getTitle()const{
    return std::string(" Title :")+id3.getTitle();
}

const std::string LyricsView::getArtist()const{
    return std::string("Artist:")+id3.getArtist();
}

const std::string LyricsView::getAlbum()const{
    return id3.getAlbum();
}

const std::string LyricsView::getYear()const{
    return std::string("Date:")+id3.getYear();
}

const std::string LyricsView::getComment()const{
    return std::string("Comment:")+id3.getComment();
}

const std::string LyricsView::getTrack()const{
    return id3.getTrack();
}

const std::string LyricsView::getGenre()const{
    return std::string("Genre :")+id3.getGenre();
}

View& LyricsView::setText(const std::string&txt){
    mText=txt;
    id3.setUrl(txt);
    invalidate(nullptr);
    return *this;
}

void LyricsView::onDraw(GraphContext& canvas){
    TextView::onDraw(canvas);
    if(id3){
         int y=0;
         RECT rect=getClientRect(); 
         RefPtr<ImageSurface> img=getImage();
         if(img){
             canvas.draw_image(img,&rect,nullptr,ST_FIT_START);
             y=img->get_height()-getFontSize();
         }
         canvas.set_color(getFgColor());
         canvas.draw_text(10,y,getTitle());  y+=26;
         canvas.draw_text(10,y,getArtist()); y+=26;
         canvas.draw_text(10,y,getGenre()+std::string("  ")+getYear()); y+=26;
         canvas.draw_text(10,y,getComment());
    }
}

void LyricsView::setTime(UINT timems){
}

}//namespace
