#include<canvas.h>
#include<pixman.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftcache.h>
#include <fontmanager.h>

namespace nglui{

Canvas::Canvas(void*p){
    pixman=p;
    _color=0xFFFFFFFF;
    memset(&font,0,sizeof(LOGFONT));
    font.lfWidth=font.lfHeight=20;
}

void Canvas::select_font(unsigned int weight,int style,const char*family){
    font.lfFaceName[0]=0;
    if(family)strncpy(font.lfFaceName,family,LF_FACESIZE-1);
    font.lfWidth=font.lfHeight=weight;
}

void Canvas::fill_rectangle(int x,int y,unsigned int w,unsigned h){
    pixman_box32_t r={x,y,(int32_t)(x+w),(int32_t)(y+h)};
    pixman_color_t color={(uint16_t)(((_color>>16)&0xFF)<<8),(uint16_t)(((_color>>8)&0xFF)<<8),(uint16_t)((_color&0xFF)<<8),(uint16_t)(_color>>24<<8)};
    pixman_image_fill_boxes(PIXMAN_OP_SRC,(pixman_image_t*)pixman,&color,1,&r);
}

void Canvas::get_text_extents(const char *text,TextExtents*te){
    FT_Face face;
    FontManager::getInstance().getFace(&font,&face);
    
}

void Canvas::get_font_extents(FontExtents*fe){
    FT_Face face;
    FontManager::getInstance().getFace(&font,&face);
    double scale = face->units_per_EM;
    fe->ascent  =  face->ascender / scale;
    fe->descent =  - face->descender / scale;
    fe->height  =  face->height / scale;
    if (0){//!_cairo_ft_scaled_font_is_vertical (&scaled_font->base)) {
        fe->max_x_advance = face->max_advance_width / scale;
        fe->max_y_advance = 0;
    } else {
        fe->max_x_advance = 0;
        fe->max_y_advance = face->max_advance_height / scale;
    } 
}

void Canvas::rectangle(int x,int y,unsigned int w,unsigned h){
    fill_rectangle(x,y,w,1);
    fill_rectangle(x+w,y,1,h);
    fill_rectangle(x,y+h,w,1);
    fill_rectangle(x,y,1,h);
}

void Canvas::draw_text(int x,int y,const char*text_utf8){
    const char*ch=text_utf8;
    unsigned int colors[256];
    unsigned int previous=0;
    unsigned int *pixels=pixman_image_get_data((pixman_image_t*)pixman);
    unsigned stride=pixman_image_get_stride((pixman_image_t*)pixman);
    FT_Face face;
    FontManager::getInstance().getFace(&font,&face);
    for(int i=0;i<256;i++){
         colors[i]=(((_color>>24)*i/255)<<24)|((((_color>>16)&0xFF)*i/255)<<16)|
                ((((_color>>8)&0xFF)*i/255)<<8)|((_color&0xFF)*i/255);
    }
    for(;*ch;ch++){
         FT_Vector  delta;
         FTC_SBit sbit;
         FT_UInt charIdx=FontManager::getInstance().getGraphIndex(face,*ch);
#if 0 
         FT_Load_Glyph(face, charIdx, FT_LOAD_RENDER);
         for(int iy=0;iy<face->glyph->bitmap.rows;iy++){
             unsigned char*src=face->glyph->bitmap.buffer+iy*face->glyph->bitmap.pitch;
             unsigned int *dst=pixels+(stride>>2)*(y+iy-(face->glyph->metrics.horiBearingY>>6)+(face->ascender>>6))+x;
             for(int ix=0;ix<face->glyph->bitmap.width;ix++,src++){
                 dst[ix]=colors[*src];
             }
         }
         FT_Get_Kerning( face, previous, charIdx,FT_KERNING_DEFAULT, &delta );
         x+=(face->glyph->metrics.horiAdvance>>6)+(delta.x>>6);
#else
         FontManager::getInstance().getCharBitmap(&font,charIdx,&sbit);
         for(int iy=0;iy<sbit->height;iy++){
             unsigned char*src=sbit->buffer+iy*sbit->pitch;
             unsigned int *dst=pixels+(stride>>2)*(y+iy-sbit->top+(face->ascender>>6))+x;
             for(int ix=0;ix<sbit->width;ix++,src++){
                 dst[ix]=colors[*src];
             }
         }
         x+=sbit->xadvance;
#endif
         previous=charIdx;
    }
}

}//namespace

