#include<ngl_types.h>
#include<ngl_log.h>
#include<ngl_disp.h>

NGL_MODULE(DISP)

DWORD nglDispInit(){
    NGLOG_DEBUG("TODO");
    return NGL_OK;
}

DWORD nglDispSetResolution(int res){
    switch(res){
    case DISP_RES_1080I:
    case DISP_RES_1080P:
        break;
    case DISP_RES_720I:
    case DISP_RES_720P:
        break;
    case DISP_RES_576I:
    case DISP_RES_576P:
    case DISP_RES_480I:
    default:break;
    }
    NGLOG_DEBUG("res=%d",res);
    return NGL_OK;
}

DWORD nglDispSetAspectRatio(int ratio){
    int rc;
    switch(ratio){
    case DISP_APR_AUTO:   break;
    case DISP_APR_4_3:    break;
    case DISP_APR_16_9:   break;
    default:NGLOG_DEBUG("Invalid value %d",ratio);return NGL_ERROR;
    }    
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
    NGLOG_DEBUG("rc=%d value=%d",rc,value);
}

DWORD nglDispSetContrast(int value){
    int rc;
    NGLOG_DEBUG("rc=%d value=%d",rc,value);
}

DWORD nglDispSetSaturation(int value){
    int rc=0;
    NGLOG_DEBUG("rc=%d value=%d ",value,rc);
}

