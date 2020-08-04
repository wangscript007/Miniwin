#include <imageview.h>
#include <app.h>
namespace nglui{

ImageView::ImageView(const std::string& path)
  : ImageView(path,0,0){
}

ImageView::ImageView(int w, int h):ImageView(std::string(),w,h){
}

ImageView::ImageView(const std::string& path, int w, int h)
  : INHERITED(path,w,h){
    //internal_name_ = VIEW_NAME;
   scale_type_=ST_CENTER_INSIDE;//MATRIX;
}

ImageView::~ImageView() {
}

int ImageView::getScaleType(){
    return scale_type_;
}

void ImageView::setScaleType(int st){
    scale_type_=st;
    invalidate(nullptr);
}

void ImageView::setImage(const std::string&respath){
    if(respath.empty()==false)
        img_=App::getInstance().loadImage(respath);
}

void ImageView::onDraw(GraphContext& canvas) {
    INHERITED::onDraw(canvas);
    RECT rect=getClientRect();
    if(img_!=nullptr)
        canvas.draw_image(img_,&rect,nullptr,(SCALETYPE)scale_type_);
}

const SIZE& ImageView::getPreferSize() {
    mPreferSize.set(getWidth(), getHeight());
    return mPreferSize;
}

}

