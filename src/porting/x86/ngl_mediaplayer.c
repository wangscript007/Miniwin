
#include <ngl_types.h>
#include <ngl_mediaplayer.h>
#include <ngl_log.h>
#include <ngl_os.h>

NGL_MODULE(NGLMP);

typedef struct{
   void*hplayer;
   MP_CALLBACK cbk;
   void*userdata;
}NGL_PLAYER;
static void MPCallbackFunc( enum aui_mp_message msg, void *data, void *user_data );
HANDLE nglMPOpen(const char*fname){
    NGL_PLAYER *mp=(NGL_PLAYER*)malloc(sizeof(NGL_PLAYER));
    memset(mp,0,sizeof(NGL_PLAYER));
    char*p=strchr(fname,':');
    return mp;
}

DWORD nglMPPlay(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPStop(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPResume(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPPause(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPClose(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    return NGL_OK;
}

DWORD nglMPGetTime(HANDLE handle,UINT*curtime,UINT*timems){
    return NGL_OK;
}
DWORD nglMPSeek(HANDLE handle,UINT timems){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    //mpg_cmd_set_speed
}

DWORD nglSetCallback(HANDLE handle,MP_CALLBACK cbk,void*userdata){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    mp->cbk=cbk;
    mp->userdata=userdata;
}

