project(respack C CXX)

set(CMAKE_CROSSCOMPILING false)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)

include_directories(  
  ./
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_BINARY_DIR}/include/jrtp
  ${CMAKE_SOURCE_DIR}/src/gui/core/
  ${CMAKE_SOURCE_DIR}/src/3rdparty/jsoncpp/include
  ${CMAKE_SOURCE_DIR}/src/3rdparty/rapidjson/include
  ./
)

find_package(ncurses)

link_directories(${CMAKE_PROJECT_DIR}/libs)

add_executable(lansplit lansplit.cc)

if (ncurses_FOUND)
   add_definitions(-DHAVE_NCURSES)
endif()

add_executable(getevent getevent.c)
add_executable(tssender tssender.cc)

target_link_libraries(tssender jrtplib-static ${ncurses_LIBS})

install(TARGETS tssender DESTINATION bin)

