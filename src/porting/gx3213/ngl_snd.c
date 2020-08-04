#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_snd.h>
#include <bus/module/player/gxplayer_module.h>

NGL_MODULE(AUDIO);

INT nglSndInit(){
    return NGL_OK;
}
static int volume;
INT nglSndSetVolume(int idx,int vol){
    int rc=0;
    GxPlayer_SetAudioVolume(vol);
    volume=vol;
    NGLOG_VERBOSE("vol=%d rc=%d",vol,rc);
    return NGL_OK;
}

INT nglSndGetColume(int idx){
    NGLOG_VERBOSE(" vol=%d",volume);
    return volume;
} 
INT nglSndSetMute(int idx,BOOL mute){
    GxPlayer_SetAudioMute(mute);
    return 0;
}

INT nglSndSetOutput(int ifc,int type){
    return 0;
}

