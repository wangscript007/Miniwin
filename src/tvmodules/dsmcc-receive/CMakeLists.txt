
project(dsmcc-receive C CXX)

set(CMAKE_CXX_STANDARD 11)


aux_source_directory(./ SRCS_DSMCC)
add_library(dsmcc-receive ${SRCS_DSMCC})
include_directories(./ 
   ${CMAKE_SOURCE_DIR}/include 
   ${CMAKE_BINARY_DIR}/include)

target_link_libraries(dsmcc-receive tvhal)

install (TARGETS dsmcc-receive  DESTINATION lib)
