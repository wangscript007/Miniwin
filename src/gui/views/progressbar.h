#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__
#include <widget.h>

namespace nglui{

class ProgressBar:public Widget{
protected:
    int min_;
    int max_;
    int progress_;
    int  indeterminatePos;
    bool indeterminate_;
    virtual bool setProgressInternal(int progress, bool fromUser=false);
    virtual void onProgressRefresh(float scale, bool fromUser, int progress){};
public:
    ProgressBar(int width, int height);
    void setMin(int value);
    void setMax(int value);
    int getMin(){return min_;}
    int getMax(){return max_;}
    void setRange(int vmin,int vmax);
    void setProgress(int value);
    int getProgress();
    void setIndeterminate(bool indeterminate);
    virtual void onDraw(GraphContext&canvas)override;
    bool onMessage(DWORD msg,DWORD wp,DWORD lp)override;
};

}
#endif
