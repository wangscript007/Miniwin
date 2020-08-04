#include <seekbar.h>
namespace nglui{

SeekBar::SeekBar(int w,int h):AbsSeekBar(w,h){
}

void SeekBar::onProgressRefresh(float scale, bool fromUser, int progress){
    AbsSeekBar::onProgressRefresh(scale, fromUser, progress);
    if (mOnSeekBarChangeListener != nullptr) {
         mOnSeekBarChangeListener(*this, progress, fromUser);
    }
}

}
