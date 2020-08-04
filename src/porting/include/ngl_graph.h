#ifndef __NGL_GRAPH_H__
#define __NGL_GRAPH_H__
#include <ngl_types.h>

/**
 @ingroup std_drivers
 */

/**
 @defgroup graphDriver GRAPH API
 @brief This section describes the interface for Graph interface.

 @{
*/


NGL_BEGIN_DECLS
/**
 @defgroup graphFormat PIXEL Format
 @brief This enum list the surface format .
 @{ */
typedef enum {
  GPF_UNKNOWN,
  GPF_ARGB4444,
  GPF_ARGB1555,
  GPF_RGB565,
  GPF_ARGB,
  GPF_ABGR,
  GPF_RGB32
}NGLPIXELFORMAT;
/** @} */

/**
 @defgroup graphStruct Structs
 @brief .
 @{ */

typedef struct _NGLRect{
    INT x;
    INT y;
    UINT w;
    UINT h;
}NGLRect;
/**}*/

/**
 @defgroup graphfunctions Graph Funtions
 @brief .
 @{ */


/*This function init the graph dirver .

/**
    @retval NGL_OK                            If init success.
    @retval NGL_ERROR                         If init failed.

    For more information refer to @ref nglCreateSurface.*/

DWORD nglGraphInit();
/**This function get the graph resolution 
    @param [out]width                         The value return screen width in pixels.
    @param [out]height                        The value return screen height in pixels.
    @retval NGL_OK                            
    @retval NGL_ERROR
     For more information refer to @ref nglCreateSurface and @ref nglGetSurfaceInfo.
*/
DWORD nglGetScreenSize(UINT*width,UINT*height);

/**This function create an OSD Surface which we can used to draw sth.
    @param [out]surface                      The value used to return surface handle.
    @param [in]width                         The value give the surface width in pixels.
    @param [in]height                        The value give the surface height in pixels.
    @param [in]format                        surface format @ref NGLPIXELFORMAT
    @param [in]hwsurface                     
    @retval NGL_OK
    @retval NGL_ERROR
    For more information refer to @ref nglCreateSurface and @ref nglGetSurfaceInfo.
*/

DWORD nglCreateSurface(HANDLE*surface,UINT width,UINT height,INT format,BOOL hwsurface);

/**This function create an OSD Surface which we can used to draw sth.
    @param [in]surface                       The surface handle which is created by @ref nglCreateSurface.
    @param [out]width                         The value return screen width in pixels.
    @param [out]height                        The value return screen height in pixels.
    @param [out]format                        The value used to return surface pixel's format @ref NGLPIXELFORMAT
    @retval NGL_OK
    @retval NGL_ERROR
    For more information refer to @ref nglCreateSurface and @ref nglGetSurfaceInfo.
*/

DWORD nglGetSurfaceInfo(HANDLE surface,UINT*width,UINT*height,INT *format);
DWORD nglLockSurface(HANDLE surface,void**buffer,UINT*pitch);
DWORD nglUnlockSurface(HANDLE surface);
DWORD nglSurfaceSetOpacity(HANDLE surface,BYTE alpha);
/**Thie function fill the surface area with color 
  @param [in]surface         
  @param [in]rect            if rect is NULL fill whole surface area
  @param [in]color           color with format as A8R8G8B8
  @retval NGL_OK
  @retval NGL_ERROR
*/
DWORD nglFillRect(HANDLE dstsurface,const NGLRect*rect,UINT color);

/**This function Blit source surface to dest surface .
    @param [in]dstsurface                     The dest surface which used to blit to.
    @param [in]dx                             The position x which source surface blit to
    @param [in]dy                             The position y which source surface blit to
    @param [in]srcsurface                     The source surface 
    @param [in]srcrect                        The rectange(area) in source surface we want to blit 
    @retval NGL_OK
    @retval NGL_ERROR
    For more information refer to @ref nglCreateSurface .
*/
DWORD nglBlit(HANDLE dstsurface,int dx,int dy,HANDLE srcsurface,const NGLRect*srcrect);
DWORD nglFlip(HANDLE dstsurface);

/**This functionDestroy the surface
    @param [in]dstsurface                     The dest surface we want to destroied
    @retval NGL_OK
    @retval NGL_ERROR
    For more information refer to @ref nglCreateSurface
*/
DWORD nglDestroySurface(HANDLE surface);

/**}*///raphfunctions

NGL_END_DECLS
#endif

