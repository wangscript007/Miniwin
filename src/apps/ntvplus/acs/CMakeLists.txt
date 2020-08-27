
project(acs  C CXX)

set(SRCS_ACS
   src/va_ctrl.cc  src/va_fs.c    src/va_net.c   src/va_pvr.cc    src/va_vse.c
   src/va_dmx.c   src/va_init.c  src/va_os.c    src/va_sc.c     src/va_xnvm.c
   src/va_prot.c  src/va_http.c
   src/va_dscr.c   src/va_schip.c
   src/va_crypt.c  ./va2ngl.cc 
#  Stubs/Src/va_dscr.c   Stubs/Src/va_schip.c
)
list(REMOVE_ITEM SRCS_ACS ./Stubs/Src/stub_main.c ./Stubs/Src/dmxtest.c)

#set(CMAKE_CXX_STANDARD 11)

link_directories(./)
set(NGL_CALIBDIR ${PROJECT_SOURCE_DIR} PARENT_SCOPE)
add_library(acs STATIC ${SRCS_ACS})

add_definitions(-DVAOPT_ENABLE_SCHIP=1)
include_directories(
     ./  ./include
     ${CMAKE_BINARY_DIR}/include/
     ${CMAKE_BINARY_DIR}/include/porting
     ${CMAKE_BINARY_DIR}/include/curl
     ${CMAKE_BINARY_DIR}/epg
)

target_link_libraries(acs pthread aui curl acs_va m)
set(NGL_CALIBS
   -Wl,--start-group
   acs_va acs
   -Wl,--end-group
   ${HAL_EXTRA_LIBS} PARENT_SCOPE
)


install (TARGETS acs  DESTINATION lib)

