#include<ngl_types.h>
#include<ngl_log.h>
#include<ngl_disp.h>
#include <gxcore.h>
#include <av/gxav_firmware.h>
#include <bus/module/player/gxplayer_module.h>
#include <bus/service/gxplayer.h>
#include <bus/gui_core/gdi_core.h>
#include <bus/module/player/gxplayer_config.h>
#include <bus/module/player/gxplayer_common.h>
#include <av/gxav_common.h>
NGL_MODULE(DISP);

static void EventCBK(PlayerEventID event,void* args){
     NGLOG_DEBUG("event_report event %x,%x  args=%p",event,event,args);
}

DWORD nglDispInit(){
    int rc;
    static inited=0;
    DisplayScreen Screen;
    VideoOutputConfig config;
    VideoColor color;
    GxMsgProperty_PlayerVideoAutoAdapt adapt={0};
    if(inited++>0)return NGL_OK;
 #if 0
    GxPlayer_ModuleRegisterDVB();
    GxPlayer_ModuleRegisterFILE();
    GxPlayer_ModuleRegisterMP3();
    GxPlayer_ModuleRegisterMP4();
    GxPlayer_ModuleRegisterAVI();
    GxPlayer_ModuleRegisterES();
    GxPlayer_ModuleRegisterMKV();
    GxPlayer_ModuleRegisterFLV();
    GxPlayer_ModuleRegisterAAC();
    GxPlayer_ModuleRegisterTS ();
    GxPlayer_ModuleRegisterPS ();
    GxPlayer_ModuleRegisterLOGO();
    GxPlayer_ModuleRegisterRMVB();

    GxPlayer_ModuleRegisterVOD();
    GxPlayer_ModuleRegisterHTTP();
    GxPlayer_ModuleRegisterM3U8();
    GxPlayer_ModuleRegisterRTMP();
    GxPlayer_ModuleRegisterHTTPS();
    GxPlayer_ModuleRegisterUDP();
    GxPlayer_ModuleRegisterRTP();
    GxPlayer_ModuleRegisterRTSP();
    GxPlayer_ModuleRegisterWFD();
    GxPlayer_ModuleRegisterCRYPTO();
    GxPlayer_ModuleRegisterFIFO();//ONLY FOR LINUX
    GxPlayer_ModuleRegisterRINGMEM();
#else
    GxPlayer_ModuleRegisterALL();    
#endif 
    GxPlayer_ModuleInit();

    GxPlayer_SetEventCallback(EventCBK);

    adapt.enable = 1;
    adapt.pal = GX_VIDEO_OUTPUT_PAL;
    adapt.ntsc = GX_VIDEO_OUTPUT_NTSC_M;
    rc=GxPlayer_SetVideoAutoAdapt(&adapt);
    NGLOG_DEBUG("GxPlayer_SetVideoAutoAdapt=%d",rc);

    Screen.xres = 1280;
    Screen.yres = 720;
    Screen.aspect = 0; /*0.4:3/1.16:9*/
    rc=GxPlayer_SetDisplayScreen(Screen);
    NGLOG_DEBUG("GxPlayer_SetDisplayScreen=%d",rc);


#ifndef GX3113C
    rc=GxPlayer_SetVideoOutputSelect(GX_VIDEO_OUTPUT_HDMI|GX_VIDEO_OUTPUT_RCA);
    NGLOG_DEBUG("GxPlayer_SetVideoOutputSelect=%d",rc);
    config.interface = GX_VIDEO_OUTPUT_HDMI;
    config.mode = GX_VIDEO_OUTPUT_720P_50HZ;//GX_VIDEO_OUTPUT_HDMI_720P_50HZ;
    rc=GxPlayer_SetVideoOutputConfig(config);
    NGLOG_DEBUG("GxPlayer_SetVideoOutputConfig(HDMI)=%d",rc);
#else
    rc=GxPlayer_SetVideoOutputSelect(GX_VIDEO_OUTPUT_RCA);
    NGLOG_DEBUG("GxPlayer_SetVideoOutputSelect=%d",rc);
#endif
    config.interface = GX_VIDEO_OUTPUT_RCA;//CVBS
    config.mode = GX_VIDEO_OUTPUT_PAL;
    rc=GxPlayer_SetVideoOutputConfig(config);
    NGLOG_DEBUG("GxPlayer_SetVideoOutputConfig(CVBS)=%d",rc);

    color.interface = GX_VIDEO_OUTPUT_HDMI;
    color.brightness = 50;
    color.saturation = 50;
    color.contrast = 50;
    rc=GxPlayer_SetVideoColors(color);

    color.interface = GX_VIDEO_OUTPUT_RCA;
    rc=GxPlayer_SetVideoColors(color);
    NGLOG_DEBUG("GxPlayer_SetVideoColors=%d",rc);
    //4 freezon
    //GxPlayer_SetFreezeFrameSwitch(1);
    GxPlayer_SetFreezeFrameSwitch(0);

    GxPlayer_SetAudioVolume(50);
    return NGL_OK;
}

DWORD nglDispSetResolution(int res){
    VideoOutputConfig cfg;
    cfg.interface=GX_VIDEO_OUTPUT_HDMI;
    switch(res){
    case DISP_RES_1080I:cfg.mode=GX_VIDEO_OUTPUT_1080I_60HZ;break;
    case DISP_RES_1080P:cfg.mode=GX_VIDEO_OUTPUT_1080P_60HZ;break;
    case DISP_RES_720I :
    case DISP_RES_720P :cfg.mode=GX_VIDEO_OUTPUT_720P_60HZ;break;
    case DISP_RES_576I :cfg.mode=GX_VIDEO_OUTPUT_576I;break;
    case DISP_RES_576P :cfg.mode=GX_VIDEO_OUTPUT_576P;break;
    case DISP_RES_480I :cfg.mode=GX_VIDEO_OUTPUT_480P;break;
    default:break;
    }
    GxPlayer_SetVideoOutputConfig(cfg);
    NGLOG_DEBUG("res=%d",res);
    return NGL_OK;
}

DWORD nglDispSetAspectRatio(int ratio){
    int rc;
    VideoAspect asp;
    switch(ratio){
    case DISP_APR_AUTO:   break;
    case DISP_APR_4_3:    asp=ASPECT_4X3;break;
    case DISP_APR_16_9:   asp=ASPECT_16X9;break;
    default:NGLOG_DEBUG("Invalid value %d",ratio);return NGL_ERROR;
    }    
    GxPlayer_SetVideoAspect(asp);
    NGLOG_DEBUG("ratio=%d rc=%d",ratio,rc);
    return rc;
}

DWORD nglDispGetAspectRatio(int*ratio){
    return NGL_OK;
}

DWORD nglDispSetMatchMode(int md){
    int rc;
    switch(md){
    case DISP_MM_PANSCAN : break;
    case DISP_MM_LETTERBOX:break;
    case DISP_MM_PILLBOX : break;
    case DISP_MM_NORMAL_SCALE:break;
    case DISP_COMBINED_SCALE:break;
    return NGL_ERROR;
    }
    NGLOG_DEBUG("md=%d rc=%d",md,rc);
    return rc;
}

DWORD nglDispSetBrightNess(int value){
    int rc;
    VideoColor cl;
    cl.interface=GX_VIDEO_OUTPUT_HDMI;
    GxPlayer_GetVideoColors(&cl);
    cl.brightness=value;
    GxPlayer_SetVideoColors(cl);
    NGLOG_DEBUG("rc=%d value=%d",rc,value);
}

DWORD nglDispSetContrast(int value){
    int rc;
    VideoColor cl;
    cl.interface=GX_VIDEO_OUTPUT_HDMI;
    GxPlayer_GetVideoColors(&cl);
    cl.contrast=value;
    GxPlayer_SetVideoColors(cl);
    NGLOG_DEBUG("rc=%d value=%d",rc,value);
}

DWORD nglDispSetSaturation(int value){
    int rc=0;
    VideoColor cl;
    cl.interface=GX_VIDEO_OUTPUT_HDMI;
    GxPlayer_GetVideoColors(&cl);
    cl.saturation=value;
    GxPlayer_SetVideoColors(cl);
    NGLOG_DEBUG("rc=%d value=%d ",value,rc);
}
 
