
project(montage C CXX)


set(SRCS_GX3213
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
)

include(platform.cmake)

if ( NOT RFB_GRAPH )
   set(SRC_GRAPH ngl_gx_graph.c)
endif()

add_library(montage OBJECT ${SRCS_GX3213} ${SRC_GRAPH})
if ( RFB_GRAPH )
   message("RFB_GRAPH is used")
elseif (SRC_GRAPH)
   message("SRC_GRAPH=${SRC_GRAPH}")
else()
   message(FATAL_ERROR "no graph module is selected,please check your enviroment!")
endif()

include_directories(
	./ 
	../inc 
	${HAL_EXTRA_INCDIRS})

#install (TARGETS montage  DESTINATION lib)
