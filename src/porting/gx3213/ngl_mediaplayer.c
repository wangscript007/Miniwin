#include <ngl_types.h>
#include <ngl_mediaplayer.h>
#include <ngl_log.h>
#include <ngl_os.h>
#include <bus/module/player/gxplayer_module.h>
#include <bus/service/gxplayer.h>
#include <bus/gui_core/gdi_core.h>
#include <bus/module/player/gxplayer_config.h>
#include <bus/module/player/gxplayer_common.h>
#include <bus/module/player/gxplayer_url.h>

NGL_MODULE(NGLMP);
enum {
  INITED=0,
  OPENED=1, 
  PLAYING=2,
  PAUSED=3,
  STOPPED=4
};
typedef struct{
   char*name;
   char*url;
   int state;
   MP_CALLBACK cbk;
   void*userdata; 
}NGL_PLAYER;
static int player_id;
HANDLE nglMPOpen(const char*fname){
    int rc=-1;
    char uniname[128];
    NGL_PLAYER *mp=NULL;
    PlayerWindow w;
    sprintf(uniname,"NGLPLAYER_%d",player_id);
    //if((rc=GxPlayer_Open(fname,NULL))>=0)
    mp=(NGL_PLAYER*)malloc(sizeof(NGL_PLAYER));
    memset(mp,0,sizeof(NGL_PLAYER));
    mp->name=strdup(uniname);
    mp->state=OPENED;
    mp->url=strdup(fname);
    player_id++;
    NGLOG_DEBUG("GxPlayer_Open %s",fname);
    return mp;
}

DWORD nglMPPlay(HANDLE handle){
    int rc;
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    rc=GxPlayer_MediaPlay(mp->name,mp->url,0,50,NULL);
    mp->state=PLAYING;
    NGLOG_DEBUG("%s rc=%d",mp->name,rc);
    return NGL_OK;
}

DWORD nglMPStop(HANDLE handle){
    int rc=-1;
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    rc=GxPlayer_MediaStop(mp->name);
    mp->state=STOPPED;
    NGLOG_DEBUG("%s rc=%d",mp->name,rc);
    return NGL_OK;
}

DWORD nglMPResume(HANDLE handle){
    int rc=-1;
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    rc=GxPlayer_MediaResume(mp->name);
    mp->state=PLAYING;
    NGLOG_DEBUG("%s rc=%d",mp->name,rc);
    return NGL_OK;
}

DWORD nglMPPause(HANDLE handle){
    int rc=-1;
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    rc=GxPlayer_MediaPause(mp->name);
    mp->state=PAUSED;
    NGLOG_DEBUG("%s rc=%d",mp->name,rc);
    return NGL_OK;
}

DWORD nglMPClose(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    if(mp->state!=INITED&&mp->state!=STOPPED)
       GxPlayer_MediaStop(mp->name);
    free(mp->name);
    free(mp->url);
    free(mp);
    return NGL_OK;
}

DWORD nglMPGetTime(HANDLE handle,UINT*curtime,UINT*timems){
    int rc;
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    PlayTimeInfo ti;
    rc=GxPlayer_MediaGetTime(mp->name,&ti);
    NGLOG_DEBUG("%s rc=%d",mp->name,rc);
    *timems=ti.totle;
    *curtime=ti.current;
    return NGL_OK;
}

DWORD nglMPSeek(HANDLE handle,UINT timems){
    int rc;
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    rc=GxPlayer_MediaSeek(mp->name,timems,0);
    NGLOG_DEBUG("%s rc=%d",mp->name,rc);
    return NGL_OK;
}

DWORD nglSetCallback(HANDLE handle,MP_CALLBACK cbk,void*userdata){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    mp->cbk=cbk;
    mp->userdata=userdata;
}
