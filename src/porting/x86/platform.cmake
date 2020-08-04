#set platform include dirs & library path

if ( TRUE )
	set(HAL_EXTRA_LIBS vncserver jrtplib-static CACHE INTERNAL "platform dependence libs")
	set(HAL_EXTRA_LIBS1 vncserver jrtplib-static CACHE INTERNAL "platform dependence libs")
	set(HAL_EXTRA_INCDIRS 
        ${CMAKE_SOURCE_DIR}/src/3rdparty/libvncserver/
        ${CMAKE_BINARY_DIR}/src/3rdparty/libvncserver/
        ${CMAKE_SOURCE_DIR}/src/3rdparty/trfb/src
        ${CMAKE_BINARY_DIR}/include/jrtp
        CACHE INTERNAL "platform dependence include")
set(SOURCES_x86
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
   ngl_mediaplayer.c
   dmxreader.cc
)

else()
  find_package(GTK3)
  find_package(X11)
  if ( X11_FOUND )
	  set(HAL_EXTRA_LIBS ${X11_LIBRARIES} CACHE INTERNAL "platform dependence libs")
	  set(HAL_EXTRA_INCDIRS ${X11_INCLUDE_DIRS} CACHE INTERNAL "platform dependence include")
  elseif ( GTK3_FOUND )
	  set(HAL_EXTRA_LIBS ${GTK3_LIBRARIES} CACHE INTERNAL "platform dependence libs")
	  set(HAL_EXTRA_INCDIRS ${GTK3_INCLUDE_DIRS} CACHE INTERNAL "platform dependence include")
  endif()
endif()

