#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_graph.h>
#include <ngl_log.h>
#include <gtk/gtk.h>

NGL_MODULE(GRAPH);

GtkWidget *ng_window, *ng_image;
//GtkImage *image;
GdkPixbuf *ng_pixbuf;

static void GTKProc(void*param){
     gtk_init(0,NULL);
     ng_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
     ng_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8,1280, 720);
     ng_image = gtk_image_new_from_pixbuf(ng_pixbuf);
     gtk_container_add(GTK_CONTAINER(ng_window), GTK_WIDGET(ng_image));
     gtk_widget_show_all(ng_window);
     gtk_main();
}

DWORD nglGraphInit(){
    
    return NGL_OK;
}

DWORD nglGetScreenSize(UINT*width,UINT*height){
    *width=1280;//dispCfg.width;
    *height=720;//dispCfg.height;
    return NGL_OK;
}

DWORD nglLockSurface(DWORD surface,void**buffer,UINT*pitch){
    GdkPixbuf*pb=(GdkPixbuf*)surface;
    *buffer= gdk_pixbuf_get_pixels(pb);
    *pitch = gdk_pixbuf_get_rowstride(pb);
    return 0;
}

DWORD nglGetSurfaceInfo(DWORD surface,UINT*width,UINT*height,INT *format)
{
    GdkPixbuf*pb=(GdkPixbuf*)surface;
    *width = gdk_pixbuf_get_width(pb);;
    *height= gdk_pixbuf_get_height(pb);
    *format=GPF_ARGB;
    return NGL_OK;
}

DWORD nglUnlockSurface(DWORD surface){
    return 0;
}

DWORD nglSurfaceSetOpacity(DWORD surface,BYTE alpha){
    return 0;//dispLayer->SetOpacity(dispLayer,alpha);
}

DWORD nglFillRect(DWORD surface,const NGLRect*rec,UINT color){
    return NGL_OK;
}

DWORD nglFlip(DWORD surface){
    return 0;
}

DWORD nglCreateSurface(DWORD*surface,INT width,INT height,INT format,BOOL hwsurface)
{
     GdkPixbuf*pixbuf=ng_pixbuf;
     if(!hwsurface) {
         GdkPixbuf*pixbuf= gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8,width,height);    
	 surface=pixbuf;
     }
     surface=pixbuf;
     NGLOG_DEBUG("surface=%x pixbuf=%p",surface,pixbuf);
     return NGL_OK;
}


DWORD nglBlit(DWORD dstsurface,DWORD srcsurface,const NGLRect*srcrect,const NGLRect* dstrect)
{
     GdkPixbuf*src=(GdkPixbuf*)srcsurface;
     GdkPixbuf*dst=(GdkPixbuf*)dstsurface;
     gdk_pixbuf_copy_area(dst,(dstrect?dstrect->x:0),(dstrect?dstrect->y:0),
	  (dstrect?dstrect->w: gdk_pixbuf_get_width(src)),(dstrect?dstrect->h: gdk_pixbuf_get_width(src)),
          src,(srcrect?srcrect->x:0),(srcrect?srcrect->y:0));
//	  (srcrect?srcrect->w:src->w),
//	  (srcrect?srcrect->h:src->h));
     return 0;
}

DWORD nglDestroySurface(DWORD surface)
{
     gdk_pixbuf_destroy((GdkPixbuf*)surface);
     return 0;
}
