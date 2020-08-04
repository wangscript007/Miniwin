#include<docview.h>
#include <strings.h>
namespace nglui{

DocumentView::DocumentView(int width, int height)
 :View(width,height){
  index_=-1;
}
bool DocumentView::loadDocument(const std::string&fname){
    int rc=1;
    std::string ext=fname.substr(fname.rfind(".")+1);
    if(strcasecmp(ext.c_str(),"pdf")==0){
#ifdef CAIRO_HAS_PDF_SURFACE
         doc_surface=PdfSurface::create(fname.c_str(),1280,720);
#endif
    }else if((rc=strcasecmp(ext.c_str(),"png"))==0){
#ifdef CAIRO_HAS_PNG_FUNCTIONS
         doc_surface=ImageSurface::create_from_png(fname.c_str());
#endif
    }else if((rc=strcasecmp(ext.c_str(),"jpg"))==0||(rc=strcasecmp(ext.c_str(),"jpeg"))==0){
#ifdef CAIRO_HAS_JPEG_SURFACE
         doc_surface=ImageSurface::create_from_jpg(fname.c_str());
#endif
    }
    //else  doc_surface.clear();
    return rc==0;
}

void DocumentView::onDraw(GraphContext&canvas){
    if(doc_surface)
        setBgPattern(SurfacePattern::create(doc_surface));
    else
        setBgColor(0x000000);        
    canvas.rectangle(0,0,getWidth(),getHeight());
}

void DocumentView::setPlaylist(const std::vector<std::string>&pl){
    playlist_=pl;
}

void DocumentView::first(){
    if(playlist_.size()){
        index_=0;
        loadDocument(playlist_[index_]);
    }
}

void DocumentView::prev(){
    if(index_>0){
       index_--;
       loadDocument(playlist_[index_]);
    }
}

void DocumentView::next(){
    if(index_<playlist_.size()-1){
        index_++;
        loadDocument(playlist_[index_]);
    }
}

void DocumentView::last(){
    if(playlist_.size()){
        index_=playlist_.size()-1;
        loadDocument(playlist_[index_]);
    }
}

}//namespace nglui

