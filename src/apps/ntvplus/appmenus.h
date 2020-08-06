#ifndef __MENU_MENU_H__
#define __MENU_MENU_H__
#include<windows.h>
#include<ntvwindow.h>
#include<satellite.h>

namespace ntvplus{

typedef std::function<void(View&v,int value)>SettingChangeListener;
typedef std::function<Window*(int)>SubMenuCreateListener;
typedef std::function<void(Window*)>SettingDataLoadingListener;

Window*CreateMainMenu();
Window*CreateChannelList();
Window*CreateTVGuide();
Window*CreateChannelPF();
Window*CreateChannelSearch(bool advance=false);
Window*CreateMultiMedia();
Window*CreateIrdCcaCaStatus();
Window*CreateIrdCcaServiceStatus();
Window*CreatePVRWindow();
Window*CreateSearchResultWindow(std::vector<TRANSPONDER>tps,INT searchmode=0);//0 for NIT search others TP search

Window*CreatePlayerCtrlWindow(void*handle,const std::string&);
Window*CreateMediaWindow();
Window*CreateSettingWindow();

Window*CreateTPManagerWindow();
typedef std::function<void(const SATELLITE&,void*)> UpdateSatelliteListener;
typedef std::function<void(const TRANSPONDER&,void*)> UpdateTransponderListener;
Window*CreateSatEditorWindow(const SATELLITE*sat=nullptr,const UpdateSatelliteListener ls=nullptr,void*userdata=nullptr);
Window*CreateTPEditorWindow (const TRANSPONDER*tp=nullptr,const UpdateTransponderListener ls=nullptr,void*userdata=nullptr);

Window*CreateChannelEditWindow();
Window*CreateManualChannelSearch();

}//namespace

#endif

