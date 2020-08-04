#ifndef __CANVAS_H__
#define __CANVAS_H__
#include<ngl_types.h>

namespace nglui{

#define LF_FACESIZE 32

typedef struct {
    double ascent;
    double descent;
    double height;
    double max_x_advance;
    double max_y_advance;
}FontExtents;

typedef struct {
    double x_bearing;
    double y_bearing;
    double width;
    double height;
    double x_advance;
    double y_advance;
} TextExtents;

class Canvas{
private:

typedef struct tagLOGFONT {
  LONG lfHeight;
  LONG lfWidth;
  LONG lfEscapement;
  LONG lfOrientation;
  LONG lfWeight;
  BYTE lfItalic;
  BYTE lfUnderline;
  BYTE lfStrikeOut;
  BYTE lfCharSet;
  BYTE lfOutPrecision;
  BYTE lfClipPrecision;
  BYTE lfQuality;
  BYTE lfPitchAndFamily;
  CHAR lfFaceName[LF_FACESIZE];
} LOGFONT;

protected:
   void*pixman;
   LOGFONT font; 
   unsigned int _color;
   unsigned int getGlyphIndex(unsigned int ch);
   friend class FontManager;
public:
   Canvas(void*p);
   void set_color(unsigned int c){_color=c;}
   void select_font(unsigned int size,int style=0,const char*family=nullptr);
   void get_text_extents(const char *text,TextExtents*te);
   void get_font_extents(FontExtents*fe);
   void fill_rectangle(int x,int y,unsigned int w,unsigned int h);
   void rectangle(int x,int y,unsigned int w,unsigned int h);
   void draw_text(int x,int y,const char*text_utf8);
};

}//namespace
#endif
