#ifndef __BASIC_TYPE_H__
#define __BASIC_TYPE_H__
#include<ngl_errors.h>

#ifdef  __cplusplus
#define NGL_BEGIN_DECLS extern "C" {
#define NGL_END_DECLS }
#else
#define NGL_BEGIN_DECLS
#define NGL_END_DECLS
#endif

#ifndef __VOID
#define __VOID
typedef void VOID;
#endif

#ifndef __HANDLE
#define __HANDLE
typedef void* HANDLE;
#endif

#ifndef __WORD
#define __WORD
typedef unsigned short WORD;
#endif

#ifndef __DWORD
#define __DWORD
typedef unsigned long DWORD;
#endif

#ifndef __LONG
#define __LONG
typedef long LONG;
#endif

#ifndef __ULONG
#define __ULONG
typedef unsigned long ULONG;
#endif

#ifndef __LONGLONG
#define __LONGLONG
typedef long long LONGLONG;
#endif

#ifndef __ULONGLONG
#define __ULONGLONG
typedef unsigned long long ULONGLONG;
#endif

#ifndef __BYTE
#define __BYTE
typedef unsigned char BYTE;
#endif

#ifndef __CHAR
#define __CHAR
typedef char CHAR;
#endif

#ifndef __INT8
#define __INT8
typedef char INT8;
#endif

#ifndef __UINT8
#define __UINT8
typedef unsigned char UINT8;
#endif
 
#ifndef __SHORT
#define __SHORT
typedef short SHORT;
#endif

#ifndef __USHORT
#define __USHORT
typedef unsigned short USHORT;
#endif

#ifndef __INT
#define __INT
typedef int INT;
#endif

#ifndef __INT32
#define __INT32
typedef long INT32;
#endif


#ifndef __UINT16
#define __UINT16
typedef unsigned short UINT16;
#endif

#ifndef __UINT
#define __UINT
typedef unsigned int UINT;
#endif

#ifndef __UINT32
#define __UINT32 
typedef unsigned long UINT32;
#define __UINT32
#endif


#ifndef __BOOL 
typedef int BOOL;
#define FALSE 0
#define TRUE 1
#define __BOOL
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif


#endif
