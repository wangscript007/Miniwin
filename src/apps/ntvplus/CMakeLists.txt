project(ntvplus C CXX)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(./ SRCS_NTV_PLUS)

include_directories(
   ./
   ./${NGL_CA}/include
   ${CMAKE_BINARY_DIR}/include
   ${CMAKE_BINARY_DIR}/include/gui
   ${CMAKE_BINARY_DIR}/include/porting
   ${CMAKE_BINARY_DIR}/include/cairo
   ${CMAKE_BINARY_DIR}/include/epg
   ${CMAKE_SOURCE_DIR}/src/modules/upnp
)

if ( NGL_CA )
    add_subdirectory(${NGL_CA})
    link_directories(${NGL_CA})
    string(TOUPPER ${NGL_CA} CA_DEF)
    add_definitions(-DHAVE_${CA_DEF})
endif()

add_executable(ntvplus ${SRCS_NTV_PLUS} )
add_executable(ntvplusd ${SRCS_NTV_PLUS} )

target_link_libraries(ntvplus jsoncpp_lib gui_static dvbepg_static ${APP_EXTLIBS} ${NGL_CA})
target_link_libraries(ntvplusd pthread jsoncpp_lib gui dvbepg ${APP_EXTLIBS}  ${NGL_CA})

add_custom_command (TARGET ntvplus PRE_BUILD
  COMMAND zip -r  ${CMAKE_CURRENT_BINARY_DIR}/ntvplus.pak *
  BYPRODUCTS ntvplus
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/assets
  DEPENDS zip
  COMMENT "package assets resource"
  )

install (TARGETS ntvplus  DESTINATION bin)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/ntvplus.pak  DESTINATION bin)
