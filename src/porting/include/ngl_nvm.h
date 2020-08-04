#ifndef __NGL_NVM_H__
#define __NGL_NVM_H__
#include<ngl_types.h>

NGL_BEGIN_DECLS

DWORD NGLOpenFlash(DWORD *handle);
DWORD NGLCloseFlash(DWORD handle);
DWORD NGLWriteFlash(DWORD handle,DWORD dwStartAddr, BYTE *pData, DWORD lDataLen);
DWORD NGLReadFlash(DWORD handle,DWORD dwStartAddr, BYTE *pData, DWORD lDataLen);

NGL_END_DECLS

#endif
