#ifndef __NGL_FRONT_PANNEL_H__
#define __NGL_FRONT_PANNEL_H__
#include<ngl_types.h>

NGL_BEGIN_DECLS

DWORD nglFPInit();
DWORD nglFPShowText(const char*text,int len);
DWORD nglFPSetBrightness(int value);
NGL_END_DECLS
#endif

