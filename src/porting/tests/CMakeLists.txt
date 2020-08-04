#cmake_minimum_required (VERSION 3.8)
project(haltest C CXX)

aux_source_directory(./ SRCS_HAL_TESTS)
include_directories(
   ./
   ${CMAKE_SOURCE_DIR}/gtest
   ${CMAKE_BINARY_DIR}/include
   ${CMAKE_BINARY_DIR}/include/porting
   ${CMAKE_SOURCE_DIR}/src/3rdparty/libvncserver
   ${CMAKE_BINARY_DIR}/src/3rdparty/libvncserver
   ${CMAKE_BINARY_DIR}/include/pixman
   ${CMAKE_BINARY_DIR}/include/freetype2
)

message("HAL_EXTRA_LIBS=${HAL_EXTRA_LIBS}")
link_directories(${CMAKE_SOURCE_DIR}/gtest)

add_executable(hal_tests ${SRCS_HAL_TESTS}) 
add_executable(hal_testsd ${SRCS_HAL_TESTS}) 

target_link_libraries(hal_tests tvhal_static gtest)
target_link_libraries(hal_testsd tvhal gtest)

install (TARGETS hal_tests  DESTINATION bin)
