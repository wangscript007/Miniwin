
#include <ngl_types.h>
#include <ngl_mediaplayer.h>
#include <ngl_log.h>
#include <ngl_os.h>

NGL_MODULE(NGLMP)

typedef struct{
   void*hplayer;
}NGL_PLAYER;
static void MPCallbackFunc( enum aui_mp_message msg, void *data, void *user_data );
DWORD nglMPOpen(const char*fname){
    NGL_PLAYER *mp=(NGL_PLAYER*)malloc(sizeof(NGL_PLAYER));
    char*p=strchr(fname,':');
    return mp;
}

DWORD nglMPPlay(DWORD handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPStop(DWORD handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPResume(DWORD handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}
DWORD nglMPPause(DWORD handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPClose(DWORD handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPGetTime(DWORD handle,UINT*curtime,UINT*timems){
    return NGL_OK;
}
DWORD nglMPSeek(DWORD handle,UINT timems){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    //mpg_cmd_set_speed
}

