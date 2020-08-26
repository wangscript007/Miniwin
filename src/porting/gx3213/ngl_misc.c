#include<gxcore.h>
#include<ngl_types.h>
#include<ngl_log.h>

NGL_MODULE(MISC);

int nglSysInit(){
    static int inited=0;
    if(inited)
       return 0;
    inited++;
    setegid(1024);
    GxCore_DownUserPermissions();
    /*** Initialize the OS API data structures*/
    NGLOG_DEBUG("goxceed vserson %s",GOXCEED_VERSON);
    GxCore_Init();
    GxCore_PartitionFlashInit();
    GxCore_HwMallocInit();
    return 0;
}

vbi_fetch_vt_flags(){
    NGLOG_INFO("DUMMY");
}
