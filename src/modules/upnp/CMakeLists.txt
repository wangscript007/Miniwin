project(dlna C CXX)

aux_source_directory(./ SRCS_USBMANAGER)

include_directories(
    ./ 
    ${CMAKE_BINARY_DIR}/include/porting
    ${CMAKE_SOURCE_DIR}/3rdparty/libUPnP/Neptune/Source/Core
    ${CMAKE_SOURCE_DIR}/3rdparty/libUPnP/Platinum/Source/Core
    ${CMAKE_SOURCE_DIR}/3rdparty/libUPnP/Platinum/Source/Platinum
    ${CMAKE_SOURCE_DIR}/3rdparty/Platinum/Source/Devices/MediaConnect
    ${CMAKE_SOURCE_DIR}/3rdparty/Platinum/Source/Devices/MediaRenderer
    ${CMAKE_SOURCE_DIR}/3rdparty/Platinum/Source/Devices/MediaServer
)
set(CMAKE_CXX_FLAGS -fPIC)
add_library(dlna STATIC ${SRCS_USBMANAGER})
target_link_libraries(dlna upnp)

install(TARGETS dlna DESTINATION lib)


