# Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia
# Licensed under the MIT License https://opensource.org/licenses/MIT

# Check if headers are available
include(CheckIncludeFiles)
check_include_files(arpa/inet.h HAVE_ARPA_INET_H)
check_include_files(arpa/nameser.h HAVE_ARPA_NAMESER_H)
check_include_files(ctype.h HAVE_CTYPE_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(dl.h HAVE_DL_H)
check_include_files(errno.h HAVE_ERRNO_H)
check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(float.h HAVE_FLOAT_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(limits.h HAVE_LIMITS_H)
check_include_files(lzma.h HAVE_LZMA_H)
check_include_files(malloc.h HAVE_MALLOC_H)
check_include_files(math.h HAVE_MATH_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(netdb.h HAVE_NETDB_H)
check_include_files(netinet/in.h HAVE_NETINET_IN_H)
check_include_files(poll.h HAVE_POLL_H)
check_include_files(pthread.h HAVE_PTHREAD_H)
check_include_files(resolv.h HAVE_RESOLV_H)
check_include_files(signal.h HAVE_SIGNAL_H)
check_include_files(stdarg.h HAVE_STDARG_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/mman.h HAVE_SYS_MMAN_H)
check_include_files(sys/select.h HAVE_SYS_SELECT_H)
check_include_files(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/timeb.h HAVE_SYS_TIMEB_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(time.h HAVE_TIME_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(zlib.h HAVE_ZLIB_H)
# Simplified STDC_HEADERS, based on AC_HEADER_STDC autoconf macro
# https://www.gnu.org/software/autoconf/manual/autoconf-2.67/html_node/Particular-Headers.html
check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

# HAVE_PTHREAD_H is set to 1 only if the WITH_THREADS option is enabled
if(NOT LIBXML2_WITH_THREADS)
  set(HAVE_PTHREAD_H "" CACHE INTERNAL "Have include pthread.h")
endif()


# Check if symbols are available
include(CheckSymbolExists)
check_symbol_exists(dlopen dlfcn.h HAVE_DLOPEN)
check_symbol_exists(fprintf stdio.h HAVE_FPRINTF)
check_symbol_exists(ftime sys/timeb.h HAVE_FTIME)
check_symbol_exists(getaddrinfo "sys/types.h;sys/socket.h;netdb.h" HAVE_GETADDRINFO)
check_symbol_exists(gettimeofday sys/time.h HAVE_GETTIMEOFDAY)
check_symbol_exists(isascii ctype.h HAVE_ISASCII)
check_symbol_exists(isinf math.h HAVE_ISINF)
check_symbol_exists(isnan math.h HAVE_ISNAN)
check_symbol_exists(localtime time.h HAVE_LOCALTIME)
check_symbol_exists(mmap sys/mman.h HAVE_MMAP)
check_symbol_exists(munmap sys/mman.h HAVE_MUNMAP)
check_symbol_exists(printf stdio.h HAVE_PRINTF)
check_symbol_exists(putenv stdlib.h HAVE_PUTENV)
check_symbol_exists(rand stdlib.h HAVE_RAND)
check_symbol_exists(rand_r stdlib.h HAVE_RAND_R)
check_symbol_exists(signal signal.h HAVE_SIGNAL)
check_symbol_exists(snprintf stdio.h HAVE_SNPRINTF)
check_symbol_exists(sprintf stdio.h HAVE_SPRINTF)
check_symbol_exists(srand stdlib.h HAVE_SRAND)
check_symbol_exists(sscanf stdio.h HAVE_SSCANF)
check_symbol_exists(stat sys/stat.h HAVE_STAT)
check_symbol_exists(stfrtime time.h HAVE_STRFTIME)
check_symbol_exists(DIR sys/dir.h HAVE_SYS_DIR_H)
check_symbol_exists(DIR sys/ndir.h HAVE_SYS_NDIR_H)
check_symbol_exists(time time.h HAVE_TIME)
check_symbol_exists(va_copy stdarg.h HAVE_VA_COPY)
check_symbol_exists(__va_copy stdarg.h HAVE___VA_COPY)
check_symbol_exists(vfprintf "stdarg.h;stdio.h" HAVE_VFPRINTF)
check_symbol_exists(vsnprintf "stdarg.h;stdio.h" HAVE_VSNPRINTF)
check_symbol_exists(vsprintf "stdarg.h;stdio.h" HAVE_VSPRINTF)

# Misc checks

# on some platforms (like AIX 5L), the structure sockaddr doesn't have
# a ss_family member, but rather __ss_family. Let's detect that
# and define the HAVE_BROKEN_SS_FAMILY when we are on one of these
# platforms.  However, we should only do this if ss_family is not
# present.
include(CheckStructHasMember)
set(HAVE_BROKEN_SS_FAMILY_DESCRIPTION "Whether struct sockaddr::__ss_family exists")
set(HAVE_BROKEN_SS_FAMILY "" CACHE INTERNAL ${HAVE_BROKEN_SS_FAMILY_DESCRIPTION})
check_struct_has_member("struct sockaddr_storage" ss_family "sys/types.h;sys/socket.h" HAVE_SOCKADDR_STORAGE_SS_FAMILY LANGUAGE C)
if(NOT HAVE_SOCKADDR_STORAGE_SS_FAMILY)
  check_struct_has_member("struct sockaddr_storage" __ss_family "sys/types.h;sys/socket.h" HAVE_SOCKADDR_STORAGE___SS_FAMILY LANGUAGE C)
  if(HAVE_SOCKADDR_STORAGE___SS_FAMILY)
    set(HAVE_BROKEN_SS_FAMILY "1" CACHE INTERNAL ${HAVE_BROKEN_SS_FAMILY_DESCRIPTION})
    set(ss_family "__ss_family")
  endif()
endif()

configure_file(config.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
