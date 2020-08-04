#include<ngl_panel.h>
#include<ngl_log.h>

DWORD nglFPInit(){
    return NGL_OK;
}

DWORD nglFPShowText(const char*txt,int len){
    
    return NGL_OK;
}

DWORD nglFPSetBrightness(int value){
    int rc=0;//aui_panel_set_led_brightness_level(hdl_panel,value);
    return NGL_OK;
}
