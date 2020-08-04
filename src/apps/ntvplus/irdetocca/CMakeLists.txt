project(irdetocca C CXX)

#aux_source_directory(./ SRCS_IRDETO)
set(SRCS_IRDETO Spi_Version.c)
link_directories(./)

add_library(irdetocca STATIC ${SRCS_IRDETO})
target_link_libraries(irdetocca CloakedCAAgent)

