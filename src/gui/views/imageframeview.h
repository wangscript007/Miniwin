#ifndef __IMAGE_FRAME_VIEW_H__
#define __IMAGE_FRAME_VIEW_H__
#include<imageview.h>

namespace nglui{
class ImageFrameView:public ImageView{
protected:
    int index;//current showing index
    void*gifhandle;
    BOOL animationStarted;
    RefPtr<ImageSurface>curframe;//current showing image frame
    std::vector<std::shared_ptr<class ImageFrame>>imageframes;
    void onAttached();
public:
    ImageFrameView(int w, int h);
    ~ImageFrameView();
    void setImage(const std::string&resname)override;
    void addImage(const std::string&res);
    int loadgif(const std::string&path);
    void clearImages();
    void onDraw(GraphContext&)override;
    bool onMessage(DWORD msg,DWORD wp,ULONG lp)override;
    void start();
    void stop();
};
}

#endif

