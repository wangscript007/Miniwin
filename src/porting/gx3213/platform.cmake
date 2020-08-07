#set platform include dirs & library path

add_definitions(-DLINUX_OS)

set(GX_ROOT $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux)

set(HAL_EXTRA_LIBS1
     -Wl,--start-group
     -L${GX_ROOT}/share
       c_sec  cyassl  curl db  dash enca freetype gxbuscore gxapi  gxdvb gxplayer 
       gxcaption iconv jpeg lz4 mpeg2 png tiff ui webp xml xml2 z zvbi
       #gxhotplug jansson  qrencode  lzo
     -Wl,--end-group
     CACHE INTERNAL "platform dependence libs")

set(GXCOMMON_LIBS    
       ${GX_ROOT}/lib/libz.a        
       ${GX_ROOT}/lib/libiconv.a
       ${GX_ROOT}/lib/libfreetype.a 
       ${GX_ROOT}/lib/libcurl.a
       ${GX_ROOT}/lib/libjpeg.a      
       ${GX_ROOT}/lib/libpng.a
    )
set(HAL_EXTRA_LIBS 
     -Wl,--start-group
       ${GX_ROOT}/lib/libcyassl.a  
       ${GX_ROOT}/lib/libdash.a 
       ${GX_ROOT}/lib/libenca.a 
       ${GX_ROOT}/lib/libgxbuscore.a 
       ${GX_ROOT}/lib/libgxcore.a 
       ${GX_ROOT}/lib/libgxdvb.a 
       ${GX_ROOT}/lib/libgxplayer.a 
       ${GX_ROOT}/lib/liblz4.a 
       ${GX_ROOT}/lib/libmpeg2.a 
       ${GX_ROOT}/lib/libtiff.a 
       ${GX_ROOT}/lib/libui.a 
       ${GX_ROOT}/lib/libwebp.a 
       ${GX_ROOT}/lib/libxml.a
       ${GX_ROOT}/lib/libxml2.a
       ${GX_ROOT}/lib/libgxcaption.a
       ${GX_ROOT}/lib/libzvbi.a

       zlib iconv freetype curl jpeg png
       #-lz -liconv -ldb -lc_sec -ljpeg -lpng -lfreetype -lcurl 
       #libs of this line comefrom 3rdparty 
     -Wl,--end-group
     CACHE INTERNAL "platform dependence libs")

set(HAL_EXTRA_INCDIRS 
     ${GX_ROOT}/include/
     CACHE INTERNAL "platform dependence include")
#set(HAL_EXTRA_LIBDIRS ${GX_ROOT}/lib CACHE INTERNAL "")

set(SOURCES_GX3213
     ngl_os.c
     ngl_msgq.c
     ngl_dmx.c
     ngl_timer.c
     ngl_dsc.c
     ngl_nvm.c
     ngl_tuner.c
     ngl_smc.c
     ngl_video.c
     ngl_snd.c
     ngl_disp.c
     ngl_disp.c
     ngl_pvr.c
     ngl_ir.cc
     ngl_panel.c
     ngl_misc.c
     ngl_mediaplayer.c
  )

