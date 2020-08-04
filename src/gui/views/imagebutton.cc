#include<imagebutton.h>
#include<ngl_log.h>
#include<app.h>
NGL_MODULE(IMAGEBUTTON);
namespace nglui{

ImageButton::ImageButton(int w,int h):ImageButton(std::string(),w,h){

}

ImageButton::ImageButton(const std::string&txt,int w,int h):INHERITED(txt,w,h){
   setFlag(FOCUSABLE|ENABLED);
}

void ImageButton::setHotImage(const std::string&respath){
    if(respath.empty()==false)
        imghot=App::getInstance().loadImage(respath);
    invalidate(nullptr);
}

void ImageButton::onDraw(GraphContext&canvas){
    View::onDraw(canvas);
    RECT rect=getClientRect();
    canvas.set_font_size(getFontSize());
    bool imgonleft=true;
    if((img_->get_width()>=rect.width)||(getFontSize()*mText.size()>=rect.width)){
        imgonleft=false;
        rect.height-=getFontSize();
    }
    bool focused=hasFlag(FOCUSED);
    canvas.draw_image(focused?imghot:img_,&rect,nullptr,imgonleft?ST_CENTER_INSIDE:ST_FIT_START);
    canvas.set_color(getFgColor());
    if(imgonleft){
        rect.offset(img_->get_width(),0);
    }else{
        rect.y+=rect.height;
        rect.height=getHeight()-rect.y;
    }
    if(mText.empty()==false){
        std::string txt=App::getInstance().getString(mText);
        canvas.draw_text(rect,txt,imgonleft?(DT_LEFT|DT_VCENTER):(DT_CENTER|DT_TOP));
    }
}

}
