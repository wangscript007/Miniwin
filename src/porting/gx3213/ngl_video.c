#include <ngl_types.h>
#include <ngl_video.h>
#include <ngl_log.h>
#include <ngl_disp.h>

#include <gxcore.h>
#include <av/gxav_firmware.h>
#include <bus/module/player/gxplayer_module.h>
#include <bus/service/gxplayer.h>
#include <bus/gui_core/gdi_core.h>
#include <bus/module/player/gxplayer_config.h>
#include <bus/module/player/gxplayer_common.h>
#include <bus/module/player/gxplayer_url.h>
#include <bus/module/frontend/gxfrontend_module.h>
#include <frontend/demod.h>
#include <frontend/tuner.h>
#include <av/gxav_common.h>

NGL_MODULE(AV);

typedef struct{
}NGLAV;
#define NGLPLAYER "ngl_player"
#define NB_DEMUX 4 

static NGLAV sAvPlayers[NB_DEMUX];


INT nglAvInit(){
    nglDispInit();
}

static int VTYPE2GX(int nt){
  switch(nt){
  case DECV_MPEG:return GX_URL_VAL_VCODEC_MPG2;//MM_STREAMTYPE_MP2V;
  case DECV_AVC: return GX_URL_VAL_VCODEC_H264;
  case DECV_AVS: return GX_URL_VAL_VCODEC_AVS;
  //case DECV_XVID:return AUI_DECV_FORMAT_XVID;
  //case DECV_FLV1:return AUI_DECV_FORMAT_FLV1;
  //case DECV_VP8: return AUI_DECV_FORMAT_VP8;
  //case DECV_WVC1:return AUI_DECV_FORMAT_WVC1;
  //case DECV_WX3: return AUI_DECV_FORMAT_WX3;
  //case DECV_RMVB:return AUI_DECV_FORMAT_RMVB;
  //case DECV_MJPG:return AUI_DECV_FORMAT_MJPG;
  case DECV_HEVC:return GX_URL_VAL_VCODEC_H265;
  case DECV_INVALID:return -1;
  }
}
static int ATYPE2GX(int nt){
  switch(nt){
  case DECA_MPEG1:return GX_URL_VAL_ACODEC_MPG1;
  case DECA_MPEG2:return GX_URL_VAL_ACODEC_MPG2;
  case DECA_AAC_LATM:return GX_URL_VAL_ACODEC_LATM;
  case DECA_AC3:return GX_URL_VAL_ACODEC_AC3;
  case DECA_DTS:return GX_URL_VAL_ACODEC_DTS;
  case DECA_PPCM://return AUI_DECA_STREAM_TYPE_PPCM;
  case DECA_LPCM_V://return AUI_DECA_STREAM_TYPE_LPCM_V;
  case DECA_LPCM_A:return GX_URL_VAL_ACODEC_LPCM;//AUI_DECA_STREAM_TYPE_LPCM_A;
  //case DECA_PCM:return AUI_DECA_STREAM_TYPE_PCM;
  //case DECA_BYE1:return AUI_DECA_STREAM_TYPE_BYE1;
  //case DECA_RA8:return  AUI_DECA_STREAM_TYPE_RA8;
  //case DECA_MP3:return AUI_DECA_STREAM_TYPE_MP3;
  //case DECA_AAC_ADTS:return AUI_DECA_STREAM_TYPE_AAC_ADTS;
  //case DECA_OGG:return AUI_DECA_STREAM_TYPE_OGG;
  case DECA_EC3:return GX_URL_VAL_ACODEC_EAC3;//AUI_DECA_STREAM_TYPE_EC3;
  //case DECA_MP3_L3:return AUI_DECA_STREAM_TYPE_MP3_L3;
  //case DECA_RAW_PCM:return AUI_DECA_STREAM_TYPE_RAW_PCM;
  //case DECA_BYE1PRO:return AUI_DECA_STREAM_TYPE_BYE1PRO;
  //case DECA_FLAC: return AUI_DECA_STREAM_TYPE_FLAC;
  //case DECA_APE: return AUI_DECA_STREAM_TYPE_APE;
  //case DECA_MP3_2:return AUI_DECA_STREAM_TYPE_MP3_2;
  //case DECA_AMR: return AUI_DECA_STREAM_TYPE_AMR;
  //case DECA_ADPCM:AUI_DECA_STREAM_TYPE_ADPCM;
  case DECA_INVALID:return -1;
  }
}

INT nglAvPlay(int dmxid,int vpid,int vtype,int apid,int atype,int pcr)
{
    char url[512];
    int rc=0;
    int pmtpid=0,vcodec,acodec;
    GxFrontend fe;
    GxFrontend_GetCurFre(0,&fe);
    vcodec=VTYPE2GX(vtype);
    acodec=ATYPE2GX(atype);
    sprintf(url,"dvbc://fre:%d&symbol:%d&qam:%d&vpid:%d&apid:%d&vcodec:%d&acodec:%d&pcrpid:%d&tuner:%d&tsid:%d&dmxid:%d&pls_num:%d",
           fe.fre,fe.symb,fe.qam,vpid,apid,vcodec,acodec,pcr,0,0,0,0);
    NGLOG_DEBUG("aui_av_start=%d video=%d/%d audio=%d/%d pcr=%d\r\nURL:%s",rc,vpid,vtype,apid,atype,pcr,url);
    nglAvStop(dmxid);
    rc=GxPlayer_MediaPlay(NGLPLAYER,url,0,50,0);
    NGLOG_DEBUG("FREQ:%d,%d,%d  rc=%d URL:%s",fe.fre,fe.symb,fe.qam,rc,url);
    return rc;
}

INT nglAvStop(int dmx){
    GxPlayer_MediaStop(NGLPLAYER);
    return NGL_OK;
}

INT nglAvSetVideoWindow(int dmxid,const NGLRect*inRect,const NGLRect*outRect){
     int rc;
     NGLAV*av=&sAvPlayers[dmxid];
     PlayerWindow inwin={0,0,1280,720};
     PlayerWindow outwin={0,0,1280,720};
     if(inRect){
         inwin.x=inRect->x;
         inwin.y=inRect->y;
         inwin.width=inRect->w;
         inwin.height=inRect->h;
     }
     if(outRect){
         outwin.x=outRect->x; 
         outwin.y=outRect->y; 
         outwin.width=outRect->w; 
         outwin.height=outRect->h; 
     }
     NGLOG_DEBUG("before mediaclip");
     GxPlayer_MediaClip(NGLPLAYER,&inwin);
     rc=GxPlayer_MediaWindow(NGLPLAYER,&outwin);
     NGLOG_DEBUG("after setmediawindow");
     return rc;
}
