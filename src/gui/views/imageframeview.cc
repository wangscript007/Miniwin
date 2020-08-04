#include<imageframeview.h>
#include<app.h>
#include<ngl_log.h>
extern "C"{
 int gifDrawFrame(void*gifhandle,size_t pxstride, void *pixels,
              bool force_dispose_1);
 void *gifload(const char*path);
 void gifunload(void*handle);
 void gifgetinfo(void*handle,int*w,int*h,int*frames,int*delays);
 void gifgetnextframe(void*handle,int*idx);
}

NGL_MODULE(ImageFrame);

namespace nglui{
class ImageFrame{
public:
    RefPtr<ImageSurface>image;
    int delay;
};

ImageFrameView::ImageFrameView(int w,int h):ImageView(w,h){
    index=0;
    animationStarted=FALSE;
    gifhandle=nullptr;
}

ImageFrameView::~ImageFrameView(){
    if(gifhandle)
       gifunload(gifhandle);
}

void ImageFrameView::onAttached(){
    sendMessage(WM_TIMER,0,0,100);
}
void ImageFrameView::addImage(const std::string&res){
    std::shared_ptr<ImageFrame>frm=std::make_shared<ImageFrame>();
    frm->image=App::getInstance().loadImage(res);
    frm->delay=0;
    imageframes.push_back(frm);
}

void ImageFrameView::setImage(const std::string&resname){
    if(imageframes.size()==0)
        addImage(resname);
}
void ImageFrameView::clearImages(){
    imageframes.clear();
}

void ImageFrameView::onDraw(GraphContext& canvas) {
    ImageView::onDraw(canvas);
    RECT rect=getClientRect();
    if(imageframes.size()==0)return;
    NGLOG_VERBOSE("draw frame %d delay=%d",index,imageframes[index]->delay);
    if(curframe){
        gifDrawFrame(gifhandle,curframe->get_stride(),curframe->get_data(),false);
        RefPtr<ImageSurface>img=ImageSurface::create(curframe->get_data(),curframe->get_format(),
               curframe->get_width(),curframe->get_height(),curframe->get_stride());
        gifgetnextframe(gifhandle,&index);
        canvas.draw_image(img,&rect,nullptr,(SCALETYPE)scale_type_);
    }else{
        canvas.draw_image(imageframes[index]->image,&rect,nullptr,(SCALETYPE)scale_type_);
        index=(index+1)%imageframes.size();
    }
}

void ImageFrameView::start(){
    animationStarted=TRUE;
    sendMessage(WM_TIMER,0,0,100);
}

void ImageFrameView::stop(){
    animationStarted=FALSE;
}

bool ImageFrameView::onMessage(DWORD msg,DWORD wp,ULONG lp){
    if( (msg==WM_TIMER) && imageframes.size() ){ 
        invalidate(nullptr);
        int delay=imageframes[index]->delay;
        sendMessage(msg,wp,lp,(delay==0?100:delay));
        return TRUE;
    }
    return ImageView::onMessage(msg,wp,lp);
}

int ImageFrameView::loadgif(const std::string&path){
    int w,h,frames;
    int*delays;
    gifhandle=gifload(path.c_str());
    if(nullptr==gifhandle)
       return 0;
    gifgetinfo(gifhandle,&w,&h,&frames,nullptr); 
    delays=(int*)malloc(sizeof(int)*frames);
    gifgetinfo(gifhandle,&w,&h,&frames,delays); 
    curframe=ImageSurface::create(Surface::Format::ARGB32,w,h); 
    for(int i=0;i<frames;i++){
        std::shared_ptr<ImageFrame>frm=std::make_shared<ImageFrame>();
        frm->image=curframe;
        frm->delay=delays[i];
        NGLOG_VERBOSE("frame %d delay=%d",i,frm->delay);
        imageframes.push_back(frm);
    }
    NGLOG_VERBOSE("%d frames loaded imagesize=%dx%d",frames,w,h);
    return frames;
}

}//namespace

