project(ntvplus C CXX)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(./ SRCS_NTV_PLUS)

include_directories(
   ./
   ./${NGL_CA}/cca_drv/inc
   ./${NGL_CA}/cca_gui/include
   ./${NGL_CA}/cca_mw/inc
   ./${NGL_CA}/cca_mw/inc/decoder_msg
   ${CMAKE_BINARY_DIR}/include
   ${CMAKE_BINARY_DIR}/include/gui
   ${CMAKE_BINARY_DIR}/include/porting
   ${CMAKE_BINARY_DIR}/include/cairo
   ${CMAKE_BINARY_DIR}/include/epg
   ${CMAKE_SOURCE_DIR}/src/modules/upnp
)

if ( NGL_CA )
    add_subdirectory(${NGL_CA})
    string(TOUPPER ${NGL_CA} CA_DEF)
    add_definitions(-DHAVE_${CA_DEF} -DHAVE_CA)
    link_directories(${NGL_CALIBDIR})
    if(NOT NGL_CALIBS)
      set(NGL_CALIBS ${NGL_CA})
    endif()
    message("========NGL_CALIBS=${NGL_CALIBS} NGL_CA=${NGL_CA} NGL_CALIBDIR=${NGL_CALIBDIR} HAL_EXTRA_LIBS=${HAL_EXTRA_LIBS}")
endif()

add_executable(ntvplus ${SRCS_NTV_PLUS} )
add_executable(ntvplusd ${SRCS_NTV_PLUS} )

target_link_libraries(ntvplus jsoncpp_lib gui_static dvbepg_static ${APP_EXTLIBS} ${NGL_CALIBS})
target_link_libraries(ntvplusd pthread jsoncpp_lib gui dvbepg ${APP_EXTLIBS}  ${NGL_CALIBS})

add_custom_command (TARGET ntvplus PRE_BUILD
  COMMAND zip -r -0  ${CMAKE_CURRENT_BINARY_DIR}/ntvplus.pak *
  BYPRODUCTS ntvplus
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/assets
  DEPENDS zip
  COMMENT "package assets resource"
  )

install (TARGETS ntvplus  DESTINATION bin)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/ntvplus.pak  DESTINATION bin)
