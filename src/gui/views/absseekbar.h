#ifndef __ABS_SEEK_BAR_H__
#define __ABS_SEEK_BAR_H__
#include <progressbar.h>

namespace nglui{

class AbsSeekBar:public ProgressBar{
protected:
   int mKeyProgressIncrement;
   int mThumbOffset;
   RefPtr<ImageSurface>mThumb;
   virtual void drawThumb(GraphContext&canvas);
   virtual void drawTickMarks(GraphContext&canvas);
   virtual void onProgressRefresh(float scale, bool fromUser, int progress);
public:
   AbsSeekBar(int w,int h);
   void setKeyProgressIncrement(int increment);
   int getKeyProgressIncrement() {
       return mKeyProgressIncrement;
   }
   void setThumbOffset(int thumbOffset);
   int getThumbOffset()const {
        return mThumbOffset;
   }
   void setMin(int min);
   void setMax(int max);
   void onDraw(GraphContext&canvas);
   bool onKeyUp(KeyEvent&event);
};

}//namespace

#endif
