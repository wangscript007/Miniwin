#include <dvbapp.h>
#include <ngl_tuner.h>
#include <ngl_dmx.h>
#include <ngl_video.h>
#include <dvbepg.h>
#include <ngl_log.h>
#include <preferences.h>
#include <ngl_disp.h>
#include <mutex>
#include <thread>
#include <renderimpl.h>
#ifdef HAVE_ACS
extern void StartACS();
#endif

NGL_MODULE(DVBAPP)

namespace nglui{

DVBApp::DVBApp(int argc,const char**argv)
  :App(argc,argv){
    Preferences pref;

    nglGraphInit();
    DtvEpgInit();
    LoadSatelliteFromDB("satellites.json");
    DtvLoadProgramsData("dvb_programs.dat");
    FavInit(getArg("favorite","favorites.json").c_str());
    DtvInitLCN((LCNMODE)(LCN_FROM_BAT|LCN_FROM_USER),1000);
    LoadServiceAdditionals("additionals.json");
    pref.load("settings.pref");
    int res=pref.getInt("pciture","resolution",DISP_RES_720P);
    NGLOG_DEBUG("DVBApp::DVBApp resolution=%d",res);
    setOpacity(200);//getArgAsInt("alpha",255));    
    nglDispSetResolution(res);
    MediaRenderer::getInstance().setName("NGL MediaRenderer");
    MediaRenderer::getInstance().start();
#ifdef HAVE_ACS
    StartACS();
#endif    
}

int DVBApp::exec(){
    std::thread th([&](){looper.run();});
    th.join();
}

}//namespace

