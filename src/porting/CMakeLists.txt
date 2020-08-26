cmake_minimum_required (VERSION 3.2)
project(tvhal C CXX)

if( CMAKE_SYSTEM_NAME MATCHES "Linux" )
add_definitions(-DLINUX=1)
endif()

include(${NGL_CHIPSET}/platform.cmake)
message("NGL_CHIPSET=${NGL_CHIPSET}  USE_RFB_GRAPH=${USE_RFB_GRAPH}")
message("HAL_EXTRA_LIBS=${HAL_EXTRA_LIBS}")
message("CMAKE_LIBRARY_PATH=${CMAKE_LIBRARY_PATH}")
aux_source_directory(./ SRCS_COMMON)

if ( NOT DEFINED SOURCES_${NGL_CHIPSET} )  
   aux_source_directory(${NGL_CHIPSET} SOURCES_${NGL_CHIPSET})
else()
   message("#############################################")
   foreach( f ${SOURCES_${NGL_CHIPSET}} )
      list(APPEND SOURCES ${NGL_CHIPSET}/${f})
   endforeach(f)
   set(SOURCES_${NGL_CHIPSET} ${SOURCES})
endif()

if( ${USE_RFB_GRAPH} MATCHES "^ON|^YES" )
  set(RFB_GRAPH ON)
  set(HAL_EXTRA_LIBS ${HAL_EXTRA_LIBS} vncserver)
  message(" RFB(remote frame buffer) is selected to ${NGL_CHIPSET}")
else()
  message("RFB is off")
  list(REMOVE_ITEM SRCS_COMMON .//ngl_rfb.c)
endif()

if(NOT HAL_EXTRA_DLIBS)
  set(HAL_EXTRA_DLIBS ${HAL_EXTRA_LIBS})
endif()

include_directories(
        include
        ${NGL_CHIPSET}
        ${HAL_EXTRA_INCDIRS} 
	${CMAKE_BINARY_DIR}/src/3rdparty/libvncserver
	${CMAKE_SOURCE_DIR}/src/3rdparty/libvncserver)

add_library(tvhal SHARED ${SRCS_COMMON} ${SOURCES_${NGL_CHIPSET}})
add_library(tvhal_static STATIC ${SRCS_COMMON} ${SOURCES_${NGL_CHIPSET}})

target_link_libraries(tvhal ${HAL_EXTRA_DLIBS})
target_link_libraries(tvhal_static ${HAL_EXTRA_LIBS})
set_target_properties(tvhal_static PROPERTIES OUTPUT_NAME "tvhal")

add_subdirectory(tests)

file(GLOB HAL_HEADERS  ${PROJECT_SOURCE_DIR}/include/*.h)

install (TARGETS tvhal  DESTINATION lib)
install (TARGETS tvhal_static  DESTINATION lib)
install (FILES ${HAL_HEADERS} DESTINATION include/porting/)

file(COPY ${HAL_HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include/porting)
file(GLOB KEYMAPS   ${PROJECT_SOURCE_DIR}/${NGL_CHIPSET}/*.map)
file(COPY ${KEYMAPS} DESTINATION ${CMAKE_BINARY_DIR} PATTERN "*.map")

