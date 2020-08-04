#ifndef __CHANNEL_EPG_VIEW_H__
#define __CHANNEL_EPG_VIEW_H__
#include <listview.h>
namespace nglui{

struct TVEvent{
   UINT id;
   LONG start;
   LONG duration;
   std::string name;
   std::string desc;
   std::string ext;
};

class ChannelBar:public ListView::ListItem{
public:
    std::vector<TVEvent>events;
    int index;
public:
    ChannelBar(const std::string& txt);
    void addEvent(const TVEvent&evt);
    void addEvent(const std::string name,ULONG starttm,ULONG dur);
    void clearEvents();
    void onDraw(AbsListView&lv,GraphContext&canvas,const RECT&rect,bool isselected);
    int selectEvent(LONG etm);
};

class ChannelEpgView:public ListView{
public:
    enum{
      CHANNEL_FG,
      CHANNEL_FOCUSED,
      BORDER,
      EVENT_POST,
      EVENT_NOW,
      EVENT_NEXT,
      EVENT_FOCUSED,
      EVENT_FEATURE,
      COLOR_LAST
    };
    DECLARE_UIEVENT(void,EventSelectListener,ChannelEpgView&,const TVEvent&);
    DECLARE_UIEVENT(void,TimeChangeListener,ChannelEpgView&);
protected:
    LONG starttime;
    float pixelMinute;
    int nameWidth;
    EventSelectListener onEventSelect;
    TimeChangeListener onTimeChange;
    int colors[COLOR_LAST];
    LONG getEndTime();
public:
    ChannelEpgView(int w,int h);
    void setColor(int idx,UINT color);
    UINT getColor(int idx);
    void setStartTime(LONG t);
    void setPixelPerMinute(float p);
    void setChannelNameWidth(int w);
    int getChannelNameWidth();
    void setIndex(int idx)override;
    LONG getStartTime();
    void setEventChangeListener(EventSelectListener ls){onEventSelect=ls;}
    void setTimeChangeListener(TimeChangeListener ls){onTimeChange=ls;}
    float getPixelPerMinute();
    virtual void onDraw(GraphContext&canvas)override;
    virtual bool onKeyUp(KeyEvent&key)override;
    static void DefaultPainter(AbsListView&lv,const AbsListView::ListItem&itm,int state,GraphContext&canvas); 
public:
   typedef ListView INHERITED;
};

}//namespace
#endif

