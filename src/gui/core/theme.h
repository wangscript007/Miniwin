#ifndef __THEME_H__
#define __THEME_H__
#include <ngl_types.h>
namespace nglui{
enum {
   COLOR_BACKGROUND,
   COLOR_SCROLLBAR,
   COLOR_WINDOW,
   COLOR_WINDOW_FRAME,
   COLOR_WINDOW_TEXT,
   COLOR_HIGHLIGHT,
   COLOR_HIGHLIGHTTEXT,
   COLOR_BTNFACE,
   COLOR_BTNTEXT,
   COLOR_BTNSHADOW,
   COLOR_INFOBK,
   COLOR_INFOTEXT,
   COLOR_MAX
};
class Theme{
protected:
   UINT sysColors[COLOR_MAX];
public:
   Theme();
   UINT getSysColor(int idx);
   void setSysColors(int size,INT*eles,UINT *colors);
   void setSysColor(int idx,UINT color);
};

}//namespace

#endif
