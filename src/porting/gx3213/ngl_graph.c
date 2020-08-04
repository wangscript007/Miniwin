#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_disp.h>
#include <ngl_graph.h>
#include <ngl_log.h>
#include <gxcore.h>
#include <av/avapi.h>
NGL_MODULE(GRAPH);

static int device_handle=0;
static int vpu_handle=0;
static int surface_count=0;
//#define USE_DOUBLE_BUFFER 1
typedef struct{
   UINT width;
   UINT height;
   UINT pitch;
   int format;
   int ishw;
   void*buffer;
   void*hw_surface;
   void*back_buffer;
   void*back_surface;
   GxHwMallocObj*vp;
   GxHwMallocObj*vp2;
}NGLSURFACE;

static void* get_idle_surface(NGLSURFACE*fb);

DWORD nglGraphInit(){
    if(0==device_handle){
        nglDispInit();
        device_handle = GxAVCreateDevice(0); 
        vpu_handle = GxAVOpenModule(device_handle, GXAV_MOD_VPU, 0);
        GxAvdev_LayerEnable(device_handle,vpu_handle,GX_LAYER_SPP,FALSE);//Close SPP surface
        NGLOG_DEBUG("device_handle=%p vpu_handle=%p",device_handle,vpu_handle);
    }
    return NGL_OK;
}

DWORD nglGetScreenSize(UINT*width,UINT*height){
    GxVpuProperty_VirtualResolution res;
    GxAVGetProperty(device_handle,vpu_handle,GxVpuPropertyID_VirtualResolution, (void*)&res, sizeof(GxVpuProperty_VirtualResolution));
    *width=res.xres;//vp.width;//1280;//dispCfg.width;
    *height=res.yres;//vp.height;//720;//dispCfg.height;
    NGLOG_DEBUG("Resolution=%dx%d",res.xres,res.yres);

    return NGL_OK;
}

DWORD nglLockSurface(HANDLE surface,void**buffer,UINT*pitch){
    int rc;
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    *buffer=fb->buffer;
    if(fb->ishw)
        *buffer=(get_idle_surface(fb)==fb->hw_surface)?fb->buffer:fb->back_buffer;
    *pitch=fb->pitch;
    return 0;
}

DWORD nglGetSurfaceInfo(HANDLE surface,UINT*width,UINT*height,INT *format)
{
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    if(width)*width =fb->width;
    if(height)*height=fb->height;
    if(format)*format=fb->format;
    return NGL_OK;
}

DWORD nglUnlockSurface(HANDLE surface){
    return 0;
}

DWORD nglSurfaceSetOpacity(HANDLE surface,BYTE alpha){
    int rc=0;
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    GxVpuProperty_Alpha Alpha={0};//crashed ???
    memset(&Alpha,0,sizeof(GxVpuProperty_Alpha));
    Alpha.surface = fb->hw_surface;
    Alpha.alpha.type =GX_ALPHA_PIXEL;//GLOBAL;//GX_ALPHA_PIXEL;
    Alpha.alpha.value=alpha;
    rc=GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_Alpha,&Alpha,sizeof(GxVpuProperty_Alpha));
    if(fb->back_surface){
        Alpha.surface = fb->back_surface;
        rc=GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_Alpha,&Alpha,sizeof(GxVpuProperty_Alpha));
    }
    return fb?NGL_OK:NGL_INVALID_PARA;
}
static int ClearSurface(void*hwsurface,int x,int y,UINT w,UINT h);
DWORD nglFillRect(HANDLE surface,const NGLRect*rect,UINT color){
    int ret = 0;
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    GxVpuProperty_FillRect FillRect;
    GxVpuProperty_BeginUpdate begin = {0};
    GxVpuProperty_EndUpdate end = {0};
    NGLRect rec={0,0,0,0};

    if(rect)rec=*rect;
    else if(fb&&rect==NULL){
        rec.w=fb->width;
        rec.h=fb->height;
    }
    NGLOG_VERBOSE("fb=%p[%dx%d] %d,%d-%d,%d color=0x%08X",fb,fb->width,fb->height,rec.x,rec.y,rec.w,rec.h,color);
    memset(&FillRect, 0, sizeof(GxVpuProperty_FillRect));

    FillRect.color.a = (color >>24) & 0xFF;
    FillRect.color.r = (color >>16) & 0xFF;
    FillRect.color.g = (color >> 8) & 0xFF;
    FillRect.color.b = (color&0xFF);
 
    FillRect.rect.x = rec.x;
    FillRect.rect.y = rec.y;
    FillRect.rect.width = rec.w;
    FillRect.rect.height = rec.h;
    FillRect.is_ksurface = 1;
    FillRect.surface =fb->ishw?get_idle_surface(fb):fb->hw_surface;//fb->hw_surface;

    begin.max_job_num =1;
    ret = GxAVSetProperty(device_handle,vpu_handle, GxVpuPropertyID_BeginUpdate,
                        &begin, sizeof(GxVpuProperty_BeginUpdate));
    ret = GxAVSetProperty(device_handle,vpu_handle, GxVpuPropertyID_FillRect,
                        &FillRect, sizeof(GxVpuProperty_FillRect));

    ret = GxAVSetProperty(device_handle,vpu_handle, GxVpuPropertyID_EndUpdate,
                                &end, sizeof(GxVpuProperty_EndUpdate));
    NGLOG_VERBOSE("FillRect %d,%d-%d,%d color=%08x ret=%d\n",rec.x,rec.y,rec.w,rec.h,color,ret);
    return NGL_OK;
}

static void*get_idle_surface(NGLSURFACE*fb){
#ifdef USE_DOUBLE_BUFFER
    int ret;
    GxVpuProperty_GetIdleSurface IdleSurface = {0};
    IdleSurface.layer = GX_LAYER_OSD;
    ret = GxAVGetProperty(device_handle,vpu_handle, GxVpuPropertyID_GetIdleSurface,
                            &IdleSurface, sizeof(GxVpuProperty_GetIdleSurface));
    return IdleSurface.idle_surface;
#else
    return fb->hw_surface;
#endif
}

DWORD nglFlip(HANDLE surface){
    NGLSURFACE*fb=(NGLSURFACE*)surface;
    NGLOG_VERBOSE("surface=%p ishw=%d",fb,fb->ishw);
#ifdef  USE_DOUBLE_BUFFER
    if(fb->ishw){
        GxVpuProperty_FlipSurface FlipSurface = {0};
        FlipSurface.layer = GX_LAYER_OSD;
        FlipSurface.surface = get_idle_surface(fb);
        GxAVSetProperty(device_handle, vpu_handle,GxVpuPropertyID_FlipSurface,
                   &FlipSurface, sizeof(GxVpuProperty_FlipSurface));
    }
#endif

    return 0;
}

static void*create_hwsurface(UINT width,UINT height,INT format,GxHwMallocObj**vp){
    int ret;
    GxHwMallocObj *ap=(GxHwMallocObj*)GxCore_Mallocz(sizeof(GxHwMallocObj));
    GxVpuProperty_CreateSurface CreateSurface = {0};

    ap->size=width*height*4;
    ap->id=0;
    GxCore_HwMalloc(ap,MALLOC_NO_CACHE);

    memset(&CreateSurface, 0, sizeof(CreateSurface));
    CreateSurface.format = format;
    CreateSurface.width  = width;
    CreateSurface.height = height;
    CreateSurface.mode   = GX_SURFACE_MODE_IMAGE;
    CreateSurface.buffer = ap->kernel_p;

    *vp=ap;

    ret = GxAVGetProperty(device_handle, vpu_handle, GxVpuPropertyID_CreateSurface,
                      &CreateSurface,  sizeof(GxVpuProperty_CreateSurface));
    NGLOG_DEBUG_IF(ret||(ap->usr_p==NULL),"create_hwsurface %dx%d buffer=%p",width,height,ap->usr_p);
    return CreateSurface.surface;
}

static int ClearSurface(void*hwsurface,int x,int y,UINT w,UINT h){
    GxVpuProperty_FillRect FillRect = {0};
    GxVpuProperty_BeginUpdate begin = {0};
    GxVpuProperty_EndUpdate end = {0};

    begin.max_job_num =1;
    GxAVSetProperty(device_handle,vpu_handle, GxVpuPropertyID_BeginUpdate,
                            &begin, sizeof(GxVpuProperty_BeginUpdate));

    FillRect.color.r = 0x00;
    FillRect.color.g = 0x00;
    FillRect.color.b = 0x00;
    FillRect.color.a = 0x00;
    FillRect.rect.x = x;
    FillRect.rect.y = y;
    FillRect.is_ksurface = 1;
    FillRect.rect.width = w;
    FillRect.rect.height= h;
    FillRect.surface = hwsurface;
    GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_FillRect,
                    &FillRect,sizeof(GxVpuProperty_FillRect));

    GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_EndUpdate,
                    &end, sizeof(GxVpuProperty_EndUpdate));

    return (0);
}

DWORD nglCreateSurface(HANDLE*surface,UINT width,UINT height,INT format,BOOL hwsurface)
{
     int ret;
     NGLSURFACE*fb;
     GxAvRect vp;
     GxVpuProperty_RegistSurface RegisterSurface = {0};
     if(format!=GPF_ARGB)
         return NGL_NOT_SUPPORT;
     fb=(NGLSURFACE*)malloc(sizeof(NGLSURFACE));

     fb->width=width;
     fb->height=height;
     fb->pitch=width*4;
     fb->ishw=hwsurface;
     fb->format=GPF_ARGB; 
     fb->back_surface=NULL;
     fb->vp=fb->vp2=NULL;
     fb->hw_surface=create_hwsurface(width,height,GX_COLOR_FMT_ARGB8888,&fb->vp);
     fb->buffer=fb->vp->usr_p;
     if(hwsurface){
         GxAvdev_LayerEnable(device_handle,vpu_handle,GX_LAYER_SPP,FALSE);
         ret=GxAvdev_SetLayerMainSurface(device_handle, vpu_handle, GX_LAYER_OSD,fb->hw_surface);
         NGLOG_VERBOSE("surface=%x hwsurface=%p buffer=%p size=%dx%d pitch=%d",surface,fb->hw_surface,fb->buffer,width,height,fb->pitch);

         vp.x = vp.y = 0;
         vp.width = width;    vp.height = height;

         ret = GxAvdev_SetLayerViewport(device_handle, vpu_handle, GX_LAYER_OSD, &vp);
         ret = GxAvdev_LayerEnable(device_handle, vpu_handle, GX_LAYER_OSD, TRUE);
         //GxAvdev_SetLayerAntiFlicker(device_handle,vpu_handle,GX_LAYER_OSD,1);
         ClearSurface(fb->hw_surface,0,0,width,height);
#ifdef USE_DOUBLE_BUFFER
         fb->back_surface=create_hwsurface(width,height,GX_COLOR_FMT_ARGB8888,&fb->vp2);
         fb->back_buffer=fb->vp2->usr_p;
         ClearSurface(fb->back_surface,0,0,width,height);
         RegisterSurface.layer = GX_LAYER_OSD;
         RegisterSurface.surfaces[0] = fb->hw_surface;
         RegisterSurface.surfaces[1] = fb->back_surface;
         GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_RegistSurface,
                                &RegisterSurface,sizeof(GxVpuProperty_RegistSurface));
#endif

     }
     surface_count++;
     *surface=fb;
     return NGL_OK;
}

#define MIN(x,y) ((x)>(y)?(y):(x))
#define MIN3(x,y,z) ((z)<MIN(x,y)?z:MIN(x,y))
DWORD nglBlit(HANDLE dstsurface,int dx,int dy,HANDLE srcsurface,const NGLRect*srcrect)
{//for gx playtdorm rect's x/y is unsigned int
     int ret;
     NGLSURFACE*dfb=(NGLSURFACE*)dstsurface;
     NGLSURFACE*sfb=(NGLSURFACE*)srcsurface;

     GxVpuProperty_Blit Blit;
     GxVpuProperty_BeginUpdate Begin = {0};
     GxVpuProperty_EndUpdate End = {0};
     NGLRect rs={0,0};//source
     rs.w=sfb->width; rs.h=sfb->height;
     if(srcrect)rs=*srcrect;
     
     NGLOG_VERBOSE("src %p %dx%d-->dst %p %dx%d at %d,%d",sfb,sfb->width,sfb->height,dfb,dfb->width,dfb->height,dx,dy);
     if( ((int)rs.w+dx<=0)||((int)rs.h+dy<=0)||(dx>=(int)dfb->width)||(dy>=(int)dfb->height)||(rs.x<0)||(rs.y<0)){
        return NGL_INVALID_PARA;//source is copy to outer of dest
     }
     if(dx<0){rs.x-=dx;rs.w=(int)rs.w+dx; dx=0;}
     if(dy<0){rs.y-=dy;rs.h=(int)rs.h+dy;dy=0;}
     if(dx+rs.w>dfb->width)rs.w=dfb->width-dx;
     if(dy+rs.h>dfb->height)rs.h=dfb->height-dy;
 
     Begin.max_job_num =1;
     ret = GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_BeginUpdate,
                              &Begin,sizeof(GxVpuProperty_BeginUpdate));

     memset(&Blit, 0, sizeof(GxVpuProperty_Blit));


     Blit.srca.dst_format  = GX_COLOR_FMT_ARGB8888;//get_color_format(0, gui.config.bpp);
     Blit.srca.surface     = sfb->hw_surface;
     Blit.srca.is_ksurface = 1;
     Blit.srca.alpha       = 1;   //??
     Blit.srca.rect.x      = rs.x;
     Blit.srca.rect.y      = rs.y;
     Blit.srca.rect.width  = MIN3(rs.w,dfb->width+dx,(int)dfb->width-dx);//rs.w;
     Blit.srca.rect.height = MIN3(rs.h,dfb->height+dy,(int)dfb->height-dy);//rs.h;

     Blit.srcb.surface     = dfb->ishw?get_idle_surface(dfb):dfb->hw_surface;
     Blit.srcb.dst_format  = GX_COLOR_FMT_ARGB8888;//get_color_format(0, gui.config.bpp);
     Blit.srcb.is_ksurface = 1;
     Blit.srcb.rect.x      =dx;
     Blit.srcb.rect.y      =dy;

     Blit.srcb.rect.width  = MIN3(rs.w,dfb->width+dx,(int)dfb->width-dx);//MIN3(rs.w,(int)rs.w+dx,(int)dfb->width-dx);
     Blit.srcb.rect.height = MIN3(rs.h,dfb->height+dy,(int)dfb->height-dy);//MIN3(rs.h,(int)rs.h+dy,(int)dfb->height-dy);

     Blit.dst = Blit.srcb;
     Blit.mode = GX_ALU_ROP_COPY;
     Blit.colorkey_info.mode = GX_BLIT_COLORKEY_BASIC_MODE;
     Blit.dst.alpha = 0xff;  // no effect.
     Blit.colorkey_info.src_colorkey_en = 0;

     NGLOG_VERBOSE("***%p %d,%d,%d,%d[%dx%d]-->%p %d,%d BeginUpdate=%d",sfb,rs.x,rs.y,rs.w,rs.h,
            Blit.srca.rect.width,Blit.srca.rect.height,dfb,dx,dy,ret);
     ret = GxAVSetProperty(device_handle,vpu_handle, GxVpuPropertyID_Blit,
                             &Blit,sizeof(GxVpuProperty_Blit));
     ret = GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_EndUpdate,
                             &End, sizeof(GxVpuProperty_EndUpdate));
     return 0;
}

DWORD nglDestroySurface(HANDLE surface)
{
     int ret;
     NGLSURFACE*fb=(NGLSURFACE*)surface;
     GxVpuProperty_DestroySurface DesSurface = { 0 };
     memset(&DesSurface, 0, sizeof(DesSurface));
     DesSurface.surface = fb->hw_surface;
     if(fb->vp)GxCore_HwFree(fb->vp);
     if(fb->vp2)GxCore_HwFree(fb->vp2);
     ret = GxAVSetProperty(device_handle,vpu_handle,GxVpuPropertyID_DestroySurface,
                      &DesSurface, sizeof(GxVpuProperty_DestroySurface));
     surface_count--;
     NGLOG_DEBUG("surface=%p ret=%d surfacecount=%d",surface,ret,surface_count);
     free(fb);
     return 0;
}
