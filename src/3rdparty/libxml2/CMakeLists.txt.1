# Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia
# Licensed under the MIT License https://opensource.org/licenses/MIT

cmake_minimum_required(VERSION 3.5)

project(libxml2 C)

# If necessary use the vendored FetchContent
if(CMAKE_VERSION VERSION_LESS 3.11)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/cmake-3.12)
endif()
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/ycm-0.9)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Download libxml2 source code using FetchContent
include(FetchContent)
FetchContent_Declare(libxml2upstream
                     GIT_REPOSITORY https://gitlab.gnome.org/GNOME/libxml2.git
                     GIT_TAG        v2.9.8)

FetchContent_GetProperties(libxml2upstream)
if(NOT libxml2upstream_POPULATED)
  FetchContent_Populate(libxml2upstream)

  # Options
  option(BUILD_SHARED_LIBS "If on, build shared libraries, otherwise build static libraries." ON)

  macro(libxml2_option optionName optionDesc optionDefault)
    # Define actual option, with LIBXML2_ prefix to avoid conflicts when the project is added
    # with add_subdirectory in a bigger project
    option(LIBXML2_${optionName} ${optionDesc} ${optionDefault})

    # Depending on the value of the CMake function, set the ${libxml2_option} CMake variable
    # to 1 or 0 for correctly generating the xmlversion.h file
    if(LIBXML2_${optionName})
      set(${optionName} 1)
    else()
      set(${optionName} 0)
    endif()
  endmacro()

  # Options with a direct autotool-equivalent
  libxml2_option(WITH_C14N "add the Canonicalization support" ON)
  libxml2_option(WITH_CATALOG "add the Catalog support" ON)
  libxml2_option(WITH_DEBUG "add the debugging module" ON)
  libxml2_option(WITH_DOCBOOK "add Docbook SGML support" ON)

  # Define WITH_DOCB for compatibility with xmlversion.h.in
  if(LIBXML2_WITH_DOCBOOK)
    set(WITH_DOCB 1)
  else()
    set(WITH_DOCB 0)
  endif()

  libxml2_option(WITH_FEXCEPTIONS "add GCC flag -fexceptions for C++ exceptions" OFF)
  libxml2_option(WITH_FTP "add the FTP support" ON)
  libxml2_option(WITH_HISTORY "add history support to xmllint shell" OFF)
  libxml2_option(WITH_HTML "add the HTML support" ON)
  libxml2_option(WITH_HTTP "add the HTTP support" ON)
  libxml2_option(WITH_ICONV "add ICONV support" ON)
  libxml2_option(WITH_ICU "add ICU support" OFF)
  libxml2_option(WITH_ISO8859X "add ISO8859X support if no iconv" ON)
  libxml2_option(WITH_LEGACY "add deprecated APIs for compatibility" ON)
  libxml2_option(WITH_MEM_DEBUG "add the memory debugging module" OFF)
  libxml2_option(WITH_MINIMUM "build a minimally sized library" OFF)
  libxml2_option(WITH_OUTPUT "add the serialization support" ON)
  libxml2_option(WITH_PATTERN "add the xmlPattern selection interface" ON)
  libxml2_option(WITH_PUSH "add the PUSH parser interfaces" ON)
  libxml2_option(WITH_READER "add the xmlReader parsing interface" ON)
  libxml2_option(WITH_REGEXPS "add Regular Expressions support" ON)
  libxml2_option(WITH_RUN_DEBUG "add the runtime debugging module" OFF)
  libxml2_option(WITH_SAX1 "add the older SAX1 interface" ON)
  libxml2_option(WITH_SCHEMAS "add Relax-NG and Schemas support" ON)
  libxml2_option(WITH_SCHEMATRON " add Schematron support " ON)
  libxml2_option(WITH_THREADS "add multithread support" ON)
  libxml2_option(WITH_THREAD_ALLOC "add per-thread memory" OFF)
  libxml2_option(WITH_TREE "add the DOM like tree manipulation APIs" ON)
  libxml2_option(WITH_VALID "add the DTD validation support" ON)
  libxml2_option(WITH_WRITER "add the xmlWriter saving interface" ON)
  libxml2_option(WITH_XINCLUDE "add the XInclude support" ON)
  libxml2_option(WITH_XPATH "add the XPATH support" ON)
  libxml2_option(WITH_XPTR "add the XPointer support" ON)
  # TODO() Encode that XInclude and xptr support depends on Xpath
  libxml2_option(WITH_MODULES "add the dynamic modules support" ON)

  # Option on dependencies
  libxml2_option(WITH_ZLIB "add zlib support." ON)
  libxml2_option(WITH_LZMA "add lzma support." ON)

  # Internal option
  # TODO(traversaro): add trio support
  set(WITH_TRIO OFF CACHE BOOL "Adding trio library for string functions")

  # Dependencies
  if(WITH_ZLIB)
    find_package(ZLIB REQUIRED)
  endif()

  if(WITH_LZMA)
    find_package(LibLZMA REQUIRED)
  endif()

  # Copy platform headers in Windows, but generate it on other platforms
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    configure_file(${libxml2upstream_SOURCE_DIR}/include/win32config.h ${CMAKE_CURRENT_BINARY_DIR}/config.h COPYONLY)
  else()
    # Generate config.h in  ${CMAKE_CURRENT_BINARY_DIR}
    # (see https://stackoverflow.com/questions/38419876/cmake-generate-config-h-like-from-autoconf )
    include(GenerateConfigHeader)
  endif()

  # Glob public headers
  file(GLOB xml2_public_headers ${libxml2upstream_SOURCE_DIR}/include/libxml/*.h)

  # Generate xmlversion.h
  # TODO(traversaro): extract version info from exiting source
  set(VERSION "2.9.8")
  set(LIBXML_VERSION_NUMBER "20908")
  configure_file(${libxml2upstream_SOURCE_DIR}/include/libxml/xmlversion.h.in ${CMAKE_CURRENT_BINARY_DIR}/libxml/xmlversion.h)
  list(APPEND xml2_public_headers ${CMAKE_CURRENT_BINARY_DIR}/libxml/xmlversion.h)

  set(srcs ${libxml2upstream_SOURCE_DIR}/SAX.c
           ${libxml2upstream_SOURCE_DIR}/entities.c
           ${libxml2upstream_SOURCE_DIR}/encoding.c
           ${libxml2upstream_SOURCE_DIR}/error.c
           ${libxml2upstream_SOURCE_DIR}/parserInternals.c
           ${libxml2upstream_SOURCE_DIR}/parser.c
           ${libxml2upstream_SOURCE_DIR}/tree.c
           ${libxml2upstream_SOURCE_DIR}/hash.c
           ${libxml2upstream_SOURCE_DIR}/list.c
           ${libxml2upstream_SOURCE_DIR}/xmlIO.c
           ${libxml2upstream_SOURCE_DIR}/xmlmemory.c
           ${libxml2upstream_SOURCE_DIR}/uri.c
           ${libxml2upstream_SOURCE_DIR}/valid.c
           ${libxml2upstream_SOURCE_DIR}/xlink.c
           ${libxml2upstream_SOURCE_DIR}/HTMLparser.c
           ${libxml2upstream_SOURCE_DIR}/HTMLtree.c
           ${libxml2upstream_SOURCE_DIR}/debugXML.c
           ${libxml2upstream_SOURCE_DIR}/xpath.c
           ${libxml2upstream_SOURCE_DIR}/xpointer.c
           ${libxml2upstream_SOURCE_DIR}/xinclude.c
           ${libxml2upstream_SOURCE_DIR}/nanohttp.c
           ${libxml2upstream_SOURCE_DIR}/nanoftp.c
           ${libxml2upstream_SOURCE_DIR}/catalog.c
           ${libxml2upstream_SOURCE_DIR}/globals.c
           ${libxml2upstream_SOURCE_DIR}/threads.c
           ${libxml2upstream_SOURCE_DIR}/c14n.c
           ${libxml2upstream_SOURCE_DIR}/xmlstring.c
           ${libxml2upstream_SOURCE_DIR}/buf.c
           ${libxml2upstream_SOURCE_DIR}/xmlregexp.c
           ${libxml2upstream_SOURCE_DIR}/xmlschemas.c
           ${libxml2upstream_SOURCE_DIR}/xmlschemastypes.c
           ${libxml2upstream_SOURCE_DIR}/xmlunicode.c
           ${libxml2upstream_SOURCE_DIR}/xmlreader.c
           ${libxml2upstream_SOURCE_DIR}/relaxng.c
           ${libxml2upstream_SOURCE_DIR}/dict.c
           ${libxml2upstream_SOURCE_DIR}/SAX2.c
           ${libxml2upstream_SOURCE_DIR}/xmlwriter.c
           ${libxml2upstream_SOURCE_DIR}/legacy.c
           ${libxml2upstream_SOURCE_DIR}/chvalid.c
           ${libxml2upstream_SOURCE_DIR}/pattern.c
           ${libxml2upstream_SOURCE_DIR}/xmlsave.c
           ${libxml2upstream_SOURCE_DIR}/xmlmodule.c
           ${libxml2upstream_SOURCE_DIR}/schematron.c
           ${libxml2upstream_SOURCE_DIR}/xzlib.c)

  if(NOT WITH_XPATH)
    list(REMOVE_ITEM xml2_public_headers ${libxml2upstream_SOURCE_DIR}/include/libxml/xpath.h)
    list(REMOVE_ITEM srcs ${libxml2upstream_SOURCE_DIR}/xpath.c)
  endif()

  if(NOT WITH_XPTR)
    list(REMOVE_ITEM xml2_public_headers ${libxml2upstream_SOURCE_DIR}/include/libxml/xpointer.h)
    list(REMOVE_ITEM srcs ${libxml2upstream_SOURCE_DIR}/xpointer.c)
  endif()

  if(NOT WITH_XINCLUDE)
    list(REMOVE_ITEM xml2_public_headers ${libxml2upstream_SOURCE_DIR}/include/libxml/xinclude.h)
    list(REMOVE_ITEM srcs ${libxml2upstream_SOURCE_DIR}/xinclude.c)
  endif()


  add_library(xml2 ${srcs})

  # Add alias that matches the target exported in FindLibXml2.cmake, to simplify the usage of the library
  # with the FetchContent CMake module
  add_library(LibXml2::LibXml2 ALIAS xml2)

  target_include_directories(xml2 PUBLIC $<BUILD_INTERFACE:${libxml2upstream_SOURCE_DIR}/include>
                                         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

  if(WITH_THREADS)
    # TODO(check logic)
    target_compile_definitions(xml2 PRIVATE LIBXML_THREAD_ENABLED)
  endif()

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(xml2 PRIVATE -DLIBXML_STATIC)
  endif()

  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_include_directories(xml2 PRIVATE ${libxml2upstream_SOURCE_DIR}/win32/vc10)
    target_link_libraries(xml2 PRIVATE wsock32.lib ws2_32.lib)
    target_compile_definitions(xml2 PRIVATE -DHAVE_WIN32_THREADS)
  endif()

  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(xml2 PRIVATE m)
  endif()

  target_link_libraries(xml2 PRIVATE ${CMAKE_DL_LIBS})
  if(LIBXML2_WITH_ZLIB)
    target_link_libraries(xml2 PRIVATE ZLIB::ZLIB)
  endif()
  if(LIBXML2_WITH_LZMA)
    target_link_libraries(xml2 PRIVATE ${LIBLZMA_LIBRARIES})
  endif()

  # Public headers
  set_target_properties(xml2 PROPERTIES PUBLIC_HEADER "${xml2_public_headers}")

  include(GNUInstallDirs)
  install(TARGETS xml2
          EXPORT  ${PROJECT_NAME}
          LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libxml2/libxml")

  # Note that we do not install CMake imported targets, to remain compatible with
  # the libxml2 compiled by autotools: the generated libraries are expected to be
  # found by LibXml2.cmake available in cmake
  # However, for compatibility with the autotools build system we generate and install
  # the libxml2-config.cmake file
  configure_file(${libxml2upstream_SOURCE_DIR}/libxml2-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libxml2-config.cmake)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libxml2-config.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libxml2")


  # Add uninstall target
  include(AddUninstallTarget)
endif()
