#cmake_minimum_required (VERSION 3.8)
project(epgtest C CXX)

aux_source_directory(./ SRCS_EPG_TESTS)

include_directories(./
    ${CMAKE_BINARY_DIR}/include/porting
   )

add_executable(epgtests ${SRCS_EPG_TESTS}) 

target_link_libraries(epgtests  tvhal_static dvbepg_static gtest)

install (TARGETS epgtests DESTINATION bin/tests)
