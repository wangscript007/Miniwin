#include <windows.h>
#include <ngl_video.h>
#include <appmenus.h>
#include <dvbapp.h>
#include <dvbepg.h>
#include <satellite.h>
#include <diseqc.h>
#include <string>
#include <ngl_log.h>
#include <iostream>
#include <teletextview.h>
#include <ntvnavwindow.h>
NGL_MODULE(MAIN);

using namespace ntvplus;

static bool onKey(int key){
     switch(key){
     case KEY_MENU:CreateMainMenu();return true;
     case KEY_ENTER:CreateChannelList();return true;
     case KEY_UP:
     case KEY_DOWN:CreateChannelPF();return true;
     case KEY_EPG: CreateTVGuide();return true;
     case KEY_F5:
             return true;
     case KEY_AUDIO:
         ShowAudioSelector(ST_AUDIO,4000);return true;
     case KEY_POWER:App::getInstance().exit(0);return true;
     case KEY_VOLUMEUP:
     case KEY_VOLUMEDOWN:
     case KEY_LEFT:
     case KEY_RIGHT:
          ShowVolumeWindow(2000);
          break;
     case KEY_SUBTITLE:
          break;
     case KEY_RED  :CreateManualChannelSearch();return true;
     case KEY_GREEN:CreateBookingEditor();      return true;
     case KEY_YELLOW:new NTVWindow(0,0,1280,720);return true;
     default:return false;
     }
}

int main(int argc,const char*argv[]){
    DVBApp app(argc,argv);
    Desktop*desktop=new Desktop();
    desktop->addChildView(new TeletextView(640,480))->setId(100).setPos(300,100)
          .setFlag(View::BORDER).setBgColor(0xFFFF0000);
    app.setName("com.ntvplus.dvbs");        
    app.setOpacity(app.getArgAsInt("alpha",255));
    app.getString("mainmenu",app.getArg("language","eng"));
    desktop->setKeyListener(onKey);
    return app.exec();
}

