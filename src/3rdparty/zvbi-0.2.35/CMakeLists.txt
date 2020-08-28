project(zvbi C)

set(SRCS_ZVBI
    src/bit_slicer.c
    src/cache.c
    src/caption.c 
    src/cc608_decoder.c
    src/conv.c
    src/dvb_demux.c
    src/event.c
    src/exp-html.c
    src/exp-templ.c
    src/exp-txt.c
    src/exp-vtx.c
    src/exp-gfx.c
    src/export.c
    src/hamm.c
    src/idl_demux.c
    src/io-bktr.c
    src/io-dvb.c
    src/io-sim.c
    src/io-v4l.c
    src/io-v4l2.c
    src/io-v4l2k.c
    src/io.c
    src/lang.c
    src/misc.c
    src/packet.c 
    src/teletext.c
    src/packet-830.c
    src/page_table.c
    src/pdc.c
    src/pfc_demux.c
    src/proxy-client.c
    src/raw_decoder.c
    src/sampling_par.c
    src/search.c
    src/ure.c
    src/sliced_filter.c
    src/tables.c
    src/trigger.c
    src/vbi.c
    src/vps.c
    src/wss.c
    src/xds_demux.c
)
include_directories(
    ./
    .src/
    ${CMAKE_BINARY_DIR}/include/    
)

add_definitions(-DPACKAGE="zvbi" -DVERSION="0.2.35" -Dxmalloc=malloc
    -DHAVE_LIBPNG
)

add_library(zvbi ${SRCS_ZVBI})
add_executable(vbitest test/test-dvb_demux.cc)

target_link_libraries(zvbi iconv pthread png)
target_link_libraries(vbitest zvbi)
install(TARGETS zvbi DESTINATION lib)
install(TARGETS vbitest DESTINATION bin/tests)



