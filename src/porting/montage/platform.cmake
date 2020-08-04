#set platform include dirs & library path
set(GX_ROOT $ENV{HOME}/gx3213/irdeto_cca/dvt/platform/library/arm-linux)
if ( TRUE )
	set(HAL_EXTRA_LIBS
           ${GX_ROOT}/lib/libgxcore.a
           CACHE INTERNAL "platform dependence libs")
	set(HAL_EXTRA_INCDIRS 
           ${GX_ROOT}/include/
           CACHE INTERNAL "platform dependence include")
else()
  find_package(GTK3)
  find_package(X11)
  if ( X11_FOUND )
	  set(HAL_EXTRA_LIBS ${X11_LIBRARIES} CACHE INTERNAL "platform dependence libs")
	  set(HAL_EXTRA_INCDIRS ${X11_INCLUDE_DIRS} CACHE INTERNAL "platform dependence include")
  elseif ( GTK3_FOUND )
	  set(HAL_EXTRA_LIBS ${GTK3_LIBRARIES} CACHE INTERNAL "platform dependence libs")
	  set(HAL_EXTRA_INCDIRS ${GTK3_INCLUDE_DIRS} CACHE INTERNAL "platform dependence include")
  endif()
endif()

