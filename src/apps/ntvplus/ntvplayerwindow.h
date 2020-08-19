#ifndef __PLAYER_WINDOW_H__
#define __PLAYER_WINDOW_H__
#include <ntvwindow.h>

namespace ntvplus{
class NTVPlayerWindow:public NTVWindow{
public:
   NTVPlayerWindow(int x,int y,int w,int h);
   ~NTVPlayerWindow();
   virtual void onFavoriteGroup(UINT favid,const std::string&favname);
   virtual void onService(const DVBService*svc);
   virtual void onEventPF(const SERVICELOCATOR&svc,const DVBEvent*e,bool precent);
   virtual void onEvents(const SERVICELOCATOR&svc,const DVBEvent*e);
   int loadGroups();
   int loadServices(UINT favid);
   int loadEventPF(const SERVICELOCATOR*svc);
   int loadEvents(const SERVICELOCATOR&svc);
};
}
#endif
