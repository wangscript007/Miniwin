
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
message("SRCS_ACS=${SRCS_ACS}")

#set(CMAKE_CXX_STANDARD 11)

link_directories(./)

add_library(acs STATIC ${SRCS_ACS})

add_definitions(-DVAOPT_ENABLE_SCHIP=1)
include_directories(
     ./ include
     ../../include/
     ../porting/inc/
     ${CMAKE_SOURCE_DIR}/include
     ${CMAKE_SOURCE_DIR}/include/curl
     ${CMAKE_SOURCE_DIR}/epg
)

target_link_libraries(acs pthread aui tvhal gui dvbepg curl acs_va m)

#add_executable(acstest main.c )
#stub_main.c )
#target_link_libraries(acstest pthread aui tvhal curl acs acs_va m )
#target_link_libraries(acstest pthread tvhal  acs)

install (TARGETS acs  DESTINATION lib)
#install (TARGETS acs_static  DESTINATION lib)
#install (TARGETS acstest  DESTINATION bin)

