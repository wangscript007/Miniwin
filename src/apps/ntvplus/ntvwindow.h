#ifndef __NTVWINDOW_H__
#define __NTVWINDOW_H__
#include <windows.h>
#include <dvbepg.h>
#include <satellite.h>
#include <ntvcommon.h>

using namespace nglui;

namespace ntvplus{
enum {
    NWS_TITLE=1,
    NWS_SIGNAL=2,
    NWS_TOOLTIPS=4
};

class NTVWindow:public Window{
protected:
   DWORD handle_notify;
   std::string title;
   ProgressBar*sig_strength;
   ProgressBar*sig_quality;
   NTVTitleBar*titlebar;
   ToolBar*toolbar;
public:
   NTVWindow(int x,int y,int w,int h);
   ~NTVWindow();
   virtual void initContent(int styles=NWS_TITLE|NWS_TOOLTIPS);//create children view. 
   virtual void setText(const std::string&txt);
   const std::string&getText()const;
   void addTipInfo(const std::string&img,const std::string&title,int width=-1);
   void clearTips();
   virtual void onEITPF(const SERVICELOCATOR*);
   virtual void onEITS(const SERVICELOCATOR*);
   virtual bool onMessage(DWORD msgid,DWORD wParam,ULONG lParam)override;
};

}//namespace

#endif
