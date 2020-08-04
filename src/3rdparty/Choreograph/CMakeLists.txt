project(Choreograph C CXX)

file (GLOB_RECURSE SRCS_CHOREOGRAPH "src/*.cpp" "src/*.hpp")
include_directories(src 
   src/choreograph
   src/choreograph/phrase
   src/choreograph/detail
   src/choreograph/specialization
)

add_library(choreograph STATIC ${SRCS_CHOREOGRAPH})
set_target_properties(choreograph PROPERTIES POSITION_INDEPENDENT_CODE 1)

add_executable(chtest  ${SRCS_CHOREOGRAPH} main.cc)
#target_link_libraries(chtest choreograph)
#specialization  detail phrase   
file(GLOB HDS1  ${PROJECT_SOURCE_DIR}/src/choreograph/*.h*)
file(GLOB HDS2  ${PROJECT_SOURCE_DIR}/src/choreograph/specialization/*.h*)
file(GLOB HDS3  ${PROJECT_SOURCE_DIR}/src/choreograph/detail/*.h*)
file(GLOB HDS4  ${PROJECT_SOURCE_DIR}/src/choreograph/phrase/*.h*)

file(COPY ${HDS1}  DESTINATION ${CMAKE_BINARY_DIR}/include/gui/choreograph)
file(COPY ${HDS2}  DESTINATION ${CMAKE_BINARY_DIR}/include/gui/choreograph/specialization)
file(COPY ${HDS3}  DESTINATION ${CMAKE_BINARY_DIR}/include/gui/choreograph/detail)
file(COPY ${HDS4}  DESTINATION ${CMAKE_BINARY_DIR}/include/gui/choreograph/phrase)

