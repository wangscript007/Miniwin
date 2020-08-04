#ifndef __NGL_LOG_H__
#define __NGL_LOG_H__

NGL_BEGIN_DECLS

typedef enum{
  NGL_LOG_VERBOSE,
  NGL_LOG_DEBUG,
  NGL_LOG_INFO,
  NGL_LOG_WARN,
  NGL_LOG_ERROR,
  NGL_LOG_ALERT
}NGLLogLevel;

#define NGL_MODULE(x) static const char*__ngl_module__=#x;
/*NGL_MODULE can only be used is c/c++ source file,it cant be used in c/c++ head files*/

void nglLogPrintf(const char*tag,int level,const char*func,int line,const char*format,...);
void nglLogDump(const char*tag,int level,const char*func,int line,const char*label,const BYTE*data,int len);
void nglLogSetModuleLevel(const char*module,int level);
void nglLogParseModule(const char*module);
void nglLogParseModules(int argc,const char*argv[]);

#define NGL_TAG

#ifndef DEBUG
  #define NGLOG_VERBOSE
  #define NGLOG_DEBUG
  #define NGLOG_DUMP
#else
  #define NGLOG_VERBOSE(...) nglLogPrintf(__ngl_module__,NGL_LOG_VERBOSE,__FUNCTION__,__LINE__,__VA_ARGS__)
  #define NGLOG_DEBUG(...)   nglLogPrintf(__ngl_module__,NGL_LOG_DEBUG,__FUNCTION__,__LINE__,__VA_ARGS__)
  #define NGLOG_DUMP(tag,data,len) nglLogDump(__ngl_module__,NGL_LOG_DEBUG,__FUNCTION__,__LINE__,tag,data,len)
#endif

#define NGLOG_INFO(...)    nglLogPrintf(__ngl_module__,NGL_LOG_INFO,__FUNCTION__,__LINE__,__VA_ARGS__)
#define NGLOG_WARN(...)    nglLogPrintf(__ngl_module__,NGL_LOG_WARN,__FUNCTION__,__LINE__,__VA_ARGS__)
#define NGLOG_ERROR(...)   nglLogPrintf(__ngl_module__,NGL_LOG_ERROR,__FUNCTION__,__LINE__,__VA_ARGS__)
#define NGLOG_ALERT(...)   nglLogPrintf(__ngl_module__,NGL_LOG_ALERT,__FUNCTION__,__LINE__,__VA_ARGS__)

#ifndef DEBUG
  #define NGLOG_VERBOSE_IF
  #define NGLOG_DEBUG_IF
#else
  #define NGLOG_VERBOSE_IF(x,...) do{if(x)nglLogPrintf(__ngl_module__,NGL_LOG_VERBOSE,__FUNCTION__,__LINE__,__VA_ARGS__);}while(0)
  #define NGLOG_DEBUG_IF(x,...) do{if(x)nglLogPrintf(__ngl_module__,NGL_LOG_DEBUG,__FUNCTION__,__LINE__,__VA_ARGS__);}while(0)
#endif

#define NGLOG_INFO_IF(x,...) do{if(x)nglLogPrintf(__ngl_module__,NGL_LOG_INFO,__FUNCTION__,__LINE__,__VA_ARGS__);}while(0)
#define NGLOG_WARN_IF(x,...) do{if(x)nglLogPrintf(__ngl_module__,NGL_LOG_WARN,__FUNCTION__,__LINE__,__VA_ARGS__);}while(0)
#define NGLOG_ERROR_IF(x,...) do{if(x)nglLogPrintf(__ngl_module__,NGL_LOG_ERROR,__FUNCTION__,__LINE__,__VA_ARGS__);}while(0)
#define NGLOG_ALERT_IF(x,...) do{if(x)nglLogPrintf(__ngl_module__,NGL_LOG_ALERT,__FUNCTION__,__LINE__,__VA_ARGS__);}while(0)
#define NGASSERT(x) do{if(!x)nglLogPrintf("ASSERT",NGL_LOG_ERROR,__FUNCTION__,__LINE__,"Failed %s",#x);}while(0)

NGL_END_DECLS

#endif
