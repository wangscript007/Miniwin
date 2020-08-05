cmake_minimum_required (VERSION 3.2)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_CXX_STANDARD 11)

include(cmake/options.cmake)

set(CAIRO_HAS_NGL_SURFACE TRUE)

set(CMAKE_INSTALL_MANDIR ${CMAKE_BINARY_DIR}/man)
set(CMAKE_INSTALL_DEFAULT_PREFIX ${CMAKE_BINARY_DIR})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
   add_definitions(-DDEBUG) 
endif()
message("BuildType=${CMAKE_BUILD_TYPE} OPTIONAL_LIBS=${OPTIONAL_LIBS}")

add_subdirectory(src/gui)
add_subdirectory(src/modules)
add_subdirectory(src/porting)
add_subdirectory(src/apps)
add_subdirectory(src/3rdparty)
add_subdirectory(src/tvmodules)

if(NOT CMAKE_CROSSCOMPILING)
  add_subdirectory(src/tools)
endif()

if(BUILD_DOCS)
include(cmake/build_doxygen.cmake)
build_doxygen()
endif()

set(CMAKE_EXE_LINKER_FLAGS "-Xlinker --no-allow-shlib-undefined ${CMAKE_EXE_LINKER_FLAGS}")
