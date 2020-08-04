#include <ngl_types.h>
#include <ngl_pvr.h>
#include <ngl_log.h>
#include <ngl_timer.h>


NGL_MODULE(PVR);

static unsigned char*pvr_buffer=NULL;
typedef struct{
   char *path;
}NGLPVR;

DWORD nglPvrInit(){
   return 0;
}


DWORD nglPvrRecordOpen(const char*record_path,const NGLPVR_RECORD_PARAM*param){
    return 0;
}

DWORD nglPvrRecordPause(DWORD handler){
    return NGL_OK;
}

DWORD nglPvrRecordResume(DWORD handler){
    return NGL_OK;
}

DWORD nglPvrRecordClose(DWORD handler){
    return NGL_OK;
}

void nglGetPvrPath(DWORD handler,char*path){
}

///////////////////////////////PVR PLAYER////////////////////////////

DWORD nglPvrPlayerOpen(const char*pvrpath){
    return 0;
}

DWORD nglPvrPlayerPlay(DWORD handle){
    return NGL_OK;
}

DWORD nglPvrPlayerStop(DWORD handle){
    return NGL_OK;
}

DWORD nglPvrPlayerPause(DWORD handle){
    return NGL_OK;
}

DWORD nglPvrPlayerClose(DWORD handle){
    return NGL_OK;
}

