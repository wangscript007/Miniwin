#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_graph.h>
#include <ngl_log.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> /* BitmapOpenFailed, etc. */
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <stdlib.h> /* getenv(), etc. */
#include <unistd.h> 

NGL_MODULE(GRAPH);

typedef struct{
   XImage *image;
   void*data;
}NGLSURFACE;

static Display*display=NULL;
static Window win;
static GC gc;
static NGLSURFACE*hwsurface;

static void graph_done(){
   XFreeGC(display,gc);
   XDestroyWindow(display,win);
   XCloseDisplay(display);
   NGLOG_INFO("Close X11 resources");
}
static void event_proc(void*p){
   XEvent ev;
   NGLOG_DEBUG("X11 Event Proc...");
   while(1){
	XNextEvent(display, &ev);
        switch( ev.type ){
        case  Expose:
              NGLOG_DEBUG("Expose");
              XPutImage(display,win,gc,hwsurface->image, 0, 0,0,0, hwsurface->image->width,hwsurface->image->height);
              XFlush(display);
              break;
        case DestroyNotify://XDestroyWindowEvent *destroywindowevent = (XDestroyWindowEvent *)&event;
             NGLOG_DEBUG("X11 Destroied");
             return ;
        default:NGLOG_DEBUG("X11Event %d",ev.type);break;
        }
   }
}

DWORD nglGraphInit(){
    int x=0,y=0;
    DWORD tid;
    int width=1280,height=720;
    int screen_num,win_border_width=0;
    XGCValues values;
    if(display!=NULL)return NGL_OK;
    display=XOpenDisplay(getenv("DISPLAY"));
    screen_num = DefaultScreen(display);
    /* create a simple window, as a direct child of the screen's */
    /* root window. Use the screen's black and white colors as  */
    /* the foreground and background colors of the window,      */
    /* respectively. Place the new window's top-left corner at  */
    /* the given 'x,y' coordinates.                              */
    win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                            x, y, width, height, win_border_width,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num));

    /* make the window actually appear on the screen. */
    XMapWindow(display, win);

    /* flush all pending requests to the X server. */
    XFlush(display);
    
    gc=XCreateGC(display, win,0,&values);
    XSetForeground(display, gc, BlackPixel(display, screen_num));
    XSetBackground(display, gc, WhitePixel(display, screen_num));
    XSetLineAttributes(display, gc,1,LineSolid,CapButt,JoinBevel);
    XSetFillStyle(display, gc, FillSolid);
    nglCreateThread(&tid,0,0,event_proc,NULL);
    atexit(graph_done);  
    NGLOG_DEBUG("display=%p screen_num=%d",display,screen_num); 
    return NGL_OK;
}

DWORD nglGetScreenSize(UINT*width,UINT*height){
    int screen_num = DefaultScreen(display);
    *width = DisplayWidth(display, screen_num);
    *height = DisplayHeight(display, screen_num);
    NGLOG_DEBUG("size=%dx%d",*width,*height);
    return NGL_OK;
}

DWORD nglLockSurface(DWORD surface,void**buffer,UINT*pitch){
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    *buffer=ngs->image->data;
    *pitch=ngs->image->bytes_per_line;
    NGLOG_DEBUG("%p buffer=%p pitch=%d",ngs,*buffer,*pitch);
    return 0;
}

DWORD nglGetSurfaceInfo(DWORD surface,UINT*width,UINT*height,INT *format)
{
    //*height= gdk_pixbuf_get_height(pb);
    *format=GPF_ARGB;
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    *width=ngs->image->width;
    *height=ngs->image->height;
    return NGL_OK;
}

DWORD nglUnlockSurface(DWORD surface){
    return 0;
}

DWORD nglSurfaceSetOpacity(DWORD surface,BYTE alpha){
    return 0;//dispLayer->SetOpacity(dispLayer,alpha);
}

DWORD nglFillRect(DWORD surface,const NGLRect*rec,UINT color){
    XColor xcol;
    Colormap colormap;
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    xcol.red=(color>>16)&0xFF;
    xcol.green=(color>>8)&0xFF;
    xcol.blue=color&0xFF;
    xcol.flags=DoRed | DoBlue | DoGreen;
    colormap=DefaultColormap( display, DefaultScreen( display ) );
    XAllocColor(display,colormap,&xcol);
    XSetForeground( display, gc, xcol.pixel);
    XFillRectangle(display,ngs->image,gc,rec->x,rec->y,rec->w,rec->h);
    NGLOG_DEBUG("FillRect %p color=0x%x",ngs,color);
    return NGL_OK;
}

DWORD nglFlip(DWORD surface){
    XEvent e;
    NGLSURFACE*ngs=(NGLSURFACE*)surface;
    NGLOG_DEBUG("flip %p",ngs);
    XCopyPlane(display, ngs->image, win, gc, 0, 0, ngs->image->width, ngs->image->height,ngs->image->width, ngs->image->height, 1);
    XFlush(display);
    XSendEvent(display, win, 1, ExposureMask, &e);
    return 0;
}

DWORD nglCreateSurface(DWORD*surface,INT width,INT height,INT format,BOOL ishwsurface)
{//XShmCreateImage XShmCreatePixmap
     NGLSURFACE*nglsurface=(NGLSURFACE*)malloc(sizeof(NGLSURFACE));
     Visual *visual = DefaultVisual(display, DefaultScreen(display));
     int depth =32;// DefaultDepth(display, DefaultScreen(display));
     void*data=malloc(width*4*height);
     nglsurface->image=XCreateImage(display, visual, depth, ZPixmap, 0,data, width, height,8,width*4 );
     if(ishwsurface)hwsurface=nglsurface;
     *surface=(DWORD)nglsurface;
     NGLOG_DEBUG("surface=%x size=%dx%d depth=%d",surface,width,height,depth);
     return NGL_OK;
}


DWORD nglBlit(DWORD dstsurface,DWORD srcsurface,const NGLRect*srcrect,const NGLRect* dstrect)
{
     unsigned int sx,sy,sw,sh,dx,dy;
     NGLSURFACE*ndst=(NGLSURFACE*)dstsurface;
     NGLSURFACE*nsrc=(NGLSURFACE*)srcsurface;
     sx=srcrect?srcrect->x:0;    sy=srcrect?srcrect->y:0;
     sw=nsrc->image->width;      sh=nsrc->image->height;
     dx=dstrect?dstrect->x:0;    dy=dstrect?dstrect->x:0;
     //dw=ndst->image->width;      dh=ndst->image->height;
     NGLOG_DEBUG("Blit from %p %d,%d-%d,%d to %p %d,%d",nsrc,sx,sy,sw,sh,ndst,dx,dy);
     XCopyArea(display,nsrc->image,ndst->image,gc,sx,sy,sw,sh,dx,dy);
     return 0;
}

DWORD nglDestroySurface(DWORD surface)
{
     NGLSURFACE*ngs=(NGLSURFACE*)surface;
     XDestroyImage(ngs->image);
     free(ngs);
     return 0;
}
