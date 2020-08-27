project(gui C CXX)

add_subdirectory(tests)

option( LUA_BINDINGS "UI Lua script bindings" OFF)
option( GUI_STATIC "UI is static link" ON)

aux_source_directory(core SRCS_GUICORE)
aux_source_directory(views SRCS_VIEWS)
#aux_source_directory(core/svg SRCS_SVG)
aux_source_directory(cairomm   SRCS_CAIROMM)
aux_source_directory(dialogs SRCS_DIALOG)
aux_source_directory(looper SRCS_LOOP)

message("UI Lua script bindings is ${LUA_BINDINGS} guistatic=${GUI_STATIC}")

if ( LUA_BINDINGS )
    aux_source_directory(luabinding SRCS_LUABIND)
    list(APPEND EXT_LIBS lua)
endif ()

include_directories(
    ./ ./private 
    ./core 
    ./views 
    ./cairomm 
    ./core/svg ./luabinding 
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/include/pixman
    ${CMAKE_BINARY_DIR}/include/cairo
    ${CMAKE_BINARY_DIR}/include/porting
    ${CMAKE_SOURCE_DIR}/src/3rdparty/libunibreak/src
    ${CMAKE_SOURCE_DIR}/src/modules/vold
    ${CMAKE_SOURCE_DIR}/src/3rdparty/grt
    ${CMAKE_SOURCE_DIR}/src/3rdparty/libsvg/src
    ${CMAKE_SOURCE_DIR}/src/3rdparty/libsvg-cairo/src
    ${CMAKE_SOURCE_DIR}/src/3rdparty/libsvg-cairo/include
    ${CMAKE_SOURCE_DIR}/src/3rdparty/Choreograph/src/
)

set(SOURCES_GUI ${SRCS_GUICORE} ${SRCS_VIEWS} ${SRCS_CAIROMM} ${SRCS_DIALOG} ${SRCS_LUABIND} ${SRCS_LOOP} ${SRCS_SVG})

add_library(gui_objs OBJECT ${SOURCES_GUI})
add_library(gui_static STATIC ${SOURCES_GUI})
add_library(gui SHARED ${SOURCES_GUI})

set_target_properties(gui_static PROPERTIES OUTPUT_NAME "gui")

file(GLOB MMHEADERS   ${PROJECT_SOURCE_DIR}/cairomm/*.h)
file(GLOB GUIHEADERS  ${PROJECT_SOURCE_DIR}/core/*.h)
file(GLOB VIEWSHEADERS  ${PROJECT_SOURCE_DIR}/views/*.h)
file(GLOB LOOPERHEADERS  ${PROJECT_SOURCE_DIR}/looper/*.h)

target_link_libraries(gui pixman-1 png jpeg freetype cairo unibreak 
    usbmanager zip tvhal choreograph ${OPTIONAL_LIBS})
target_link_libraries(gui_static pixman-1 png jpeg cairo_static freetype unibreak 
   zip usbmanager tvhal_static choreograph ${OPTIONAL_LIBS})

file(COPY ${GUIHEADERS} ${VIEWSHEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include/gui PATTERN "*.h")
file(COPY ${MMHEADERS}     DESTINATION ${CMAKE_BINARY_DIR}/include/gui/cairomm PATTERN "*.h")
file(COPY ${LOOPERHEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include/looper PATTERN "*.h")

install (TARGETS gui  DESTINATION lib)
install (FILES ${MMHEADERS} DESTINATION include/gui/cairomm/)
install (FILES ${GUIHEADERS} DESTINATION include/gui/)
install (FILES ${VIEWSHEADERS} DESTINATION include/gui/)
install (FILES ${LOOPERHEADERS} DESTINATION include/looper)

