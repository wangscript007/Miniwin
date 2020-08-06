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

set(HAL_EXTRA_LIBS 
     -Wl,--start-group
     -L${GX_ROOT}/lib 
       -lc_sec  -lcyassl  -ldb  -ldash -lenca -lgxbuscore -lgxcore -lgxdvb -lgxplayer 
       -lgxcaption -liconv 
       #-ljpeg -lpng -lfreetype -lcurl libs of this line comefrom 3rdparty 
       -llz4 -lmpeg2 -ltiff -lui -lwebp -lxml -lxml2 -lz -lzvbi
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

