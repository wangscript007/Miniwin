#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_graph.h>
#include <ngl_log.h>
#include <stdio.h>
#include <stdlib.h> /* getenv(), etc. */
#include <unistd.h> 
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <ngl_input.h>

NGL_MODULE(GRAPH);

typedef struct{
   unsigned int width;
   unsigned int height;
   unsigned int pitch;
   void*data;
   unsigned int ishw;
}NGLSURFACE;

static NGLSURFACE*hwsurface;

void SENDKEY(int k){
   NGLKEYINFO ki;
   ki.key_code=k;
   ki.repeat=1;
   ki.state=NGL_KEY_RELEASE;//NGL_KEY_PRESSED:NGL_KEY_RELEASE;
   nglIrSendKey(NULL,&ki,10);
}
DWORD nglGraphInit(){
    int x=0,y=0,argc=0,*fb;
    HANDLE tid;
    int width=1280,height=720;
    gtk_init(0,NULL);
    return NGL_OK;
}

DWORD nglGetScreenSize(UINT*width,UINT*height){
    *width = rfbScreen->width;
    *height = rfbScreen->height;
    NGLOG_DEBUG("size=%dx%d",*width,*height);
    return NGL_OK;
}

DWORD nglLockSurface(HANDLE surface,void**buffer,UINT*pitch){
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    *buffer=ngs->data;
    *pitch=ngs->pitch;
    //NGLOG_DEBUG("%p buffer=%p pitch=%d",ngs,*buffer,*pitch);
    return 0;
}

DWORD nglGetSurfaceInfo(HANDLE surface,UINT*width,UINT*height,INT *format)
{
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    *width=ngs->width;
    *height=ngs->height;
    *format=GPF_ABGR;
    return NGL_OK;
}

DWORD nglUnlockSurface(HANDLE surface){
    return 0;
}

DWORD nglSurfaceSetOpacity(HANDLE surface,BYTE alpha){
    return 0;//dispLayer->SetOpacity(dispLayer,alpha);
}

DWORD nglFillRect(HANDLE surface,const NGLRect*rect,UINT color){
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    UINT x,y;
    NGLRect recsurface={0,0,ngs->width,ngs->height};
    NGLRect *rec=(rect?rect:&recsurface);
    NGLOG_VERBOSE("FillRect %p %d,%d-%d,%d color=0x%x pitch=%d",ngs,rec->x,rec->y,rec->w,rec->h,color,ngs->pitch);
    UINT*fb=(UINT*)(ngs->data+ngs->pitch*rec->y+x*4);
    for(y=0;y<rec->h;y++){
        for(x=0;x<rec->w;x++)
           fb[x]=color&0x00FFFFFF;
        fb+=(ngs->pitch>>2);
    }
    rfbFillRect(rfbScreen,rec->x,rec->y,rec->x+rec->w,rec->y+rec->h,color);
    rfbMarkRectAsModified(rfbScreen,rec->x,rec->y,rec->x+rec->w,rec->y+rec->h);
    return NGL_OK;
}

DWORD nglFlip(HANDLE surface){
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    rfbMarkRectAsModified(rfbScreen,0,0,rfbScreen->width,rfbScreen->height);
    NGLOG_VERBOSE("flip %p",ngs);
    return 0;
}

DWORD nglCreateSurface(HANDLE*surface,INT width,INT height,INT format,BOOL ishwsurface)
{//XShmCreateImage XShmCreatePixmap
     NGLSURFACE*nglsurface=(NGLSURFACE*)malloc(sizeof(NGLSURFACE));
     if(ishwsurface){
         nglsurface->data=rfbScreen->frameBuffer;
         hwsurface=nglsurface;
     }else{
         nglsurface->data=malloc(width*height*4);
     }
     nglsurface->ishw=ishwsurface;
     nglsurface->width=width;
     nglsurface->height=height;
     nglsurface->pitch=width*4;
     *surface=(HANDLE)nglsurface;
     NGLOG_DEBUG("surface=%p/%p framebuffer=%p size=%dx%d hw=%d",nglsurface,*surface,nglsurface->data,width,height,ishwsurface);
     return NGL_OK;
}


DWORD nglBlit(HANDLE dstsurface,HANDLE srcsurface,const NGLRect*srcrect,const NGLRect* dstrect)
{
     unsigned int x,y,sx,sy,sw,sh,dx,dy;
     NGLSURFACE*ndst=(NGLSURFACE*)dstsurface;
     NGLSURFACE*nsrc=(NGLSURFACE*)srcsurface;
     UINT *pbs=(UINT*)nsrc->data;
     UINT *pbd=(UINT*)ndst->data;
     sx=srcrect?srcrect->x:0;    sy=srcrect?srcrect->y:0;
     sw=nsrc->width;      sh=nsrc->height;
     dx=dstrect?dstrect->x:0;    dy=dstrect?dstrect->x:0;

     //NGLOG_DEBUG("Blit from %p->%p %d,%d-%d,%d to %p %d,%d buffer=%p->%p",nsrc,ndst,sx,sy,sw,sh,ndst,dx,dy,pbs,pbd);
     pbs+=sy*(nsrc->pitch>>2)+sx;
     pbd+=dy*(ndst->pitch>>2)+dx;
     NGLOG_VERBOSE("buffer %p->%p pitch=%d/%d sw=%dx%d",pbs,pbd,nsrc->pitch,ndst->pitch,sw,sh);
     for(y=0;y<sh;y++){
         for(x=0;x<sw;x++)pbd[x]=pbs[x]&0x00FFFFFF;
         pbs+=(nsrc->pitch>>2);
         pbd+=(ndst->pitch>>2);
     }
     //rfbNewFramebuffer(rfbScreen, (char*)rfbScreen->frameBuffer, rfbScreen->width,rfbScreen->height, 8, 3,4);
     rfbMarkRectAsModified(rfbScreen,0,0,rfbScreen->width,rfbScreen->height);//sx,sy,sx+sw,sy+sh);
     rfbDoCopyRect(rfbScreen,0,0,rfbScreen->width,rfbScreen->height,0,0);
     return 0;
}

DWORD nglDestroySurface(HANDLE surface)
{
     NGLSURFACE*ngs=(NGLSURFACE*)surface;
     if(ngs->data&&(!ngs->ishw)){
        free(ngs->data);
     }
     free(ngs);
     return 0;
}
