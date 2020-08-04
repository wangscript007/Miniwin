#ifndef __IMAGE_BUTTON_H__
#define __IMAGE_BUTTON_H__
#include <button.h>
#include <imageview.h>
namespace nglui{

class ImageButton:public ImageView{
protected:
  RefPtr<ImageSurface>imghot;
public :
  ImageButton(int w,int h);
  ImageButton(const std::string&txt,int w,int h);
  virtual void setHotImage(const std::string&respath);
  virtual void onDraw(GraphContext&canvas)override;
private:
  typedef ImageView INHERITED;
  DISALLOW_COPY_AND_ASSIGN(ImageButton);
};

}//namespace
#endif

