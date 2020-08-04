#include <absseekbar.h>
#include <math.h>
#include <ngl_types.h>
#include <ngl_log.h>

#define MAX(a,b) ((a)>(b)?(a):(b))

NGL_MODULE(AbsSeekBar);

namespace nglui{

AbsSeekBar::AbsSeekBar(int w,int h):ProgressBar(w,h){
    mKeyProgressIncrement=5;
    setFlag(FOCUSABLE);
}

void AbsSeekBar::setMin(int min) {
    ProgressBar::setMin(min);
    int range = getMax() - getMin();

    if ((mKeyProgressIncrement == 0) || (range / mKeyProgressIncrement > 20)) {
        // It will take the user too long to change this via keys, change it
        // to something more reasonable
        setKeyProgressIncrement(MAX(1,round((float) range / 20)));
    }
}
void AbsSeekBar::setMax(int max) {
    ProgressBar::setMax(max);
    int range = getMax() - getMin();

    if ((mKeyProgressIncrement == 0) || (range / mKeyProgressIncrement > 20)) {
        // It will take the user too long to change this via keys, change it
        // to something more reasonable
        setKeyProgressIncrement(MAX(1,round((float) range / 20)));
    }
}

void AbsSeekBar::onProgressRefresh(float scale, bool fromUser, int progress){
}

void AbsSeekBar::setKeyProgressIncrement(int increment){
    mKeyProgressIncrement=increment;
}

void AbsSeekBar::setThumbOffset(int thumbOffset){
    mThumbOffset = thumbOffset;
    invalidate(nullptr);
}

void AbsSeekBar::drawThumb(GraphContext&canvas) {
    if (mThumb != nullptr) {
        canvas.save();
        // Translate the padding. For the x, we need to allow the thumb to
        // draw in its extra space
        canvas.translate( - mThumbOffset, 0);
        //mThumb.draw(canvas);
        canvas.restore();
    }
}

void  AbsSeekBar::drawTickMarks(GraphContext&canvas){
    int count = getMax() - getMin();
    if (count > 1) {
        int w = getWidth();//mTickMark.getIntrinsicWidth();
        int h = getHeight();//mTickMark.getIntrinsicHeight();
        int halfW = w >= 0 ? w / 2 : 1;
        int halfH = h >= 0 ? h / 2 : 1;
        //mTickMark.setBounds(-halfW, -halfH, halfW, halfH);

        float spacing = getWidth()/ (float) count;
        canvas.save();
        canvas.translate(0, getHeight() / 2);
        for (int i = 0; i <= count; i++) {
            //mTickMark.draw(canvas);
            canvas.translate(spacing, 0);
        }
        canvas.restore();
    }
}

void AbsSeekBar::onDraw(GraphContext&canvas){
    ProgressBar::onDraw(canvas);    
}

bool AbsSeekBar::onKeyUp(KeyEvent&event){
    int increment = mKeyProgressIncrement;
    switch(event.getKeyCode()){
    case KEY_LEFT :
    case KEY_MINUS:increment*=-1;
         setProgressInternal(getProgress()+increment,true);
         return true;
    case KEY_RIGHT :
    case KEY_KPPLUS:
         setProgressInternal(getProgress()+increment,true);
         return true;
    default:return ProgressBar::onKeyUp(event);
    }
}
}//namespace
