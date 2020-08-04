#ifndef __UI_IMAGE_VIEW_H__
#define __UI_IMAGE_VIEW_H__

#include <widget.h>

namespace nglui{

class ImageView : public Widget {
protected:
   int scale_type_;
   RefPtr<ImageSurface> img_;
public:
   static constexpr const char* VIEW_NAME = "ImageBox";

public:
   explicit ImageView(const std::string& path);
   explicit ImageView(int w, int h);
   explicit ImageView(const std::string& path, int w, int h);
   virtual ~ImageView();

   virtual void onDraw(GraphContext& canvas) override;
   virtual const SIZE& getPreferSize() override;
   virtual void setImage(const std::string&resname);
   int getScaleType();
   void setScaleType(int st);
private:
   typedef Widget INHERITED;
   DISALLOW_COPY_AND_ASSIGN(ImageView);
};

}
#endif
