
option(BUILD_DOCS "Build documents" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

option(ENABLE_GIF "enable gif encode and decoder" ON)
option(ENABLE_CAIROSVG "enable svg decoder" ON)
option(ENABLE_MBEDTLS "enable mbedtls" OFF)
option(ENABLE_UPNP "enable upnp/dlna" OFF)
option(ENABLE_GESTURE "enable gestrure" OFF)
option(ENABLE_TURBOJPEG "enable turbo jpeg" ON)

if(ENABLE_GIF)
  list(APPEND OPTIONAL_LIBS gif)
endif()

if(ENABLE_CAIROSVG)
  list(APPEND OPTIONAL_LIBS svg-cairo)
endif()

if(ENABLE_MBEDTLS)
endif()

if(ENABLE_UPNP)
  list(APPEND OPTIONAL_LIBS upnp)
endif()

if(ENABLE_GESTURE)
  list(APPEND OPTIONAL_LIBS grt)
endif()

if(ENABLE_TURBOJPEG)
  list(APPEND OPTIONAL_LIBS turbojpeg)
endif()

configure_file(src/gui/gui_features.h.cmake  ${CMAKE_BINARY_DIR}/include/gui/gui_features.h)
