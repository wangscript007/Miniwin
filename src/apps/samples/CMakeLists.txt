cmake_minimum_required (VERSION 2.6)
project(gui_test C CXX)

set(CMAKE_CXX_STANDARD 11)

include_directories(
   ${CMAKE_BINARY_DIR}/include
   ${CMAKE_BINARY_DIR}/include/gui
   ${CMAKE_BINARY_DIR}/include/porting
   ${CMAKE_BINARY_DIR}/include/cairo
   ${CMAKE_BINARY_DIR}/include/epg
)
set(CMAKE_CXX_FLAGS "-Wl,--copy-dt-needed-entries")  #prevent error adding symbols: DSO missing from command line

file(GLOB ExamplesFileList "*.cc")
add_custom_target(build_app_examples COMMENT A target that requires all the examples.)
foreach(Example ${ExamplesFileList})
  message(STATUS "\tCreating build rule for ${Example}")
  # Get the name without extension or directory
  get_filename_component(ExampleName ${Example} NAME_WE)
  # Define example executable
  add_executable(${ExampleName} ${Example})
  # Link example against curlpp
  target_link_libraries(${ExampleName} gui_static)
  # make the meta target depend on this example.
  add_dependencies(build_app_examples ${ExampleName})
  install(TARGETS ${ExampleName} DESTINATION bin/examples)
endforeach(Example ${ExamplesFileList})

