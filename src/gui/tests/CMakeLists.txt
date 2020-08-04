project(gui_test C CXX)

aux_source_directory(./ SRCS_GUI_TESTS)

include_directories(
   ${CMAKE_BINARY_DIR}/include
   ${CMAKE_BINARY_DIR}/include/gui
   ${CMAKE_BINARY_DIR}/include/porting
   ${CMAKE_BINARY_DIR}/include/cairo
   ${CMAKE_BINARY_DIR}/include/epg
   ${CMAKE_SOURCE_DIR}/src/3rdparty/libsvg/src
   ${CMAKE_SOURCE_DIR}/src/3rdparty/libsvg-cairo/src
)

add_executable(gui_test ${SRCS_GUI_TESTS})

target_link_libraries(gui_test gui_static gtest dvbepg_static)

install(TARGETS gui_test DESTINATION bin)

