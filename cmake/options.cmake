
option(BUILD_DOCS "Build documents" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

option(ENABLE_GIF "enable gif encode and decoder" ON)
option(ENABLE_CAIROSVG "enable svg decoder" ON)
option(ENABLE_MBEDTLS "enable mbedtls" OFF)
option(ENABLE_UPNP "enable upnp/dlna" OFF)
option(ENABLE_GESTURE "enable gestrure" OFF)
option(ENABLE_TURBOJPEG "enable turbo jpeg" ON)
option(ENABLE_MP3ID3 "enable mp3 ids" ON)

option(WITH_JPEG8 "Emulate libjpeg v8 API/ABI (this makes ${CMAKE_PROJECT_NAME} backward-incompatible with libjpeg v6b)" ON)
option(FT_WITH_HARFBUZZ "Improve auto-hinting of OpenType fonts." OFF)

if(ENABLE_GIF)
  list(APPEND OPTIONAL_LIBS gif)
endif()

if(ENABLE_CAIROSVG)
  list(APPEND OPTIONAL_LIBS svg-cairo)
endif()

if(ENABLE_MBEDTLS)
endif()

if(ENABLE_UPNP)
  list(APPEND APP_EXTLIBS upnp)
endif()

if(ENABLE_GESTURE)
  list(APPEND OPTIONAL_LIBS grt)
endif()

if(ENABLE_TURBOJPEG)
  list(APPEND OPTIONAL_LIBS turbojpeg)
endif()

if(ENABLE_MP3ID3)
  list(APPEND APP_EXTLIBS id3)
endif()

configure_file(src/gui/gui_features.h.cmake  ${CMAKE_BINARY_DIR}/include/gui/gui_features.h)
