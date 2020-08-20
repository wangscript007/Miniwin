#ifndef __NTV_COMMON_H__
#define __NTV_COMMON_H__
#include<ngl_types.h>
#include<dvbepg.h>
#include<windows.h>

#define WM_CCAMSG (View::WM_USER+1)
namespace ntvplus{
class NTVTitleBar:public View{
protected:
   unsigned long time_now;
   std::string title;
   RefPtr<ImageSurface>logo;
public:
   NTVTitleBar(int w,int h);
   void setTime(time_t n);
   void setTitle(const std::string&txt);
   virtual void onDraw(GraphContext&canvas)override;
};

class NTVEditText:public EditText{
public:
    NTVEditText(int w,int h);
    virtual void onDraw(GraphContext&canvas);
};


class NTVSelector :public Selector{
public:
   NTVSelector(const std::string&txt,int w,int h);
   virtual void onDraw(GraphContext&canvas)override;
};

class NTVProgressBar:public ProgressBar{
public:
   NTVProgressBar(int w,int h);
   virtual void onDraw(GraphContext&canvas)override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHANNEL_LIST_ITEM_HEIGHT 42
class ChannelItem:public ListView::ListItem{
public:
    SERVICELOCATOR svc;
    BOOL camode;
    BOOL isHD;
public :
    ChannelItem(const std::string&txt,const SERVICELOCATOR*loc,bool camd=false):ListView::ListItem(txt){
        svc=*loc;camode=camd;
        setId((svc.tsid<<16)|svc.sid);
    }

    virtual bool operator==(const ListView::ListItem&b)const{
        return svc==((const ChannelItem&)b).svc;
    }
};

class SatelliteItem:public AbsListView::ListItem{
public :
   int id;
   SATELLITE satellite;
   SatelliteItem(const SATELLITE&sat,int id_)
         :AbsListView::ListItem(sat.name,id){
        satellite=sat;       id=id_;
   }
};

class TransponderItem:public AbsListView::ListItem{
public:
   TRANSPONDER tp;
   TransponderItem(const TRANSPONDER&t);
};

ToolBar*CreateNTVToolBar(int w,int h);
void DrawSplitter(GraphContext&canvas,RECT&r);
void ChannelPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas);
void ChannelPainterLCN(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas);

void SettingPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas);
void GetTPString(const TRANSPONDER*tp,std::string&s);
void ShowVolumeWindow(int timeout);
void ShowAudioSelector(int estype,int timeout);
}//namespace

#endif
