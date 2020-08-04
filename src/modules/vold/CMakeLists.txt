project(usbmanager C CXX)

aux_source_directory(./ SRCS_USBMANAGER)

include_directories(./
    ${CMAKE_BINARY_DIR}/include/porting
)
set(CMAKE_CXX_FLAGS -fPIC)
add_library(usbmanager STATIC ${SRCS_USBMANAGER})

install(TARGETS usbmanager DESTINATION lib)


