#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_snd.h>
NGL_MODULE(AUDIO);

INT nglSndInit(){
    return NGL_OK;
}

static int volume=50;
INT nglSndSetVolume(int idx,int vol){
    int rc=0;
    volume=vol;
    NGLOG_VERBOSE("vol=%d rc=%d",vol,rc);
    return NGL_OK;
}

INT nglSndGetColume(int idx){
    NGLOG_VERBOSE(" vol=%d",volume);
    return volume;
} 
INT nglSndSetMute(int idx,BOOL mute){
    return NGL_OK;
}

INT nglSndSetOutput(int ifc,int type){
    return 0;
}

