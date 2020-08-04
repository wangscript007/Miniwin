
#include <ngl_types.h>
#include <ngl_mediaplayer.h>
#include <aui_common.h>
#include <aui_mp.h>
#include <aui_dis.h>
#include <ngl_log.h>
#include <ngl_os.h>

NGL_MODULE(NGLMP);

typedef struct{
   void*hplayer;
   aui_attr_mp attr;
   MP_CALLBACK cbk;
   void *userdata;
}NGL_PLAYER;
static void MPCallbackFunc( enum aui_mp_message msg, void *data, void *user_data );

HANDLE nglMPOpen(const char*fname){
    aui_hdl hdl;
    NGL_PLAYER *mp=(NGL_PLAYER*)malloc(sizeof(NGL_PLAYER));
    char*p=strchr(fname,':');
    //aui_log_priority_set(AUI_MODULE_MP,AUI_LOG_PRIO_DEBUG);
    memset(mp, 0, sizeof(NGL_PLAYER));
    if(p==NULL){
        strcpy(mp->attr.uc_file_name,"file://");
        strcat(mp->attr.uc_file_name,fname);
        mp->attr.stream_protocol = AUI_MP_STREAM_PROTOCOL_UNKNOW;
    }else{
        mp->attr.stream_protocol = AUI_MP_STREAM_PROTOCOL_LIVE;
        strcpy(mp->attr.uc_file_name,fname);
    }
    mp->attr.aui_mp_stream_cb =MPCallbackFunc;
    mp->attr.user_data = mp;
    mp->attr.b_is_preview =false;// b_is_preview;
    mp->attr.start_time = 0;

    int rc=aui_mp_open(&mp->attr,&mp->hplayer);
    NGLOG_DEBUG("mp_open=%d hdl=%p uri=%s",rc,hdl,mp->attr.uc_file_name);
    return mp;
}

DWORD nglMPPlay(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    NGLOG_DEBUG("hdl=%p",mp);
    int rc=aui_mp_set_buffering_time(mp->hplayer,10,5,30);
    NGLOG_DEBUG("_mp_set_buffering_time=%d hdl=%p",rc,mp);
    rc=aui_mp_set_start2play_percent(mp->hplayer,3);
    NGLOG_DEBUG("mp_set_start2play_percent=%d hdl=%p",rc,mp);
    rc=aui_mp_start(mp->hplayer);
    NGLOG_DEBUG("mp_start=%d hdl=%p",rc,mp);
    return NGL_OK;
}

DWORD nglMPStop(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    int rc=aui_mp_stop(mp->hplayer);
    NGLOG_DEBUG("rc=%d hdl=%p",rc,mp);
    return NGL_OK;
}

DWORD nglMPResume(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    int rc=aui_mp_resume(mp->hplayer);
    NGLOG_DEBUG("rc=%d hdl=%p",rc,mp);
    return NGL_OK;
}
DWORD nglMPPause(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    int rc=aui_mp_pause(mp->hplayer);
    NGLOG_DEBUG("rc=%d hdl=%p",rc,mp);
    return NGL_OK;
}

DWORD nglMPClose(HANDLE handle){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    int rc=aui_mp_close(&mp->attr,&mp->hplayer);
    NGLOG_DEBUG("rc=%d hdl=%p",rc,mp);
    free(mp);
    return NGL_OK;
}

DWORD nglMPGetTime(HANDLE handle,UINT*curtime,UINT*timems){
    aui_hdl hdl=(aui_hdl)handle;
    UINT t1,t2;
    int rc=0;
    rc=aui_mp_get_cur_time(hdl,&t1);
    rc=aui_mp_total_time_get(hdl,&t2);
    if(curtime)*curtime=t1;
    if(timems)*timems=t2;
    NGLOG_DEBUG("rc=%d hdl=%p times=%d/%d",rc,hdl,t1,t2);
    return NGL_OK;
}
DWORD nglMPSeek(HANDLE handle,UINT timems){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    //mpg_cmd_set_speed
}

DWORD nglMPSetCallback(HANDLE handle,MP_CALLBACK cbk,void*userdata){
    NGL_PLAYER*mp=(NGL_PLAYER*)handle;
    mp->cbk=cbk;
    mp->userdata=userdata;
}
static void MPCallbackFunc( enum aui_mp_message msg, void *data, void *user_data )
{
    NGL_PLAYER*mp=(NGL_PLAYER*)user_data;
    #define MPCBK(msg,param) if(mp->cbk)mp->cbk(mp,msg,param,mp->userdata)
    NGLOG_DEBUG("callback msg:");
    switch ( msg ) {
    case AUI_MP_PLAY_BEGIN: {
        NGLOG_DEBUG("AUI_MP_PLAY_BEGIN");
        break;
        }
    case AUI_MP_PLAY_END: {
        MPCBK(MP_END,0);
        NGLOG_DEBUG( "AUI_MP_PLAY_END userdata=%p",user_data);
        break;
       }
    case AUI_MP_VIDEO_CODEC_NOT_SUPPORT: {
        MPCBK(MP_ERROR,0);
        NGLOG_DEBUG("AUI_MP_VIDEO_CODEC_NOT_SUPPORT");
        break;
        }
    case AUI_MP_AUDIO_CODEC_NOT_SUPPORT: {
        MPCBK(MP_ERROR,0);
        NGLOG_DEBUG("AUI_MP_AUDIO_CODEC_NOT_SUPPORT");
        break;
    }
    case AUI_MP_RESOLUTION_NOT_SUPPORT: {
        MPCBK(MP_ERROR,0);
        NGLOG_DEBUG("AUI_MP_RESOLUTION_NOT_SUPPORT");
        break;
    }
    case AUI_MP_FRAMERATE_NOT_SUPPORT: {
        MPCBK(MP_ERROR,0);
        NGLOG_DEBUG("AUI_MP_FRAMERATE_NOT_SUPPORT");
        break;
    }
    case AUI_MP_NO_MEMORY: {
        MPCBK(MP_ERROR,0);
        NGLOG_DEBUG("AUI_MP_NO_MEMORY");
        break;
    }
    case AUI_MP_DECODE_ERROR: {
        MPCBK(MP_ERROR,0);
        NGLOG_DEBUG("AUI_MP_DECODE_ERROR");
        break;
    }
    case AUI_MP_ERROR_UNKNOWN: {
        MPCBK(MP_ERROR,0);
        NGLOG_DEBUG("AUI_MP_ERROR_UNKNOWN%s", ( char * )data );
        break;
    }
    case AUI_MP_BUFFERING: {
        NGLOG_DEBUG("AUI_MP_BUFFERING");
        break;
    }
    case AUI_MP_ERROR_SOUPHTTP: {
        NGLOG_DEBUG("AUI_MP_ERROR_SOUPHTTP");
        break;
    }
    case AUI_MP_FRAME_CAPTURE: {
        char path[128], frameInfo[128];
        unsigned int h, w;
        sscanf( ( char * )data, "%[^;];h=%u,w=%u", path, &h, &w );
        NGLOG_DEBUG("frame captured path %s", path );
        NGLOG_DEBUG("frame width=%u, height=%u", w, h );
        NGLOG_DEBUG("AUI_MP_FRAME_CAPTURE" );
        break;
    }
    default:
        NGLOG_DEBUG("unkown callback message:%d", msg );
        break;
    }
}
