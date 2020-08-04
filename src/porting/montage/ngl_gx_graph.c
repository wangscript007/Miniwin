#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_graph.h>
#include <ngl_log.h>
NGL_MODULE(GRAPH)

static int device_handle=0;
static int vpu_handle=0;

typedef struct{
   UINT width;
   UINT height;
   UINT pitch;
   int format;
   int ishw;
   void*buffer;
   void*hw_surface;
}NGLSURFACE;

DWORD nglGraphInit(){
    return NGL_OK;
}

DWORD nglGetScreenSize(UINT*width,UINT*height){
    *width=1280;//dispCfg.width;
    *height=720;//dispCfg.height;
    return NGL_OK;
}

DWORD nglLockSurface(HANDLE surface,void**buffer,UINT*pitch){
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    *buffer=fb->buffer;
    *pitch=fb->pitch;
    return 0;
}

DWORD nglGetSurfaceInfo(HANDLE surface,UINT*width,UINT*height,INT *format)
{
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    *width =fb->width;
    *height=fb->height;
    *format=fb->format;
    return NGL_OK;
}

DWORD nglUnlockSurface(HANDLE surface){
    return 0;
}

DWORD nglSurfaceSetOpacity(HANDLE surface,BYTE alpha){
    return 0;//dispLayer->SetOpacity(dispLayer,alpha);
}

DWORD nglFillRect(HANDLE surface,const NGLRect*rect,UINT color){
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    return NGL_OK;
}

DWORD nglFlip(HANDLE surface){
    NGLSURFACE*fb=(NGLSURFACE*)surface;

    return 0;
}

DWORD nglCreateSurface(HANDLE*surface,UINT width,UINT height,INT format,BOOL hwsurface)
{
     int ret;
     NGLSURFACE*fb;

     fb=malloc(sizeof(NGLSURFACE));
     fb->width=width;
     fb->height=height;
     fb->ishw=hwsurface;
     fb->buffer=ap.usr_p;
     fb->format=GPF_ARGB; 
     NGLOG_DEBUG("surface=%x ",surface);

     return NGL_OK;
}


DWORD nglBlit(HANDLE dstsurface,int dx,int dy,HANDLE srcsurface,const NGLRect*srcrect)
{
     int ret;
     NGLSURFACE*dfb=(NGLSURFACE*)dstsurface;
     NGLSURFACE*sfb=(NGLSURFACE*)srcsurface;
     return 0;
}

DWORD nglDestroySurface(HANDLE surface)
{
     int ret;
     NGLSURFACE*fb=(NGLSURFACE*)surface;
     free(fb);
     return 0;
}
