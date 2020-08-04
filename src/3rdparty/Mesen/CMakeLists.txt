project(mesen C CXX)
set(CMAKE_CXX_STANDARD 11)
file(GLOB_RECURSE MESEN_SRCS 
    Core/*.c Core/*.cpp  
    SevenZip/*.c*
    Utilities/*.cpp
#   Utilities/SimpleLock.cpp 
#   Utilities/AutoResetEvent.cpp
#   Utilities/ArchiveReader.cpp
#   Utilities/ZipReader.cpp
#   Utilities/Socket.cpp
#   Utilities/Timer.cpp
#   Utilities/CRC32.cpp
#   Utilities/IpsPatcher.cpp
   Miniwin/*.cc
)

include_directories(Core SevenZip Utilities 
  Miniwin 
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_BINARY_DIR}/include/porting
  ${CMAKE_BINARY_DIR}/include/gui
  ${CMAKE_BINARY_DIR}/include/cairo
  ${CMAKE_BINARY_DIR}/include/lua
)
add_definitions(-DLIBRETRO)
add_executable(mesen ${MESEN_SRCS})
target_link_libraries(mesen xlua gui)

