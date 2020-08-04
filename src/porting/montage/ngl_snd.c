#include <ngl_types.h>
#include <ngl_log.h>
#include <ngl_snd.h>
NGL_MODULE(AUDIO)

INT nglSndInit(){
    return NGL_OK;
}

INT nglSndSetVolume(int idx,int vol){
    int rc=0;
    NGLOG_VERBOSE("vol=%d rc=%d",vol,rc);
    return NGL_OK;
}

INT nglSndGetColume(int idx){
    BYTE vol;
    NGLOG_VERBOSE(" vol=%d",vol);
    return vol;
} 
INT nglSndSetMute(int idx,BOOL mute){
    return 0;
}

INT nglSndSetOutput(int ifc,int type){
    return 0;
}

