project(subttx C CXX)

set(CMAKE_CXX_STANDARD 11)
set( SRCS_CCVBI vbiview.cc teletextview.cc ccview.cc dvbsub.c subtitleview.cc)

include_directories(
    ./
    ${CMAKE_SOURCE_DIR}/src/3rdparty/zvbi-0.2.35/src
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/include/epg
    ${CMAKE_BINARY_DIR}/include/pixman
    ${CMAKE_BINARY_DIR}/include/porting
    ${CMAKE_BINARY_DIR}/include/cairo
    ${CMAKE_BINARY_DIR}/include/gui
    )

add_library(subttx STATIC ${SRCS_CCVBI})
target_link_libraries(subttx zvbi tvhal gui)

add_executable(subttxtest main.cc tvtestutils.cc)
target_link_libraries(subttxtest gui dvbepg subttx)

