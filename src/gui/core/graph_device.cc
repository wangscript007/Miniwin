#include <graph_context.h>
#include <graph_device.h>
#include <cairo.h>
#include <ngl_types.h>
#include <ngl_os.h>
#include <ngl_graph.h>
#include <ngl_log.h>
#include <ngl_misc.h>
#include <cairo-ngl.h>
#include <vector>
#include <cairomm/context.h>
#include <cairomm/ngl_surface.h>
#include <cairomm/region.h>
#include <cairomm/fontface.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <fontmanager.h>
#include <pixman.h>
#include <chrono>

using namespace std;
using namespace Cairo;
using namespace std::chrono;

NGL_MODULE(GraphDevice);

namespace nglui{

std::vector<GraphContext*>GraphDevice::gSurfaces;
GraphDevice*GraphDevice::mInst=nullptr;

GraphDevice&GraphDevice::GraphDevice::getInstance(){
    if(nullptr==mInst)
        mInst=new GraphDevice();
    return *mInst;
}

GraphDevice::GraphDevice(int fmt){
    mInst=this;
    compose_event=0;
    nglSysInit();
    nglGraphInit();
    nglGetScreenSize((UINT*)&width,(UINT*)&height);

    format=fmt<0?GPF_ARGB:fmt;
    nglCreateSurface(&primarySurface,width,height,format,1);
    last_compose_time=(std::chrono::steady_clock::now().time_since_epoch()).count();  
    NGLOG_DEBUG("primarySurface=%p size=%dx%d",primarySurface,width,height);

    primaryContext=new GraphContext(mInst,primarySurface);
#ifndef CAIRO_HAS_FC_FONT
    const char*fontpath=getenv("FONT_PATH");
    if(fontpath){
        NGLOG_DEBUG("load font from %s",fontpath);
        FontManager::getInstance().loadFonts(fontpath);
    }else{
        FontManager::getInstance().loadFonts("/usr/share/fonts");
        FontManager::getInstance().loadFonts("/usr/lib/fonts");
    }
    FT_Init_FreeType(&ft_library);
    std::vector<std::string>families;
    FontManager::getInstance().getFamilies(families);
    NGLOG_DEBUG("Fonts count=%d",families.size());
    for(auto family:families){
        FT_Face ft_face;
        std::string path=FontManager::getInstance().getFontFile(family);
        FT_New_Face(ft_library,path.c_str(),0,&ft_face);
        RefPtr<FontFace>face=FtFontFace::create(ft_face,FT_LOAD_FORCE_AUTOHINT);
        fonts[family]=face;
    }
#endif
}

GraphDevice::~GraphDevice(){
#ifndef CAIRO_HAS_FC_FONT
    fonts.clear();
    FT_Done_FreeType(ft_library);
#endif
    //nglDestroySurface(primarySurface);//surface will be destroied by NGLSurface
    //delete primaryContext;delete by desktop window
    NGLOG_DEBUG("%p Destroied",this);
}

#ifndef CAIRO_HAS_FC_FONT
RefPtr<const FontFace>GraphDevice::getFont(const std::string&family){
    RefPtr<const FontFace>face;
    if(fonts.size()){
        face=fonts.begin()->second;//family.empty()?fonts.begin()->second:fonts[family];
    }
    return face;
}
#endif

void GraphDevice::getScreenSize(int &w,int&h){
    w=width;
    h=height;
}

int GraphDevice::getScreenWidth(){
    return width;
}

int GraphDevice::getScreenHeight(){
    return height;
}

void GraphDevice::flip(){
    compose_event++;
}

bool GraphDevice::needCompose(){
    uint64_t tnow64;
    tnow64=steady_clock::now().time_since_epoch().count();
    return compose_event&&(tnow64-last_compose_time)>=30*milliseconds::period::den;
}

GraphContext*GraphDevice::getPrimaryContext(){
    return primaryContext;
}

GraphContext*GraphDevice::createContext(int width,int height){
    HANDLE nglsurface;
    nglCreateSurface(&nglsurface,width,height,format,0);
    GraphContext*graph_ctx=new GraphContext(this,NGLSurface::create(nglsurface));
    NGLOG_VERBOSE("nglsurface=%p  size=%dx%d",nglsurface,width,height);     
    gSurfaces.push_back(graph_ctx);
    graph_ctx->dev=this;
#ifndef CAIRO_HAS_FC_FONT
    graph_ctx->set_font_face(getFont());
#endif
    graph_ctx->screenPos.set(0,0);
    graph_ctx->set_antialias(ANTIALIAS_GRAY);//ANTIALIAS_SUBPIXEL);
    return graph_ctx;
}

GraphContext*GraphDevice::createContext(const RECT&rect){
    GraphContext*ctx=createContext(rect.width,rect.height);
    ctx->screenPos.set(rect.x,rect.y);
    return ctx;
}

void GraphDevice::remove(GraphContext*ctx){
    int x=ctx->screenPos.x;
    int y=ctx->screenPos.y;
    RECT rc={0,0,0,0};
    for(auto itr=gSurfaces.begin();itr!=gSurfaces.end();itr++){
       NGLSurface*s=(NGLSurface*)(*itr)->get_target().get();
       nglGetSurfaceInfo(s->getSurface(),(UINT*)&rc.width,(UINT*)&rc.height,nullptr);
       if((*itr)==ctx){
            gSurfaces.erase(itr);
            invalidate(&rc);
            break;
       }
       (*itr)->invalidate(rc);
    }
    flip();
}

void GraphDevice::invalidate(const RECT*r){
    RECT rect={0,0,width,height};
    if(r)rect.intersect(*r);
    for(auto s:gSurfaces){
        RECT rs;
        NGLSurface*ngs=(NGLSurface*)s->get_target().get();
        nglGetSurfaceInfo(ngs->getSurface(),(UINT*)&rs.width,(UINT*)&rs.height,nullptr);
        rs.x=s->screenPos.x; rs.y=s->screenPos.y;
        if(!rs.intersect(rect))continue;
        rs.offset(-s->screenPos.x,-s->screenPos.y);
        s->invalidate(rs);
    }
    primaryContext->invalidate(rect);
}

void GraphDevice::ComposeSurfaces(){
    int rects=0;
    steady_clock::time_point t2,t1=steady_clock::now();

    std::sort(gSurfaces.begin(),gSurfaces.end(),[](GraphContext*c1,GraphContext*c2){
        return c2->mLayer-c1->mLayer>0;
    });

    for(int i=0;i<primaryContext->mInvalid->get_num_rectangles();i++){
        RectangleInt r=primaryContext->mInvalid->get_rectangle(i);
        nglFillRect(primarySurface,(const NGLRect*)&r,0);
    } 
    primaryContext->mInvalid->do_xor(primaryContext->mInvalid);

    for(auto s=gSurfaces.begin();s!=gSurfaces.end();s++){
        if( (*s)->mInvalid->empty() )
            continue;
        rects+=(*s)->blit2Device(primarySurface);
    }
    nglFlip(primarySurface);
    
    t2=steady_clock::now();//.time_since_epoch()).count();
    duration<double>dur=duration_cast<duration<double>>(t2 - t1);
    NGLOG_VERBOSE("ComposeSurfaces %d surfaces %d rects used %f s",gSurfaces.size(),rects,dur);
    last_compose_time=t2.time_since_epoch().count();
    compose_event=0;
}
}//end namespace
