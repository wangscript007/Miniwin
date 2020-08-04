#ifndef __NGL_MSGQ_H__
#define __NGL_MSGQ_H__
#include <ngl_types.h>

NGL_BEGIN_DECLS

HANDLE nglMsgQCreate(int howmany, int sizepermag);
DWORD nglMsgQDestroy(HANDLE msgq);
DWORD nglMsgQSend(HANDLE msgq, const void* pvmag, int msgsize, DWORD timeout);
DWORD nglMsgQReceive(HANDLE msgq, const void* pvmag, DWORD msgsize, DWORD timeout);
DWORD nglMsgQGetCount(HANDLE msgq,UINT*count);
NGL_END_DECLS

#endif
