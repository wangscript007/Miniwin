#ifndef __SEEK_BAR_H__
#define __SEEK_BAR_H__
#include <absseekbar.h>

namespace nglui{
class SeekBar:public AbsSeekBar{
public:
   DECLARE_UIEVENT(void,OnSeekBarChangeListener,AbsSeekBar&seek,int progress,bool fromuser);
protected:
   OnSeekBarChangeListener  mOnSeekBarChangeListener;
   void onProgressRefresh(float scale, bool fromUser, int progress)override;
public:
   SeekBar(int w,int h);
   void setOnSeekBarChangeListener(OnSeekBarChangeListener l) {
        mOnSeekBarChangeListener = l;
   }
   
};

}//namespace
#endif 
