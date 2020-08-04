#include <ngl_types.h>
#include <graph_context.h>
#include <cairo.h>
#include <cairo-ngl.h>
#include <cairomm/ngl_surface.h>
#include <ngl_log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wordbreak.h>
#include <linebreak.h>
#include <windowmanager.h>

using namespace std;

NGL_MODULE(GraphContext);
namespace nglui{

GraphContext::GraphContext(GraphDevice*_dev,HANDLE surface)
   :GraphContext(_dev,NGLSurface::create(surface,false)){
   screenPos.set(0,0);
   parent_ctx=nullptr;
}

GraphContext::GraphContext(GraphDevice*_dev,const RefPtr<Surface>& target)
   :Context(target){
    dev=_dev;
    parent_ctx=nullptr;
    screenPos.set(0,0);
    mInvalid=Region::create();
}

GraphContext*GraphContext::subContext(int x,int y,int w,int h){
    auto suf=Surface::create(get_target(),x,y,w,h);
    auto ctx=new GraphContext(dev,suf);
    ctx->parent_ctx=this;
    ctx->screenPos.set(x,y);
    NGLOG_VERBOSE("subContext (%d,%d-%d,%d) screenpos=%d,%d",x,y,w,h,x+screenPos.x,screenPos.y);
    return ctx;
}

void GraphContext::setLayer(int l){
    if(dev&&mLayer!=l){
        mLayer=l;
    }
    mInvalid->subtract(mInvalid);
}

void GraphContext::invalidate(const RECT&r){
    RECT rect=r;
    switch(get_target()->get_type()){
    case Surface::Type::NGL:NGLOG_VERBOSE("surface=%p parent=%p ",this,parent_ctx);break;
    case Surface::Type::IMAGE:NGLOG_VERBOSE("IMAGE SURFACE %p",this);break;
    case Surface::Type::SUBSURFACE:NGLOG_VERBOSE("SUBSURFACE %p",this);break;
    }
    if(parent_ctx){
        RECT rc={0,0,0,0};
        NGLSurface*s=(NGLSurface*)parent_ctx->get_target().get();
        nglGetSurfaceInfo(s->getSurface(),(UINT*)&rc.width,(UINT*)&rc.height,nullptr);
        rect.offset(screenPos.x,screenPos.y);//the rect perhaps outof screen,must clipped by screen rectangle
        if(rect.intersect(rc))
           parent_ctx->mInvalid->do_union((RectangleInt&)rect);
    }else
        mInvalid->do_union((RectangleInt&)rect);
    NGLOG_VERBOSE("%p, %d,%d-%d,%d",this,rect.x,rect.y,rect.width,rect.height);
    dev->flip();
}

void GraphContext::invalidate(const RefPtr<Region>crgn){
    if(parent_ctx){
        RefPtr<Region>rgn=crgn->copy();
        rgn->translate(screenPos.x,screenPos.y);
        parent_ctx->mInvalid->do_union(rgn);
    }else
        mInvalid->do_union(crgn);
    dev->flip();
}

int GraphContext::blit2Device(HANDLE surface){
    HANDLE nglsurface=cairo_ngl_surface_get_surface(get_target()->cobj());
    RefPtr<Region>visrgn=WindowManager::getInstance().getVisibleRegion(mLayer);
    if(visrgn)mInvalid->intersect(visrgn);
    int num=mInvalid->get_num_rectangles();
    for(int i=0;i<num;i++){
        RectangleInt r=mInvalid->get_rectangle(i);
        NGLOG_VERBOSE("   blit %p %d,%d-%d,%d-->%d,%d",this,r.x,r.y,r.width,r.height,screenPos.x,screenPos.y);
        nglBlit(surface,screenPos.x+r.x,screenPos.y+r.y,nglsurface,(NGLRect*)&r);
    }
    mInvalid->subtract(mInvalid);
    return num;
}

GraphContext::~GraphContext(){
    NGLOG_VERBOSE("%p  type=%d parent_ctx=%p",this,get_target()->get_type(),parent_ctx);
    //if(parent_ctx==nullptr)
    dev->remove(this);
}

void GraphContext::set_position(int x,int y){
    RECT rc,rc1;
    RefPtr<Region>rgn;
    NGLSurface*s=(NGLSurface*)get_target().get();
    nglGetSurfaceInfo(s->getSurface(),(UINT*)&rc.width,(UINT*)&rc.height,nullptr);
    rc.x=screenPos.x;
    rc.y=screenPos.y;
    rgn=Region::create((RectangleInt&)rc);
    rc1=rc;
    rc1.offset(x-screenPos.x,y-screenPos.y);
    rgn->subtract((RectangleInt&)rc1);
    screenPos.x=x;
    screenPos.y=y;
    for(int i=0;i<rgn->get_num_rectangles();i++){
        RectangleInt r=rgn->get_rectangle(i);
        dev->invalidate((const RECT*)&r);
    }
    dev->flip();
}

void GraphContext::set_color(UINT color){
    set_color((color>>16)&0xFF,(color>>8)&0xFF,color&0xFF,(color>>24));
}

void GraphContext::set_color(BYTE r,BYTE g,BYTE b,BYTE a){
    set_source_rgba(r/255.f,g/255.f,b/255.f,a/255.f);
}

void GraphContext::rectangle(int x,int y,int w,int h){
    Context::rectangle((double)x,(double)y,(double)w,(double)h);
}

void GraphContext::rectangle(const RECT &r){
    rectangle((double)r.x,(double)r.y,(double)r.width,(double)r.height);
}

void GraphContext::draw_arraw(const RECT&r,int dir){
    POINT pt[3]={};
    switch(dir){
    case 0://up
         pt[0].y=pt[1].y=r.y+r.width/2;
         pt[0].x=r.x;pt[1].x=r.x+r.width;
         pt[2].x=r.x+r.width/2;pt[2].y=r.y;
         break;
    case 1://down
         pt[0].y=pt[1].y=r.y+r.height-r.width/2;
         pt[0].x=r.x;pt[1].x=r.x+r.width;
         pt[2].x=r.x+r.width/2;pt[2].y=r.y+r.height;
         break;
    case 2://left
         pt[0].x=pt[1].x=r.x+r.height/2;
         pt[0].y=r.y;pt[1].y=r.y+r.height;
         pt[2].x=r.x;
         pt[2].y=r.y+r.height/2;
         break;
    case 3://right
         pt[0].x=pt[1].x=r.x+r.width-r.height/2;
         pt[0].y=r.y;pt[1].y=r.y+r.height;
         pt[2].x=r.x+r.width;pt[2].y=r.y+r.height/2;
         break;
    default:return;
    }
    move_to(pt[0].x,pt[0].y);
    line_to(pt[1].x,pt[1].y);
    line_to(pt[2].x,pt[2].y);
    line_to(pt[0].x,pt[0].y);
    fill();
}

void GraphContext::roundrect(int x,int y,int width,int height,int radius){
    if(0==radius)
        Context::rectangle(x,y,width,height);
    else{
        double from_degre = M_PI/180;
        arc(x+width-radius, y+height-radius, radius, 0*from_degre, 90*from_degre);
        arc(x+radius, y+height-radius, radius, 90*from_degre, 180*from_degre);
        arc(x+radius, y+radius, radius, 180*from_degre, 270*from_degre);
        arc(x+width-radius, y+radius, radius, 270*from_degre, 360*from_degre);
        close_path();
    }
}

void GraphContext::get_text_size(const std::string&text,int*width,int *height){
    TextExtents te;
    get_text_extents(text,te);
    if(width)*width=te.width;
    if(height)*height=te.height;
}

void GraphContext::draw_text(const RECT&rect,const std::string&text,int text_alignment){
     char*brks=(char*)malloc(text.length()) ;//WORDBREAK_BREAK, #WORDBREAK_NOBREAK,#WORDBREAK_INSIDEACHAR
     set_wordbreaks_utf8((const utf8_t*)text.c_str(),text.length(),"eng",brks);
     const char*ptxt=text.c_str();
     const char*pword=ptxt;
     TextExtents extents,te;
     FontExtents ftext;
     std::vector<std::string>lines;
     std::string line;
     double x,y;
     double total_width=0, total_height=0;
     get_font_extents(ftext);
     //NGLOG_VERBOSE("ascent=%.3f descent=%.3f height=%.3f xy_advance=%.3f/%.3f",ftext.ascent,ftext.descent,ftext.height,ftext.max_x_advance,ftext.max_x_advance);
     for(int i=0; i<text.length();i++){
         switch(brks[i]){
         case WORDBREAK_BREAK:{
                 std::string word(pword,ptxt+i-pword+1);
                 get_text_extents(word,extents);

                 if( ((total_width+extents.width >= rect.width) || (text[i]=='\n')) && (text_alignment&DT_MULTILINE) ){
                     lines.push_back(line);
                     total_height+=extents.height;
                     line="";total_width=0;
                     size_t ps=word.find('\n');
                     if(ps!=std::string::npos)word.erase(ps,1);
                 }
                 total_width+=extents.width;
                 line.append(word);
                 pword=ptxt+i+1;
             }
             break;
        case WORDBREAK_NOBREAK:    break;
        case WORDBREAK_INSIDEACHAR:break;
        default:break;
        }
    }
    free(brks);
    total_height+=extents.height;
    lines.push_back(line);
    total_height=lines.size()*ftext.height;
    
    if((text_alignment&DT_MULTILINE)==0){
        get_text_extents(lines[0],te);
        y=rect.y;
        switch(text_alignment&0xF0){
        case DT_TOP:y=rect.y+ftext.ascent-ftext.descent;break;
        case DT_VCENTER:y=rect.y+rect.height/2+(ftext.ascent-ftext.descent)/2;break;
        case DT_BOTTOM:y=rect.y+rect.height;break;
        }
        switch(text_alignment&0x0F){
        case DT_LEFT:x=rect.x;break;
        case DT_CENTER:x=rect.x+(rect.width-te.x_advance)/2;break;
        case DT_RIGHT:x=rect.x+rect.width-te.x_advance;break;
        }
        move_to(x+te.x_bearing,y);
        show_text(text);
        //NGLOG_VERBOSE("%s Rect(%d,%d,%d,%d) drawpos=%.3f,%.3f width=%.3f height=%.3f advance=%.3f/%.3f bearing=%.3f/%.3f  alignment=0x%02x",text.c_str(),
        //   rect.x,rect.y,rect.width,rect.height,x+te.x_bearing,y,te.width,te.height,te.x_advance,te.y_advance,te.x_bearing,te.y_bearing,text_alignment);
    }else {
        y=rect.y;
        switch(text_alignment&0xF0){
        case DT_TOP:y=rect.y+ftext.descent;break;
        case DT_VCENTER:y=rect.y+(rect.height-total_height)/2+ftext.descent;break;
        case DT_BOTTOM:y=rect.y+rect.height-total_height+ftext.descent;break;
        }
        for(auto line:lines){
            get_text_extents(line,te);
            switch(text_alignment&0x0F){
            case DT_LEFT:x=rect.x;break;
            case DT_CENTER:x=rect.x+(rect.width-te.x_advance)/2;break;
            case DT_RIGHT:x=rect.x+rect.width-te.x_advance;break;
            }
            move_to(x+te.x_bearing,y-te.y_bearing);
            y+=ftext.height;
            show_text(line);
        }
    }
}

void GraphContext::draw_text(int x,int y,const std::string& text){
    FontExtents fe;
    get_font_extents(fe);
    move_to(x,y);//-fe.descent);
    show_text(text);
}

void GraphContext::draw_image(const RefPtr<ImageSurface>&img,int x,int y){
    if(img){
       set_source(img,x,y);
       rectangle(x,y,img->get_width(),img->get_height());
       fill();
    }
}

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
void GraphContext::draw_image(const RefPtr<ImageSurface>&img,const RECT*dst,const RECT*src,SCALETYPE st){
    const char*scaleNames[]={"CENTER","CENTER_CROP","CENTER_INSIDE","MATRIX","FIT_CENTER","FIT_START","FIT_END","FIT_XY"};
    RECT rs;
    float scale,scalex,scaley;
    int x=0,y=0;
    if(img)
        rs.set(0,0,img->get_width(),img->get_height());
    else 
        return;
    if(src)rs=*src;
    scalex=(float)rs.width/dst->width;
    scaley=(float)rs.height/dst->height; 
    switch(st){
    case ST_CENTER://ok     
          x=(rs.width>dst->width)?dst->x-(rs.width-dst->width)/2:(dst->x+(dst->width-rs.width)*.5f);
          y=(rs.height>dst->height)?dst->y-(rs.height-dst->height)/2:(dst->x+(dst->height-rs.height)*.5f);
          set_source(img,x,y);
          break;
    case ST_CENTER_CROP://ok
          scale =MIN(scalex,scaley);//(float) rs.height / (float) dst->height;
          if(rs.width>=dst->width||rs.height>=dst->height){
             x=dst->x- (rs.width/scale - dst->width)*.5f;
             y=dst->y-(rs.height/scale - dst->height)*.5f;
          }
          set_source(img,x,y);
          img->set_device_scale(scale,scale);//cairo_surface_scale
          break;
    case ST_CENTER_INSIDE://ok
          scale =MAX(scalex,scaley);//(float) rs.height / (float) dst->height;
          if(rs.width>=dst->width||rs.height>=dst->height){
             x=dst->x-(rs.width/scale - dst->width)*.5f;
             y=dst->y-(rs.height/scale - dst->height)*.5f;
          }else{
             x=dst->x+(dst->width - rs.width)*.5f;
             y=dst->y+(dst->height - rs.height)*.5f;
          }
          set_source(img,x,y);//dst->x+(dst->width-rs.width)/2,dst->y+(dst->width-rs.height)/2);
          if(rs.width>dst->width||rs.height>dst->height)
              img->set_device_scale(scale,scale);
          break;
    case ST_MATRIX://ok
          set_source(img,dst->x,dst->y);
          break;
    case ST_FIT_CENTER:
          scale=MAX(scalex,scaley);
          x=dst->x- (rs.width/scale - dst->width)*.5f;
          y=dst->y-(rs.height/scale - dst->height)*.5f;
          set_source(img,x,y);//dst->x-(rs.width-dst->width)/2,dst->y-(rs.width-dst->height)/2);
          img->set_device_scale(scale,scale);
          break;
    case ST_FIT_START:
          scale=MAX(scalex,scaley);
          x=dst->x- (rs.width/scale - dst->width)*.5f;
          y=dst->y;
          set_source(img,x,y);
          img->set_device_scale(scale,scale);
          break;
    case ST_FIT_END:
          scale=MAX(scalex,scaley);
          x=dst->x- (rs.width/scale - dst->width)*.5f;
          y=dst->y+(dst->height-rs.height/scale);
          set_source(img,x,y);
          img->set_device_scale(scale,scale);
          break;
    case ST_FIT_XY://ok
          set_source(img,dst->x,dst->y);
          img->set_device_scale(scalex,scaley);
          break;
    }
    NGLOG_VERBOSE("ScaleType:%s x=%d y=%d / dstrect.size=%dx%d imgsize=%d,%d  scale=%f/%f/%f",scaleNames[st],
         x,y,dst->width,dst->height,rs.width,rs.height,scale,scalex,scaley);
    /*cairo_set_source_surface(cr,img->surface,dst->x,dst->y);
    cairo_surface_scale(img->surface,rs.width/dst->width,rs.height/dst->height);*/
    rectangle(dst->x,dst->y,dst->width,dst->height);
    fill();

}

void GraphContext::dump2png(const char*fname){
#ifdef CAIRO_HAS_PNG_FUNCTIONS
    get_target()->write_to_png(fname);
#else
    NGLOG_WARN("PNG NOT support!");
#endif
}

void DumpRegion(const std::string&label,RefPtr<Region>rgn){
    for(int i=0;i<rgn->get_num_rectangles();i++){
       RectangleInt rr=rgn->get_rectangle(i);
       NGLOG_VERBOSE("%s[%d]=%d,%d-%d,%d)",label.c_str(),i,rr.x,rr.y,rr.width,rr.height);
    }
}

}//namespace
