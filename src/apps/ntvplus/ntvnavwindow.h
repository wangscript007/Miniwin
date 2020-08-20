#ifndef __NAVIGATE_WINDOW_H__
#define __NAVIGATE_WINDOW_H__
#include <ntvwindow.h>

namespace ntvplus{
class TVChannel:public ListView::ListItem{
public:
    SERVICELOCATOR svc;
    BOOL camode;
    BOOL isHD;
public :
    TVChannel(const std::string&txt,const SERVICELOCATOR*loc,bool camd=false):ListView::ListItem(txt){
        svc=*loc;
        camode=camd;
        setId(svc.tsid|svc.sid);
    }
    virtual bool operator==(const ListView::ListItem&b)const{
        return svc==((const TVChannel&)b).svc;
    }
};

class NTVNavWindow:public NTVWindow{
public:
   NTVNavWindow(int x,int y,int w,int h);
   ~NTVNavWindow();
   static void ChannelPainter(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas);
   static void ChannelPainterLCN(AbsListView&lv,const ListView::ListItem&itm,int state,GraphContext&canvas);
   static TVChannel*DVBService2Channel(const DVBService*);

   /*the flowing 4 method onXXXX is called by loadXXXX,user should override this method to fill UI component,*/
   virtual void onGroup(UINT favid,const std::string&favname);
   virtual void onService(const DVBService*svc);/*called by loadService ,override to fill UI component*/
   virtual void onEventPF(const SERVICELOCATOR&svc,const DVBEvent*e,bool present);/*called by loadEventPF to fill UI component*/
   virtual void onEvents(const SERVICELOCATOR&svc,const std::vector<DVBEvent>&evts);/*called by loadEvents to fill schedule epg to UI component*/
   virtual int loadGroups();
   virtual int loadServices(UINT favid);
   virtual int loadEventPF(const SERVICELOCATOR*svc=nullptr/*null for currentsurface*/);
   virtual int loadEvents(const SERVICELOCATOR&svc);
   virtual bool onMessage(DWORD msg,DWORD wp,ULONG lp)override;
};
}
#endif
