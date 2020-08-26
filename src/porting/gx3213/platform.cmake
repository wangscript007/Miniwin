#set platform include dirs & library path

add_definitions(-DLINUX_OS)

set(GX_ROOT $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux)
set(GX_SROOT $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/lib)
set(GX_DROOT $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux/lib/share)

set(GXCOMMON_DLIBS
       ${GX_DROOT}/libjpeg.so 
       ${GX_DROOT}/libpng.so
       ${GX_DROOT}/libiconv.so
       ${GX_DROOT}/libfreetype.so
       ${GX_DROOT}/libcurl.so 
   )

#dynamic linked libraries
set(HAL_EXTRA_DLIBS
     -Wl,--start-group
#      ${GX_DROOT}/libc_sec.so
       ${GX_DROOT}/libcyassl.so
       ${GX_DROOT}/libdb.so  
       ${GX_DROOT}/libdash.so
       ${GX_DROOT}/libenca.so
       ${GX_DROOT}/libgxbuscore.so
       ${GX_DROOT}/libgxapi.so
       ${GX_DROOT}/libgxdvb.so 
       ${GX_DROOT}/libgxplayer.so
       ${GX_DROOT}/libgxcaption.so
       ${GX_DROOT}/liblz4.so 
       ${GX_DROOT}/libmpeg2.so
       ${GX_DROOT}/libtiff.so
       ${GX_DROOT}/libui.so
       ${GX_DROOT}/libwebp.so
       ${GX_DROOT}/libxml.so 
       ${GX_DROOT}/libxml2.so 
       
       png jpeg iconv freetype curl zvbi
       #gxhotplug jansson  qrencode  lzo zlib
     -Wl,--end-group
     CACHE INTERNAL "platform dependence libs")

set(GXCOMMON_SLIBS    
       ${GX_SROOT}/libz.a        
       ${GX_SROOT}/libiconv.a
       ${GX_SROOT}/libfreetype.a 
       ${GX_SROOT}/libcurl.a
       ${GX_SROOT}/libjpeg.a      
       ${GX_SROOT}/libpng.a
    )

#static linked libraries
set(HAL_EXTRA_SLIBS 
     -Wl,--start-group
       ${GX_SROOT}/libcyassl.a  
       ${GX_SROOT}/libdash.a 
       ${GX_SROOT}/libenca.a 
       ${GX_SROOT}/libgxbuscore.a 
       ${GX_SROOT}/libgxcore.a 
       ${GX_SROOT}/libgxdvb.a 
       ${GX_SROOT}/libgxplayer.a 
       ${GX_SROOT}/liblz4.a 
       ${GX_SROOT}/libmpeg2.a 
       ${GX_SROOT}/libtiff.a 
       ${GX_SROOT}/libui.a 
       ${GX_SROOT}/libwebp.a 
       ${GX_SROOT}/libxml.a
       ${GX_SROOT}/libxml2.a
       ${GX_SROOT}/libgxcaption.a

       zlib iconv freetype curl jpeg png zvbi
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
file(GLOB EXTRA_INSTALL_LIBS  ${GX_DROOT}/*)
file(COPY ${EXTRA_INSTALL_LIBS} DESTINATION ${CMAKE_BINARY_DIR}/lib)
