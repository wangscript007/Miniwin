
project(dvbepg C CXX)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(./ SRCS_EPG)
add_definitions(-DLIBICONV_PLUG)

add_library(dvbepg SHARED  ${SRCS_EPG})
add_library(dvbepg_static STATIC  ${SRCS_EPG})

set_target_properties(dvbepg_static PROPERTIES OUTPUT_NAME "dvbepg")

include_directories(
   ./
   ${CMAKE_BINARY_DIR}/include
   ${CMAKE_BINARY_DIR}/include/porting
   ${CMAKE_BINARY_DIR}/include/epg
   ../
)

target_link_libraries(dvbepg tvhal iconv dl)
target_link_libraries(dvbepg_static tvhal_static iconv dl)

file(GLOB TVHEADERS   ${PROJECT_SOURCE_DIR}/*.h)
file(COPY ${TVHEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include/epg)

add_subdirectory(tests)

install (TARGETS dvbepg  DESTINATION lib)
install (TARGETS dvbepg_static  DESTINATION lib)
