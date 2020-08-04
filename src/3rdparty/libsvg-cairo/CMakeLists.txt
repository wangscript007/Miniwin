project(C CXX)

aux_source_directory(src SRCS_CAIRO_SVG)
include_directories(include src
	${CMAKE_SOURCE_DIR}/src/3rdparty/cairo-1.16.0/src
	${CMAKE_SOURCE_DIR}/src/3rdparty/libsvg/src
	)
set(CMAKE_C_FLAGS "-D__DO_SVG_DEBUG=1 -DDEBUG -O0 -g2 -fPIC")
#configure_file(src/svg-cairo-version.h.in ${PROJECT_SOURCE_DIR}/svg-cairo.version.h) 
add_library(svg-cairo  ${SRCS_CAIRO_SVG})
target_link_libraries(svg-cairo svg)

install(TARGETS svg-cairo DESTINATION lib)

