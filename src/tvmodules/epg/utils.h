#ifndef __DVB_UTILS_H__
#define __DVB_UTILS_H__
#include <ngl_types.h>
#include <ngl_timer.h>

NGL_BEGIN_DECLS

INT ToUtf8(const char*buf,int len,char*utf);
void MJD2YMD(INT mjd,INT*y,INT*m,INT*d);
void UTC2Tm(INT mjd,INT utc,NGL_TM*tm);
void UTC2Time(INT mjd,INT utc,NGL_TIME*t);
INT Hex2BCD(INT x);
UINT GetCRC32(const BYTE*buf,size_t size);
NGL_END_DECLS

#endif

